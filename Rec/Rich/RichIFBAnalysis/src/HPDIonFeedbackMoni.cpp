// $Id: HPDIonFeedbackMoni.cpp,v 1.7 2010-01-31 10:56:51 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "HPDIonFeedbackMoni.h"
#include "RichKernel/RichSmartIDCnv.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <math.h>
#include <stdio.h>

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
  declareProperty("MonitorRate",  m_MonitorRate = 1         );
  declareProperty("EventSize",    m_interval = 3000000      );
  declareProperty("IonFeedbackALICE",       m_ionFeedbackCutALICE=5);
  declareProperty("IonFeedbackLHCB",       m_ionFeedbackCutLHCB=5);
  declareProperty("WantIFB",      m_wantIFB = true  );
  declareProperty("WantHitmaps", m_wantHitmaps = true       );
  declareProperty("WantQuickHitmap",      m_wantQuickHitmap = false  );
  declareProperty("SuppressCornerPixels", m_suppresscornpix = false );
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
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  //Tools to get the data and to analyse it for clusters
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );
  acquireTool( "RichSmartIDTool", m_idTool,            0, true );
  acquireTool( "RichHPDPixelClusterFinder", m_clusterTool, this );

  m_nEvts = 0;
  m_nMonitoredEvents = 0;
  dataset = 0;
  r1clustersThisEvent = 0;
  r2clustersThisEvent = 0;

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
  if ( ((int)(m_nEvts/m_interval) % m_MonitorRate)!=0){
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "do not monitor event number " << m_nEvts << endmsg;
    return StatusCode::SUCCESS;
  } // if monRate

  const Rich::DAQ::L1Map & allSmartIDs = m_SmartIDDecoder->allRichSmartIDs();

  // Check number of hits
  if ( allSmartIDs.empty() ) { return StatusCode::SUCCESS; }

  m_nMonitoredEvents++;

  // -- increment "Event" histogram by 1
  std::stringstream ss;
  ss << "Events_" << dataset;
  plot1D( 1, ss.str(), ss.str(), 0, 10, 10);

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
        const bool aliceMode     = (*iAllSmartIDs).second.header().aliceMode();


        if(m_suppresscornpix) {
          // This should be updated
          /*const LHCb::RichSmartID::Vector  &pixHits(SuppressCornerPixels(smartIDHits, aliceMode);
            Create new array of pixel hits without corner pixels
            else const LHCb::RichSmartID::Vector  &pixHits = smartIDHits; */
        }

        if(m_wantQuickHitmap){
          ExecuteQuickHitmap(RichNum, smartIDHits);
          m_wantIFB = false;
        }

        if(m_wantIFB){
          unsigned int ionFeedbackCut = ( aliceMode ? m_ionFeedbackCutALICE : m_ionFeedbackCutLHCB);
          ExecuteIFB(RichNum, PanNum, HPDCol, HPDRow, ionFeedbackCut, smartIDHits);
        }

      } // ----- End of Loop over each HPD in this ingress
    } // ----- End of Loop over ingresses for this L1 board
  } // ----- End of Loop over L1 boards

  if(r1clustersThisEvent>0) {      // If clusters were found in RICH1
    std::stringstream ss;
    ss << "R1ClusOcc_" << dataset;
    plot1D(r1clustersThisEvent,ss.str(),ss.str(),-0.5,3000,3001);
    r1clustersThisEvent = 0;
  }

  if(r2clustersThisEvent>0) {      // If clusters were found in RICH2
    std::stringstream ss;
    ss << "R2ClusOcc_" << dataset;
    plot1D(r2clustersThisEvent,ss.str(),ss.str(),-0.5,3000,3001);
    r2clustersThisEvent = 0;
  }

  if((m_nEvts%m_interval)==0){       // Increment index at user-defined interval
    dataset++;
    debug() << "Evt at dataset increase: " << m_nEvts << endmsg;
  }

  return StatusCode::SUCCESS;
  // --------------------------------------------
  // ---------- Event processing over -----------
  // --------------------------------------------

}

//-----------------------------------------------
//Ion Feedback Algorithms
//-----------------------------------------------

void HPDIonFeedbackMoni::ExecuteIFB(const Rich::DetectorType RichNum, const Rich::Side PanNum, const unsigned int HPDCol,
                                    const unsigned int HPDRow, unsigned int ionfeedbackCut,
                                    const LHCb::RichSmartID::Vector  &smartIDHits )
{
  int rich = (int)RichNum + 1;

  LHCb::RichSmartID::Vector hitMap(smartIDHits); // need local copy of hits to prevent altering the originals

  const Rich::HPDPixelClusters * clusters = m_clusterTool->findClusters(hitMap);
  const Rich::HPDPixelClusters::Cluster::PtnVector& clusVec = clusters->clusters();

  for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
         clusVec.begin(); iClust != clusVec.end(); ++iClust)
  {
    unsigned int clusterSize = (*(*iClust)).size();

    if(clusterSize>0){     // If a cluster of non-zero pixel size is observed
      std::stringstream ss;
      ss << "R" << rich << "_Pan" << (int)PanNum << "_Col" << HPDCol << "_Row" << HPDRow << "_" << dataset;
      std::string histid = ss.str();
      //Syntax: (value, ID, title, min, max, bins)
      plot1D( clusterSize-0.5, histid, histid, 0, 300, 300);

      if(rich==1) r1clustersThisEvent++;      // Increment #RICH1 clusters
      if(rich==2) r2clustersThisEvent++;      // Increment #RICH1 clusters


      // ---Clus--- Note down the cluster for histograms

      if(m_wantHitmaps){   // If we want to plot hitmaps (CPU-intensive!)
        //std::cout<<"Filled Panels"<<std::endl;
        const Rich::HPDPixelCluster& pixVec = (*(*iClust)).pixels();
        //Rich::HPDPixelCluster & pixVec = (*(*iClust)).pixels();

        const Rich::SmartIDGlobalOrdering limits(smartIDHits.front());

        minX = limits.minGlobalPixelX();
        maxX = limits.maxGlobalPixelX();
        minY = limits.minGlobalPixelY();
        maxY = limits.maxGlobalPixelY();
        binsX = limits.totalNumInGlobalX();
        binsY = limits.totalNumInGlobalY();

        std::stringstream ssAll, ssIFB;
        ssAll << "R" << rich << "AllClusHit_" << dataset;
        ssIFB << "R" << rich << "IFBClusHit_" << dataset;
        const std::string & histidAll = ssAll.str();
        const std::string & histidIFB = ssIFB.str();


        for( LHCb::RichSmartID::Vector::const_iterator iP =
               pixVec.smartIDs().begin(); iP != pixVec.smartIDs().end(); ++iP){
          const Rich::SmartIDGlobalOrdering order(*iP); // ordering object - Chris Jones' plotCoordinate tool

          const std::string RichNum = Rich::text( (*iP).rich() );

          plot2D( order.globalPixelX(), order.globalPixelY(),                 //PlotX, PlotY
                  histidAll,"Pixel Hit Map - "+RichNum+", All Hits",  //ID, Title
                  minX-0.5, maxX+0.5,   //minX, maxX
                  minY-0.5, maxY+0.5,   //minY, maxY
                  binsX+1, binsY+1 );   //binsX, binsY

          if(clusterSize >= ionfeedbackCut) // If IFB cluster is observed
          {
            plot2D( order.globalPixelX(), order.globalPixelY(),
                    histidIFB, "Pixel Hit Map - "+RichNum+", IFB Cluster Hits",
                    minX-0.5, maxX+0.5,  //minX, maxX
                    minY-0.5, maxY+0.5,  //minY, maxY
                    binsX+1, binsY+1 );  //binsX, binsY
          }
        }
      } // ...if we m_wantHitmaps
    }
  }

  delete clusters;

  return;
}


//----------------------------------------
//Full RICH-array Hitmap Algorithms
//---------------------------------------
void HPDIonFeedbackMoni::ExecuteQuickHitmap(const Rich::DetectorType RichNum, 
                                            const LHCb::RichSmartID::Vector &smartIDHits)
{
  // This method plots pixel hits on RICHes without clustering

  const int rich = (int)RichNum + 1;
  std::stringstream ss;
  ss << "R" << rich << "AllHit_" << dataset;
  const std::string& histid = ss.str();

  const Rich::SmartIDGlobalOrdering limits(smartIDHits.front());

  unsigned int minX, maxX, minY, maxY, binsX, binsY;   // Hitmap limits
  minX = limits.minGlobalPixelX();
  maxX = limits.maxGlobalPixelX();
  minY = limits.minGlobalPixelY();
  maxY = limits.maxGlobalPixelY();
  binsX = limits.totalNumInGlobalX();
  binsY = limits.totalNumInGlobalY();

  for( LHCb::RichSmartID::Vector::const_iterator iP =
         smartIDHits.begin(); iP != smartIDHits.end(); ++iP){
    const Rich::SmartIDGlobalOrdering order(*iP); // ordering object - Chris Jones' plotCoordinate tool

    plot2D( order.globalPixelX(), order.globalPixelY(),  //PlotX, PlotY
            histid, histid,   //ID, Title
            minX-0.5, maxX+0.5,   //minX, maxX
            minY-0.5, maxY+0.5,   //minY, maxY
            binsX+1, binsY+1 );   //binsX, binsY
  }

}


//=============================================================================
// Finalization
//=============================================================================
StatusCode HPDIonFeedbackMoni::finalize()
{

  if(msgLevel(MSG::VERBOSE))
  {
    verbose() << "Maximum dataset number " << dataset << endmsg;
    verbose() << "Number of monitored events " << m_nMonitoredEvents << endmsg;
  }

  // Must call base class finalize last
  return HistoAlgBase::finalize();
}

