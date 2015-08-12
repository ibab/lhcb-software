// $Id: HPDIonFeedbackMoni.cpp,v 1.9 2010-02-13 12:38:07 jonrob Exp $
// Include files

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
DECLARE_ALGORITHM_FACTORY( HPDIonFeedbackMoni )

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
  declareProperty("WantHitmaps", m_wantHitmaps = false       );
  declareProperty("WantQuickHitmap",      m_wantQuickHitmap = false  );
  declareProperty("IsDark", m_isdark = false );
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
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, NULL, true );
  acquireTool( "RichSmartIDTool", m_idTool,            NULL, true );
  acquireTool( "RichHPDPixelClusterFinder", m_clusterTool, this );

  m_nEvts = 0;
  m_nMonitoredEvents = 0;
  r1clustersThisEvent = 0;
  r2clustersThisEvent = 0;

  return sc;
}

//=============================================================================
// Pre-book histograms
//=============================================================================
StatusCode HPDIonFeedbackMoni::prebookHistograms()
{
  // Book event-counter + cluster occupancy histograms
  richHisto1D( HID("Events"), "Events", 0,10,10 );
  richHisto1D( HID("R1ClusOcc"), "R1ClusOcc", 0, 2000, 2000);
  richHisto1D( HID("R2ClusOcc"), "R2ClusOcc", 0, 2000, 2000);

  // Loop over all RICH1+RICH2 positions and book corresponding cluster-size histograms...
  for(int rich=1; rich<3; rich++){
    for(int pan=0; pan<2; pan++){
      int colmax, rowmax;
      if(rich==1) {
        colmax=7;
        rowmax=14;
      }
      else {
        colmax=9;
        rowmax=16;
      }

      for(int col=0;col<colmax;col++){
        for(int row=0;row<rowmax;row++){

          int hid = (rowmax*col)+row+1+
            (pan*(rowmax*colmax))+
            ((rich-1)*(196));

          std::stringstream ss;
          ss << "R" << rich << "_Pan" << pan << "_Col" << col << "_Row" << row;
          const std::string& hpdstr = ss.str();
          hidtostring.insert( make_pair( hid,hpdstr ) );
          // Cluster-size histograms
          richHisto1D( HID(hpdstr), hpdstr, 0, 50, 50);
        }
      }
    }
  }  // ...end RICH loop

  return StatusCode::SUCCESS;

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

  // Only execute for dark count measurement
  if ( m_isdark ) 
  {
    const LHCb::ODIN * odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
    if ( odin->triggerType()     != LHCb::ODIN::CalibrationTrigger || // Calibration trigger
         odin->calibrationType() != LHCb::ODIN::A )    // Corner-pixel test pattern (A=0)
      return StatusCode::SUCCESS;
  }

  const Rich::DAQ::L1Map & allSmartIDs = m_SmartIDDecoder->allRichSmartIDs();

  // Check number of hits
  if ( allSmartIDs.empty() ) { return StatusCode::SUCCESS; }

  m_nMonitoredEvents++;

  // -- increment "Event" histogram by 1

  richHisto1D( HID("Events") ) -> fill(1);

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

        const unsigned int HPDCol = smartIDHPD.pdCol();
        const unsigned int HPDRow = smartIDHPD.pdNumInCol();
        const bool aliceMode      = (*iAllSmartIDs).second.header().aliceMode();



        if(m_isdark) {
          // Suppress corner pixels...this should be updated
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
    richHisto1D( HID("R1ClusOcc") ) -> fill((double)r1clustersThisEvent);
    r1clustersThisEvent = 0;
  }

  if(r2clustersThisEvent>0) {      // If clusters were found in RICH2
    richHisto1D( HID("R2ClusOcc") ) -> fill((double)r2clustersThisEvent);
    r2clustersThisEvent = 0;
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

  int colmax, rowmax;
  if(rich==1) {
    colmax=7;
    rowmax=14;
  }
  else {
    colmax=9;
    rowmax=16;
  }
  int hid = (rowmax*HPDCol)+HPDRow+1+
    ((int)PanNum*(rowmax*colmax))+
    ((rich-1)*(196));     // Convert position to unique histogram ID for map

  for( Rich::HPDPixelClusters::Cluster::PtnVector::const_iterator iClust =
         clusVec.begin(); iClust != clusVec.end(); ++iClust)      // loop over found clusters
  {
    unsigned int clusterSize = (*(*iClust)).size();

    if(clusterSize>0){     // If a cluster of non-zero pixel size is observed

      richHisto1D( hidtostring[hid] ) -> fill( clusterSize-0.5 );

      if(rich==1) r1clustersThisEvent++;      // Increment #RICH1 clusters
      if(rich==2) r2clustersThisEvent++;      // Increment #RICH1 clusters


      // ---Clus--- Note down the cluster for histograms

      if(m_wantHitmaps){   // If we want to plot hitmaps (CPU-intensive!)

        Rich::HPDPixelCluster pixVec = (*(*iClust)).pixels();

        const Rich::SmartIDGlobalOrdering limits(smartIDHits.front());

        minX = limits.minGlobalPixelX();
        maxX = limits.maxGlobalPixelX();
        minY = limits.minGlobalPixelY();
        maxY = limits.maxGlobalPixelY();
        binsX = limits.totalNumInGlobalX();
        binsY = limits.totalNumInGlobalY();

        std::stringstream ssAll, ssIFB;
        ssAll << "R" << rich << "AllClusHit";
        ssIFB << "R" << rich << "IFBClusHit";

        for( LHCb::RichSmartID::Vector::const_iterator iP =
               pixVec.smartIDs().begin(); iP != pixVec.smartIDs().end(); ++iP)
        {
          const Rich::SmartIDGlobalOrdering order(*iP); // ordering object - Chris Jones' plotCoordinate tool

          const std::string RichNumT = Rich::text( (*iP).rich() );

          plot2D( order.globalPixelX(), order.globalPixelY(),                 //PlotX, PlotY
                  ssAll.str(),"Pixel Hit Map - "+RichNumT+", All Hits",  //ID, Title
                  minX-0.5, maxX+0.5,   //minX, maxX
                  minY-0.5, maxY+0.5,   //minY, maxY
                  binsX+1, binsY+1 );   //binsX, binsY


          if(clusterSize >= ionfeedbackCut) // If IFB cluster is observed
          {
            plot2D( order.globalPixelX(), order.globalPixelY(),
                    ssIFB.str(), "Pixel Hit Map - "+RichNumT+", IFB Cluster Hits",
                    minX-0.5, maxX+0.5,  //minX, maxX
                    minY-0.5, maxY+0.5,  //minY, maxY
                    binsX+1, binsY+1 );  //binsX, binsY
          }

        } // ...if we m_wantHitmaps
      }

    }
  }


  delete clusters;

  return;
}




//----------------------------------------
//Full RICH-array Hitmap Algorithms
//---------------------------------------
void HPDIonFeedbackMoni::ExecuteQuickHitmap( const Rich::DetectorType RichNum, 
                                             const LHCb::RichSmartID::Vector &smartIDHits )
{
  // This method plots pixel hits on RICHes without clustering
  const int rich = (int)RichNum + 1;
  std::stringstream ss;
  ss << "R" << rich << "AllHit";
  const Rich::SmartIDGlobalOrdering limits(smartIDHits.front());

  unsigned int minX, maxX, minY, maxY, binsX, binsY;   // Hitmap limits
  minX = limits.minGlobalPixelX();
  maxX = limits.maxGlobalPixelX();
  minY = limits.minGlobalPixelY();
  maxY = limits.maxGlobalPixelY();
  binsX = limits.totalNumInGlobalX();
  binsY = limits.totalNumInGlobalY();

  for ( LHCb::RichSmartID::Vector::const_iterator iP =
         smartIDHits.begin(); iP != smartIDHits.end(); ++iP ) 
  {
    const Rich::SmartIDGlobalOrdering order(*iP); // ordering object - Chris Jones' plotCoordinate tool

    plot2D( order.globalPixelX(), order.globalPixelY(),  //PlotX, PlotY
            ss.str(), ss.str(),   //ID, Title
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
    verbose() << "Number of monitored events " << m_nMonitoredEvents << endmsg;
  }

  return HistoAlgBase::finalize();
}

