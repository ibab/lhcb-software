// $Id: STClusterMonitor.cpp,v 1.16 2009-12-06 02:02:13 mtobin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "STClusterMonitor.h"
#include "Kernel/STDetSwitch.h"
#include "Event/STCluster.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/STChannelID.h"
#include "Kernel/TTNames.h"
#include "Kernel/ITNames.h"
#include "Kernel/ISTSignalToNoiseTool.h"

#include "Event/ODIN.h"
#include "Kernel/TTDetectorPlot.h"
#include "Kernel/ITDetectorPlot.h"
#include "Kernel/STDAQDefinitions.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeTTSector.h"

// from Boost
#include <boost/assign/list_of.hpp>
#include "boost/lexical_cast.hpp"

// AIDA histograms
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STClusterMonitor
//
// 2009-03-10 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STClusterMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STClusterMonitor::STClusterMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : ST::HistoAlgBase ( name , pSvcLocator )
{
  // Input location for clusters...
  declareSTConfigProperty("ClusterLocation", m_clusterLocation, 
                          LHCb::STClusterLocation::TTClusters);

  /// Use signal to noise tool
  declareProperty("SignalToNoise", m_stn=false );
  // Signal to noise
  declareProperty("SignalToNoiseTool",
                  m_sigNoiseToolName="STSignalToNoiseTool");

  /// Plots per service box
  declareProperty( "ByServiceBox", m_plotBySvcBox=false );

  /// Plots per detector region
  declareProperty( "ByDetectorRegion", m_plotByDetRegion=false );

  /// Plot by port 
  declareProperty( "ByPort", m_plotByPort=false );

  /// Plot hitmap for each layer
  declareProperty( "HitMaps", m_hitMaps=false );

  /// Set maximum number of clusters per TELL1
  declareProperty( "MaxClustersPerTELL1", m_maxNClusters=4000 );

  declareProperty("BunchID",       m_bunchID               );// BunchID 

}
//=============================================================================
// Destructor
//=============================================================================
ST::STClusterMonitor::~STClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ST::STClusterMonitor::initialize() {

  StatusCode sc = ST::HistoAlgBase::initialize(); 
  if ( sc.isFailure() ) return sc;

  m_debug = msgLevel( MSG::DEBUG );
  m_verbose = msgLevel( MSG::VERBOSE );
  if(m_verbose) m_debug = true;

  if (m_debug) debug() << "==> Initialize" << endmsg;
  if( "" == histoTopDir() ) setHistoTopDir(this->detType()+"/");

  // Turn all histograms on in expert mode
  if( fullDetail() ) {
    m_hitMaps = true;
    m_plotBySvcBox = true;
    m_plotByDetRegion = true;
    m_plotByPort = true;
  }

  // sig to noise tool
  if(m_stn) m_sigNoiseTool = tool<ISTSignalToNoiseTool>( m_sigNoiseToolName,
                                               m_sigNoiseToolName + detType());

  // Get the tell1 mapping from source ID to tell1 number
  m_nTELL1s = readoutTool()->nBoard();

  // Store number of clusters/TELL1 (48 Tell1s, 1->48)
  m_nClustersPerTELL1.resize(m_nTELL1s,0);

  // Book histograms
  bookHistograms();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STClusterMonitor::execute() {

  if(m_debug) debug() << "==> Execute" << endmsg;

  const LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  if( !m_bunchID.empty() && 
      std::find(m_bunchID.begin(), m_bunchID.end(), 
                odin->bunchId()) == m_bunchID.end()) return StatusCode::SUCCESS;
  plot1D(odin->bunchId(),"BCID","BCID",-0.5,6000.5,6001);

  counter("Number of events") += 1; 

  //  std::cout << "Counter: " << counter("Number of events").nEntries() << std::endl;
  
  // code goes here  
  monitorClusters();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ST::STClusterMonitor::finalize() {

  if(m_debug) debug() << "==> Finalize" << endmsg;

  return ST::HistoAlgBase::finalize(); 
}

//=============================================================================
// Look at the clusters histogram
//=============================================================================
void ST::STClusterMonitor::monitorClusters() {

  //  std::cout << counter("Number of events").nEntries() << ": BCID: " << BCID << std::endl;

  // Check location exists
  if(m_debug) debug() << "monitorClusters" << endmsg;
  if(exist<LHCb::STClusters>(m_clusterLocation)){
    m_nClustersPerTELL1.resize(m_nTELL1s,0);
    LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);
    LHCb::STClusters::const_iterator itClus;
    // Loop over clusters
    const unsigned int nClusters = clusters->size();
    if(m_debug) debug() << "Number of clusters in " << m_clusterLocation 
                        << " is " << nClusters << endmsg;

    m_1d_nClusters->fill(nClusters);
    if(100  < nClusters) {
      m_1d_nClusters_gt_100->fill(nClusters);
    }
    for(itClus = clusters->begin(); itClus != clusters->end(); ++itClus) {
      const LHCb::STCluster* cluster = (*itClus);
      fillHistograms(cluster);
      if(m_hitMaps) fillHitMaps(cluster);
      if(fullDetail()) fillDetailedHistograms(cluster);
    }// End of cluster iterator

    // Fill histogram for number of clusters/TELL1
    std::vector<unsigned int>::const_iterator itClPerTELL1;
    unsigned int TELL1=1;
    for(itClPerTELL1 = m_nClustersPerTELL1.begin(); 
        itClPerTELL1 != m_nClustersPerTELL1.end(); ++itClPerTELL1, ++TELL1) {
      if(m_verbose) verbose() << "TELL1: " << TELL1 << ",clusters: " 
                              << (*itClPerTELL1) << endmsg;
      unsigned int nClusters = (*itClPerTELL1);
      if(0 < nClusters) 
        m_2d_nClustersVsTELL1->fill(TELL1, nClusters);
    }
    m_nClustersPerTELL1.clear();
  } else Warning("No clusters found at "+m_clusterLocation, StatusCode::SUCCESS,0).ignore(); // End of cluster exists
}

//==============================================================================
// Book histograms
//==============================================================================
void ST::STClusterMonitor::bookHistograms() {
  // filled in monitor clusters
  m_1d_nClusters = book1D("Number of clusters",-20.,20020.,501);
  m_1d_nClusters_gt_100 = book1D("Number of clusters (N > 100)", 90.,10010., 
                                 496);
  m_2d_nClustersVsTELL1 = book2D("Number of clusters per TELL1", 0.5, 
                                 m_nTELL1s+0.5, m_nTELL1s, -5.,505., 51);

  // filled in fillHistograms
  m_2d_ClusterSizeVsTELL1 = book2D("Cluster Size vs TELL1", 0.5, 
                                   m_nTELL1s+0.5, m_nTELL1s, 0.5, 4.5, 4);
  if(m_stn) {
    m_2d_STNVsTELL1 = book2D("Signal to Noise vs TELL1", 0.5, 
                             m_nTELL1s+0.5, m_nTELL1s, -2.5, 102.5, 21);
  }
  m_2d_ChargeVsTELL1 = book2D("Cluster Charge vs TELL1", 0.5, 
                              m_nTELL1s+0.5, m_nTELL1s, 0., 60., 60);
  if(m_plotByPort) {
    m_2d_ClustersPerPortVsTELL1 = book2D("Clusters per port vs TELL1", 0.5,
                                     m_nTELL1s+0.5, m_nTELL1s, -0.5, 95.5, 96);
  }
  m_1d_totalCharge = book1D("Cluster ADC Values", 0., 200., 100);
  if(detType() == "TT" || m_plotBySvcBox) {
    /// list of service boxes  
    std::vector<std::string>::const_iterator itSvcBoxes = 
      readoutTool()->serviceBoxes().begin();
    for( ; itSvcBoxes != readoutTool()->serviceBoxes().end(); ++itSvcBoxes) {
      std::string svcBox=(*itSvcBoxes);
      if(detType() == "TT") {
        std::string quadrant = svcBox.substr(0,2);
        if(m_1ds_chargeByServiceBox[quadrant] == 0)
          m_1ds_chargeByServiceBox[quadrant] = book1D("Cluster ADC Values "+quadrant, 0., 200., 100);
      }
      if(m_plotBySvcBox)
        m_1ds_chargeByServiceBox[svcBox] = book1D("Cluster ADC Values "+svcBox, 0., 200., 100);
    } // End of service box loop
  } // End of service box condition

  if(m_plotByDetRegion) {
    std::vector<std::string> names;
    if(detType() == "TT"){
      names = LHCb::TTNames().allDetRegions();
    } else if(detType() == "IT"){
      names = LHCb::ITNames().allBoxes();
    }
    std::vector<std::string>::iterator itNames = names.begin();
    for( ; itNames != names.end(); ++itNames ){
      std::string region = (*itNames);
      m_1ds_chargeByDetRegion[region] = book1D("Cluster ADC Values "+region, 0., 200., 100);
    };
  }
  // Book histograms for hitmaps
  if(m_hitMaps) {
    std::string idMap = detType()+" cluster map";
    if(detType()=="TT"){// Cluster maps for TT
      const unsigned int nBinsPerSector = 16u; // equals ports per sector in TT
      ST::TTDetectorPlot hitMap(idMap, idMap, nBinsPerSector);
      m_2d_hitmap = book2D(hitMap.name(), hitMap.minBinX(), hitMap.maxBinX(), hitMap.nBinX(),
                           hitMap.minBinY(), hitMap.maxBinY(), hitMap.nBinY());
    } else if( detType() == "IT" ) {// Cluster map for IT
      ST::ITDetectorPlot hitMap(idMap, idMap);
      m_2d_hitmap = book2D(hitMap.name(), hitMap.minBinX(), hitMap.maxBinX(), hitMap.nBinX(),
                           hitMap.minBinY(), hitMap.maxBinY(), hitMap.nBinY());
    }
  }
}

//==============================================================================
// Fill histograms
//==============================================================================
void ST::STClusterMonitor::fillHistograms(const LHCb::STCluster* cluster){
  const double totalCharge = cluster->totalCharge();
  const unsigned int clusterSize = cluster->size();
  const unsigned int sourceID = cluster->sourceID();
  unsigned int TELL1ID = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
  m_nClustersPerTELL1[TELL1ID-1] += 1;
  m_2d_ClusterSizeVsTELL1->fill(TELL1ID, clusterSize);
  if(m_stn) {
    m_2d_STNVsTELL1->fill(TELL1ID, m_sigNoiseTool->signalToNoise(cluster));
  }
  m_2d_ChargeVsTELL1->fill(TELL1ID, totalCharge);
  if(m_plotByPort) {
    const unsigned int tell1Channel = cluster->tell1Channel();
    unsigned int port = tell1Channel/32;
    m_2d_ClustersPerPortVsTELL1->fill(TELL1ID, port);
  }
  // Always fill histograms per readout quadrant for TT
  // Get service box and set up histogram IDs
  m_1d_totalCharge->fill(totalCharge);
  std::string svcBox = readoutTool()->serviceBox(cluster->firstChannel());
  if(detType() == "TT") {
    std::string quadrant = svcBox.substr(0,2);
    m_1ds_chargeByServiceBox[quadrant]->fill(totalCharge);
  }
  if(m_plotBySvcBox) m_1ds_chargeByServiceBox[svcBox]->fill(totalCharge);
  if(m_plotByDetRegion) m_1ds_chargeByDetRegion[cluster->detRegionName()]->fill(totalCharge);

}
//==============================================================================
/// Fill more detailed histograms
//==============================================================================
void ST::STClusterMonitor::fillDetailedHistograms(const LHCb::STCluster* 
                                                  cluster) {
  // high threshold
  plot(cluster->highThreshold(),"High threshold","High threshold",-0.5,1.5,2);

  // by strip, modulo a few things....
  const unsigned int strip = cluster->strip();
  plot1D(strip%8,"strip modulo 8","strip modulo 8", -0.5, 8.5, 9);
  plot1D(strip%32,"strip modulo 32","strip modulo 32", -0.5, 32.5, 33);
  plot1D(strip%128,"strip modulo 128","strip modulo 128", -0.5, 128.5, 129);

  // histogram by station
  const int station = cluster->station();
  plot1D(station, "Number of clusters per station", 
         "Number of clusters per station", -0.5, 4.5, 5);
 
  // by layer
  const int layer = cluster->layer();
  plot((double)(10*station+layer), "Number of clusters per layer", 
       "Number of clusters per layer", -0.5, 40.5, 41);

  const double neighbourSum = cluster->neighbourSum();
  plot1D(cluster->pseudoSize(), "Pseudo size of cluster", 
         "Pseudo size of cluster",-0.5, 10.5,11); 

  const double totalCharge = cluster->totalCharge();
  plot1D(neighbourSum/totalCharge,"Relative neighbour sum",
         "Relative neighbour sum", -1.02, 1.02, 51);

  const unsigned int clusterSize = cluster->size();
  if(3 > clusterSize) {
    plot1D(neighbourSum,"Neighbour sum (1- and 2-strip clusters)",
           "Neighbour sum (1- and 2-strip clusters)",-16.5, 26.5, 43);
    plot1D(neighbourSum/totalCharge, 
           "Relative neighbour sum (1- and 2-strip clusters)",
           "Relative neighbour sum (1- and 2-strip clusters)", -1.02, 1.02, 51);
  }
  // charge and S/N
  plot1D(totalCharge,cluster->layerName()+"/Charge", "Charge",-0.5, 500.5, 501);
  if(m_stn) plot1D( m_sigNoiseTool->signalToNoise(cluster),
                    cluster->layerName()+"/Signal to noise","S/N",0., 100.,100);

  // Plot cluster ADCs for 1, 2, 3, 4 strip clusters
  std::string svcBox = (this->readoutTool())->serviceBox(cluster->firstChannel());
  std::string idhStrip = " (" + boost::lexical_cast<std::string>(clusterSize) + " strip)";
  std::string idh;
  if(detType() == "TT") {
    idh = "Cluster ADCs in " + svcBox.substr(0,2)+idhStrip;
    plot1D(totalCharge,idh, idh,0.,200.,100);
  }
  idh = "Cluster ADCs in " + svcBox+idhStrip;
  plot1D(totalCharge,idh, idh,0.,200.,100);
  

}
//==============================================================================
/// Fill histograms of hitmaps
//==============================================================================
void ST::STClusterMonitor::fillHitMaps(const LHCb::STCluster* cluster) {  
  std::string idMap = detType()+" cluster map";
  if(detType()=="TT"){// Cluster maps for TT
    const unsigned int nBinsPerSector = 16u; // equals ports per sector in TT
    const DeSTSector* aSector = tracker()->findSector(cluster->channelID());
    const DeTTSector* ttSector = dynamic_cast<const DeTTSector* >(aSector);
    ST::TTDetectorPlot hitMap(idMap, idMap, nBinsPerSector);
    ST::TTDetectorPlot::Bins bins = hitMap.toBins(ttSector);
    int port = (cluster->channelID().strip()-1)/STDAQ::nstrips;
    double xBin = bins.xBin - double(port+1)/nBinsPerSector + 0.5;
    // Hack to make real x-y distribution plot (not occupancy)
    int nBins = bins.endBinY - bins.beginBinY;
    for( int yBin = bins.beginBinY; yBin != bins.endBinY; ++yBin ) {
      m_2d_hitmap->fill(xBin,yBin,1./nBins);
    }
  } else if( detType() == "IT" ) {// Cluster map for IT
    //    const unsigned int nBinsPerSector = 16u; // equals ports per sector in TT
    ST::ITDetectorPlot hitMap(idMap, idMap);
    ST::ITDetectorPlot::Bins bins = hitMap.toBins(cluster->channelID());
     // Hack to make real x-y distribution plot (not occupancy)
    m_2d_hitmap->fill(bins.xBin, bins.yBin, 1.);
  }
}
