// $Id: HPDIonFeedbackMoni.cpp,v 1.5 2009-05-02 12:19:01 ryoung Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "HPDIonFeedbackMoni.h"
#include "RichKernel/RichSmartIDCnv.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <time.h>

using namespace Rich::Mon;

//-----------------------------------------------------------------------------
// Last updated 26 Jun 2008
//
// 2008-11-14 : Ross Young
// Implementation file for class : HPDIonFeedbackMoni
//
// Based on:
// Implementation file for class : RichHPDClusterAnalysis
// 2007-08-24 : Nicholas Styles
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HPDIonFeedbackMoni );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HPDIonFeedbackMoni::HPDIonFeedbackMoni( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : Rich::HistoAlgBase( name , pSvcLocator )
{
  declareProperty("IFBSize",      m_ionFeedbackCut = 3      );
  declareProperty("IFBSizeALICE", m_ionFeedbackCutALICE = 5 );
  declareProperty("MonitorRate",  m_MonitorRate = 1         );
  declareProperty("ReferenceDay", m_dayref = 14365          ); // 1st May 2009
}

//=============================================================================
// Destructor
//=============================================================================
HPDIonFeedbackMoni::~HPDIonFeedbackMoni() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HPDIonFeedbackMoni::initialize()
{
  const StatusCode sc = Rich::HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  //Tools to get the data and to analyse it for clusters
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );
  acquireTool( "RichSmartIDTool", m_idTool,            0, true );
  acquireTool( "RichHPDPixelClusterFinder", m_clusterTool, this );

  m_rich1flag = false; // Set to true if analysed data is from RICH1
  m_rich2flag = false; // Set to true if analysed data is from RICH2
  
  m_nEvts = 0;
  m_nMonitoredEvents = 0;
  
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HPDIonFeedbackMoni::execute()
{

 m_nEvts++;
  //
  // check if event should be monitored
  //
  if ( m_nEvts%m_MonitorRate != 0) {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "do not monitor event number " << m_nEvts << endmsg;
    return StatusCode::SUCCESS;
  } // if monRate
    


  const Rich::DAQ::L1Map & allSmartIDs = m_SmartIDDecoder->allRichSmartIDs();

  // Check number of hits
  if ( allSmartIDs.empty() ) { return StatusCode::SUCCESS; }

  m_nMonitoredEvents++;

  // --- Loop over L1 boards
  for ( Rich::DAQ::L1Map::const_iterator iL1 = allSmartIDs.begin();
        iL1 != allSmartIDs.end(); ++iL1 )
  {
    // --- loop over ingresses for this L1 board
    for ( Rich::DAQ::IngressMap::const_iterator iIn = (*iL1).second.begin();
          iIn != (*iL1).second.end(); ++iIn )
    {
      // --- Loop over HPDs in this ingress
      for ( Rich::DAQ::HPDMap::const_iterator iAllSmartIDs= (*iIn).second.hpdData().begin();
            iAllSmartIDs != (*iIn).second.hpdData().end(); ++iAllSmartIDs )
      {
        // ---HPD--- Initialise for this HPD
        const LHCb::RichSmartID          smartIDHPD  = (*iAllSmartIDs).second.hpdID(); // SmartID of HPD that got the hits
        // Is HPD OK
        if ( (*iAllSmartIDs).second.header().inhibit() ) { continue; }
        if ( !smartIDHPD.isValid() )                     { continue; }
        const LHCb::RichSmartID::Vector  &smartIDHits = (*iAllSmartIDs).second.smartIDs();  // SmartID array of hits in HPD
        // do we have any hits ?
        if ( smartIDHits.empty() ) continue;

        const Rich::Side PanNum          = smartIDHPD.panel (); //Retrieve The RICH panel.
        const Rich::DetectorType RichNum = smartIDHPD.rich (); //  Retrieve The RICH Detector.

        const unsigned int HPDCol = smartIDHPD.hpdCol();
        const unsigned int HPDRow = smartIDHPD.hpdNumInCol();

        //const int nHits = smartIDHits.size(); // number of hits for this HPD $$$ Not Used?

        const bool aliceMode     = (*iAllSmartIDs).second.header().aliceMode();
        const int ionFeedbackCut = ( aliceMode ? m_ionFeedbackCutALICE : m_ionFeedbackCut );

        LHCb::RichSmartID::Vector hitMap(smartIDHits); // need local copy of hits to prevent altering the originals
        const Rich::HPDPixelClusters * clusters = m_clusterTool->findClusters(hitMap);
        // note we own this object, so must delete clusters when finished with it !!

        const Rich::HPDPixelClusters::Cluster::PtnVector& clusVec = clusters->clusters();

        if(RichNum==Rich::Rich2){
          m_rich2flag = true;
          if(PanNum == 0) {
            int nClustTotRich2PanZ = 0;
            int nClustIFBRich2PanZ = 0;
            for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
                   clusVec.begin(); iClust != clusVec.end(); ++iClust) {
              int clusterSize = (*(*iClust)).size();
              if (clusterSize>0) {
                ++nClustTotRich2PanZ;
              if (clusterSize >= ionFeedbackCut) ++nClustIFBRich2PanZ;
              }
            }
            plot2D((HPDCol+9),(15-HPDRow),"Rich2TotClus","Rich2 #Tot clusters", -0.5, 18.5, -0.5, 15.5, 19, 16, nClustTotRich2PanZ);
            plot2D((HPDCol+9),(15-HPDRow),"Rich2IFBClus","Rich2 #IFB clusters", -0.5, 18.5, -0.5, 15.5, 19, 16, nClustIFBRich2PanZ);
          }else{
            int nClustTotRich2PanNZ = 0;
            int nClustIFBRich2PanNZ = 0;
            for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
                   clusVec.begin(); iClust != clusVec.end(); ++iClust) {
              int clusterSize = (*(*iClust)).size();
              if (clusterSize>0) {
                ++nClustTotRich2PanNZ;
              if (clusterSize >= ionFeedbackCut) ++nClustIFBRich2PanNZ;
              }
            }
            plot2D(HPDCol,(15-HPDRow),"Rich2TotClus","Rich2 #Tot clusters", -0.5, 18.5, -0.5, 15.5, 19, 16, nClustTotRich2PanNZ);
            plot2D(HPDCol,(15-HPDRow),"Rich2IFBClus","Rich2 #IFB clusters", -0.5, 18.5, -0.5, 15.5, 19, 16, nClustIFBRich2PanNZ);
          }
        } else {  
          m_rich1flag = true;
          if(PanNum == 0) {
            int nClustTotRich1PanZ = 0;
            int nClustIFBRich1PanZ = 0;
            for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
                   clusVec.begin(); iClust != clusVec.end(); ++iClust) {
              int clusterSize = (*(*iClust)).size();
              if (clusterSize>0) {
                ++nClustTotRich1PanZ;
              if (clusterSize >= ionFeedbackCut) ++nClustIFBRich1PanZ;
              }
            }
            plot2D((13-HPDRow),(13-HPDCol),"Rich1TotClus","Rich1 #Tot clusters", -0.5, 14.5, -0.5, 13.5, 15, 14, nClustTotRich1PanZ);
            plot2D((13-HPDRow),(13-HPDCol),"Rich1IFBClus","Rich1 #IFB clusters", -0.5, 14.5, -0.5, 13.5, 15, 14, nClustIFBRich1PanZ);
          }else{
            int nClustTotRich1PanNZ = 0;
            int nClustIFBRich1PanNZ = 0;
            for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
                   clusVec.begin(); iClust != clusVec.end(); ++iClust) {
              int clusterSize = (*(*iClust)).size();
              if (clusterSize>0) {
                ++nClustTotRich1PanNZ;
              if (clusterSize >= ionFeedbackCut) ++nClustIFBRich1PanNZ;
              }
            }
            plot2D(HPDRow,HPDCol,"Rich1TotClus","Rich1 #Tot clusters", -0.5, 14.5, -0.5, 13.5, 15, 14, nClustTotRich1PanNZ);
            plot2D(HPDRow,HPDCol,"Rich1IFBClus","Rich1 #IFB clusters", -0.5, 14.5, -0.5, 13.5, 15, 14, nClustIFBRich1PanNZ);
          }
        }

        // clean up
        delete clusters;

      } // ----- End of Loop over each HPD in this ingress
    } // ----- End of Loop over ingresses for this L1 board
  } // ----- End of Loop over L1 boards
 
  // --------------------------------------------
  // ---------- Event processing over -----------
  // --------------------------------------------

  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode HPDIonFeedbackMoni::finalize()
{

  time_t seconds;
  seconds = time (NULL);
  int m_days = (int)(seconds/(24*3600)) - m_dayref; // Elapsed #days from reference date 

  if(msgLevel(MSG::VERBOSE))
  {
  verbose() << "elapsed days " << m_days << endmsg;
  verbose() << "monitored events " << m_nMonitoredEvents << endmsg;
  }
  
  // Extra information for saveset - no. of sampled events; date of measurement; Tot/IFB (1/2) histo ID
  if(m_rich1flag){
    plot2D(14,0,"Rich1TotClus","Rich1 #Tot clusters",-0.5, 14.5, -0.5, 13.5, 15, 14, m_nMonitoredEvents);
    plot2D(14,0,"Rich1IFBClus","Rich1 #IFB clusters",-0.5, 14.5, -0.5, 13.5, 15, 14, m_nMonitoredEvents);
    plot2D(14,1,"Rich1TotClus","Rich1 #Tot clusters",-0.5, 14.5, -0.5, 13.5, 15, 14, m_days);
    plot2D(14,1,"Rich1IFBClus","Rich1 #IFB clusters",-0.5, 14.5, -0.5, 13.5, 15, 14, m_days);
    plot2D(14,2,"Rich1TotClus","Rich1 #Tot clusters",-0.5, 14.5, -0.5, 13.5, 15, 14, 1);
    plot2D(14,2,"Rich1IFBClus","Rich1 #IFB clusters",-0.5, 14.5, -0.5, 13.5, 15, 14, 2);
  }
  if(m_rich2flag){
    plot2D(18,0,"Rich2TotClus","Rich2 #Tot clusters",-0.5, 18.5, -0.5, 15.5, 19, 16, m_nMonitoredEvents);
    plot2D(18,0,"Rich2IFBClus","Rich2 #IFB clusters",-0.5, 18.5, -0.5, 15.5, 19, 16, m_nMonitoredEvents);
    plot2D(18,1,"Rich2TotClus","Rich2 #Tot clusters",-0.5, 18.5, -0.5, 15.5, 19, 16, m_days);
    plot2D(18,1,"Rich2IFBClus","Rich2 #IFB clusters",-0.5, 18.5, -0.5, 15.5, 19, 16, m_days);
    plot2D(18,2,"Rich2TotClus","Rich2 #Tot clusters",-0.5, 18.5, -0.5, 15.5, 19, 16, 1);
    plot2D(18,2,"Rich2IFBClus","Rich2 #IFB clusters",-0.5, 18.5, -0.5, 15.5, 19, 16, 2); 
  }
  
  return StatusCode::SUCCESS;
}
