// $Id: HPDIonFeedbackMoni.cpp,v 1.4 2009-04-24 16:18:55 jonrob Exp $
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

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HPDIonFeedbackMoni::execute()
{

  const Rich::DAQ::L1Map & allSmartIDs = m_SmartIDDecoder->allRichSmartIDs();

  // Check number of hits
  if ( allSmartIDs.empty() ) { return StatusCode::SUCCESS; }

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
          if(PanNum == 0) {
            int nClustTotRich2PanZ = 0;
            int nClustIFBRich2PanZ = 0;
            for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
                   clusVec.begin(); iClust != clusVec.end(); ++iClust) {
              int clusterSize = (*(*iClust)).size();
              if (clusterSize>0) ++nClustTotRich2PanZ;
              else if (clusterSize >= ionFeedbackCut) ++nClustIFBRich2PanZ;
            }
            plot2D((HPDCol+9),(15-HPDRow),"Rich2TotClus","Rich2 #Tot clusters", -0.5, 17.5, -0.5, 15.5, 18, 16, nClustTotRich2PanZ);
            plot2D((HPDCol+9),(15-HPDRow),"Rich2IFBClus","Rich2 #IFB clusters", -0.5, 17.5, -0.5, 15.5, 18, 16, nClustIFBRich2PanZ);
          }else{
            int nClustTotRich2PanNZ = 0;
            int nClustIFBRich2PanNZ = 0;
            for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
                   clusVec.begin(); iClust != clusVec.end(); ++iClust) {
              int clusterSize = (*(*iClust)).size();
              if (clusterSize>0) ++nClustTotRich2PanNZ;
              else if (clusterSize >= ionFeedbackCut) ++nClustIFBRich2PanNZ;
            }
            plot2D(HPDCol,(15-HPDRow),"Rich2TotClus","Rich2 #Tot clusters", -0.5, 17.5, -0.5, 15.5, 18, 16, nClustTotRich2PanNZ);
            plot2D(HPDCol,(15-HPDRow),"Rich2IFBClus","Rich2 #IFB clusters", -0.5, 17.5, -0.5, 15.5, 18, 16, nClustIFBRich2PanNZ);
          }
        } else {
          if(PanNum == 0) {
            int nClustTotRich1PanZ = 0;
            int nClustIFBRich1PanZ = 0;
            for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
                   clusVec.begin(); iClust != clusVec.end(); ++iClust) {
              int clusterSize = (*(*iClust)).size();
              if (clusterSize>0) ++nClustTotRich1PanZ;
              else if (clusterSize >= ionFeedbackCut) ++nClustIFBRich1PanZ;
            }
            plot2D((13-HPDRow),(13-HPDCol),"Rich1TotClus","Rich1 #Tot clusters", -0.5, 13.5, -0.5, 13.5, 14, 14, nClustTotRich1PanZ);
            plot2D((13-HPDRow),(13-HPDCol),"Rich1IFBClus","Rich1 #IFB clusters", -0.5, 13.5, -0.5, 13.5, 14, 14, nClustIFBRich1PanZ);
          }else{
            int nClustTotRich1PanNZ = 0;
            int nClustIFBRich1PanNZ = 0;
            for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
                   clusVec.begin(); iClust != clusVec.end(); ++iClust) {
              int clusterSize = (*(*iClust)).size();
              if (clusterSize>0) ++nClustTotRich1PanNZ;
              else if (clusterSize >= ionFeedbackCut) ++nClustIFBRich1PanNZ;
            }
            plot2D(HPDRow,HPDCol,"Rich1TotClus","Rich1 #Tot clusters", -0.5, 13.5, -0.5, 13.5, 14, 14, nClustTotRich1PanNZ);
            plot2D(HPDRow,HPDCol,"Rich1IFBClus","Rich1 #IFB clusters", -0.5, 13.5, -0.5, 13.5, 14, 14, nClustIFBRich1PanNZ);
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

//================================================================================

