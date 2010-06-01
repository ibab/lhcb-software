// $Id: STClusterMonitor.cpp,v 1.31 2010/04/13 20:10:05 mtobin Exp $
// Include files 

#include <string>
#include <limits>

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
#include "AIDA/IProfile1D.h"
#include "TH1D.h"
#include "GaudiUtils/Aida2ROOT.h"

//#include <boost/accumulators/statistics/mean.hpp>

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

  /// Plots per service box
  declareProperty( "ByServiceBox", m_plotBySvcBox=false );

  /// Plots per detector region
  declareProperty( "ByDetectorRegion", m_plotByDetRegion=false );

  /// Plot by port 
  declareProperty( "ByPort", m_plotByPort=false );

  /// Plot hitmap for each layer
  declareProperty( "HitMaps", m_hitMaps=false );

  /// Some data quality cuts
  declareProperty("ChargeCut", m_chargeCut=0);//< charge on the cluster
  declareProperty("MinTotalClusters", m_minNClusters=0);/// cuts on the minimum number of clusters in the event
  declareProperty("MaxTotalClusters", m_maxNClusters=std::numeric_limits<unsigned int>::max());/// cut on maximum number clusters 
  declareProperty("MinMPVCharge",m_minMPVCharge=0.);//< Cut on the charge of the cluster when calculating MPV

  /// Overflow limit for number of clusters in event
  declareProperty("OverflowLimit", m_overFlowLimit=5000);
  /// Reset rate for histograms/accumulators
  declareProperty("ResetRate", m_resetRate=std::numeric_limits<unsigned int>::max());

  setForcedInit();  
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


  // Get the tell1 mapping from source ID to tell1 number
  m_nTELL1s = readoutTool()->nBoard();

  // Store number of clusters/TELL1 (48 Tell1s, 1->48)
  m_nClustersPerTELL1.resize(m_nTELL1s,0);

  m_nBeetlePortsPerSector = 1;
  if(detType() == "TT") m_nBeetlePortsPerSector=m_nBeetlePortsPerTTSector;
  else if(detType() == "IT") m_nBeetlePortsPerSector=m_nBeetlePortsPerITSector;

  // Book histograms
  bookHistograms();

  // Reset the accumulators
  m_runNumber = std::numeric_limits<unsigned int>::max();
  resetAccumulators();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STClusterMonitor::execute() {

  if(m_debug) debug() << "==> Execute" << endmsg;

  unsigned int runNumber;
  if( exist<LHCb::ODIN> ( LHCb::ODINLocation::Default ) ) {
    const LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    plot1D(odin->bunchId(),"BCID","BCID",-0.5,2808.5,2808);
    runNumber = odin->runNumber();
  } else return Warning("No ODIN bank found", StatusCode::SUCCESS,1);

  counter("Number of events") += 1; 

  if(runNumber != m_runNumber) resetAccumulators();
  // code goes here  
  monitorClusters();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ST::STClusterMonitor::finalize() {

  if(m_debug) debug() << "==> Finalize" << endmsg;
  if(m_hitMaps) fillMPVMap();
  
  return ST::HistoAlgBase::finalize(); 
}

//=============================================================================
// Look at the clusters histogram
//=============================================================================
void ST::STClusterMonitor::monitorClusters() {

  // Check location exists
  if(m_debug) debug() << "monitorClusters" << endmsg;
  if(exist<LHCb::STClusters>(m_clusterLocation)){
    m_nClustersPerTELL1.resize(m_nTELL1s,0);
    LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);

    const unsigned int nClusters = clusters->size();
    if(m_debug) debug() << "Number of clusters in " << m_clusterLocation << " is " << nClusters << endmsg;

    if(nClusters < m_minNClusters || nClusters > m_maxNClusters) return;

    m_1d_nClusters->fill(nClusters);
    if(100  < nClusters) {
      m_1d_nClusters_gt_100->fill(nClusters);
    }
    if(m_overFlowLimit < nClusters) m_1d_nClusters_overflow->fill(m_overFlowLimit);

    // Loop over clusters
    LHCb::STClusters::const_iterator itClus = clusters->begin();
    for(; itClus != clusters->end(); ++itClus) {
      const LHCb::STCluster* cluster = (*itClus);
      fillHistograms(cluster);
      if(m_hitMaps) fillHitMaps(cluster);
      if(fullDetail()) fillDetailedHistograms(cluster);
    }// End of cluster iterator

    // Fill histogram for number of clusters/TELL1
    unsigned int TELL1=1;
    std::vector<unsigned int>::const_iterator itClPerTELL1 = m_nClustersPerTELL1.begin();
    for(; itClPerTELL1 != m_nClustersPerTELL1.end(); ++itClPerTELL1, ++TELL1) {
      if(m_verbose) verbose() << "TELL1: " << TELL1 << ",clusters: " 
                              << (*itClPerTELL1) << endmsg;
      unsigned int nClusters = (*itClPerTELL1);
      if(0 < nClusters) {
        m_2d_nClustersVsTELL1->fill(TELL1, nClusters);
        m_prof_nClustersVsTELL1->fill(TELL1, nClusters);
      }
    }
    m_nClustersPerTELL1.clear();

    // this is where any histograms/counters are reset
    if(m_resetRate > 0 && counter("Number of events").nEntries()%m_resetRate == 0) {
      if(m_hitMaps) {
        fillMPVMap();
        resetMPVCounters();
      }
    }

  } else Warning("No clusters found at "+m_clusterLocation, StatusCode::SUCCESS,0).ignore(); // End of cluster exists

}
//==============================================================================
// Reset MPV counters
//==============================================================================
void ST::STClusterMonitor::resetMPVCounters() {
  std::map<const unsigned int,TH1D*>::iterator itH = m_1ds_chargeBySector.begin();
  for(; itH != m_1ds_chargeBySector.end(); ++itH) {
    //(*itH).second->Reset();
    TH1D* h = (*itH).second;
    double* data = h->GetArray();
    memset(data,0,static_cast<size_t>(h->GetNbinsX()+2)*sizeof(double));
    h->SetEntries(0);
  }
  resetAccumulators();

}
//==============================================================================
// Reset accumulators after a change of run number
//==============================================================================
void ST::STClusterMonitor::resetAccumulators() {
  if(m_debug) debug() << "Resetting accumulators" << endreq;
  m_sectorMPVs.clear();
  m_sectorMeans.clear();
  std::vector<DeSTSector*>::const_iterator Sectors = tracker()->sectors().begin();
  for(; Sectors != tracker()->sectors().end(); ++Sectors) {
    m_sectorMPVs[(*Sectors)->elementID().uniqueSector()] = ST::MedianAccumulator();
    m_sectorMeans[(*Sectors)->elementID().uniqueSector()] = ST::MeanAccumulator();
  }
}
//================================================================================================================================
// Fill the MPV cluster maps 
//================================================================================================================================
void ST::STClusterMonitor::fillMPVMap() {

  std::vector<DeSTSector*>::const_iterator Sectors = tracker()->sectors().begin();
  for(; Sectors != tracker()->sectors().end(); ++Sectors) {
    if(m_1ds_chargeBySector.find((*Sectors)->elementID().uniqueSector()) != m_1ds_chargeBySector.end()) {
      TH1D* hSec = m_1ds_chargeBySector[(*Sectors)->elementID().uniqueSector()];
      double mpv = hSec->GetBinCenter(hSec->GetMaximumBin());
      //      double nEntries = hSec->GetEntries();
      unsigned int bin = m_sectorBins1D[(*Sectors)->elementID().uniqueSector()];
      m_prof_sectorMPVs->fill(bin, mpv);
      if(detType() == "TT") {
        const DeTTSector* ttSector = dynamic_cast<const DeTTSector* >((*Sectors));
        ST::TTDetectorPlot hitMap("map","map");
        ST::TTDetectorPlot::Bins bins = hitMap.toBins(ttSector);
        double xBin = bins.xBin;
        // Hack to make real x-y distribution plot (not occupancy)
        for( int yBin = bins.beginBinY; yBin != bins.endBinY; ++yBin ) {
          m_2d_sectorMPVs->fill(xBin,yBin,mpv);
          m_2d_sectorMPVsNorm->fill(xBin,yBin,1.);
        }
      } else if( detType() == "IT" ) {// Cluster map for IT
        ST::ITDetectorPlot hitMap("map", "map");
        ST::ITDetectorPlot::Bins bins = hitMap.toBins((*Sectors)->elementID());
        // Hack to make real x-y distribution plot (not occupancy)
        m_2d_sectorMPVs->fill(bins.xBin, bins.yBin, mpv);
        m_2d_sectorMPVsNorm->fill(bins.xBin, bins.yBin, 1.);
      }
    } else Warning("Invalid Sector",0,StatusCode::SUCCESS).ignore();
  } // End of sector iterator loop

}

//==============================================================================
// Book histograms
//==============================================================================
void ST::STClusterMonitor::bookHistograms() {
  // filled in monitor clusters
  m_1d_nClusters = book1D("Number of clusters",0.,20000.,2000);
  m_1d_nClusters_gt_100 = book1D("Number of clusters (N > 100)", 0., 20000., 2000);
  m_1d_nClusters_overflow = book1D("Number of clusters (no overflow)", 0., m_overFlowLimit+1, 1000);
  m_2d_nClustersVsTELL1 = book2D("Number of clusters per TELL1", 0.5, 
                                 m_nTELL1s+0.5, m_nTELL1s, 0.,100., 50);

  // Number of clusters produced by each TELL1
  m_1d_nClustersVsTELL1 = book1D("Number of clusters vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
  m_prof_nClustersVsTELL1 = bookProfile1D("Mean number of clusters vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);

  // filled in fillHistograms
  m_1d_ClusterSize = book1D("Cluster Size", 0.5, 4.5, 4); 
  m_2d_ClusterSizeVsTELL1 = book2D("Cluster Size vs TELL1", 0.5, 
                                   m_nTELL1s+0.5, m_nTELL1s, 0.5, 4.5, 4);
  m_2d_STNVsTELL1 = book2D("Signal to Noise vs TELL1", 0.5, 
                             m_nTELL1s+0.5, m_nTELL1s, 0., 100., 25);
  
  m_2d_ChargeVsTELL1 = book2D("Cluster Charge vs TELL1", 0.5, 
                              m_nTELL1s+0.5, m_nTELL1s, 0., 60., 60);
  if(m_plotByPort) {
    m_2d_ClustersPerPortVsTELL1 = book2D("Clusters per port vs TELL1", 0.5,
                                     m_nTELL1s+0.5, m_nTELL1s, -0.5, 95.5, 96);
  }
  m_1d_totalCharge = book1D("Cluster ADC Values", 0., 200., 200);
  if(detType() == "TT" || m_plotBySvcBox) {
    /// list of service boxes  
    std::vector<std::string>::const_iterator itSvcBoxes = readoutTool()->serviceBoxes().begin();
    for( ; itSvcBoxes != readoutTool()->serviceBoxes().end(); ++itSvcBoxes) {
      std::string svcBox=(*itSvcBoxes);
      if(detType() == "TT") {
        std::string quadrant = svcBox.substr(0,2);
        if(m_1ds_chargeByServiceBox[quadrant] == 0)
          m_1ds_chargeByServiceBox[quadrant] = book1D("Cluster ADC Values "+quadrant, 0., 200., 200);
      }
      if(m_plotBySvcBox)
        m_1ds_chargeByServiceBox[svcBox] = book1D("Cluster ADC Values "+svcBox, 0., 200., 200);
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
      m_1ds_chargeByDetRegion[region] = book1D("Cluster ADC Values "+region, 0., 200., 200);
    };
  }
  // Book histograms for hitmaps
  if(m_hitMaps) {
    std::string idMap = detType()+" cluster map";
    std::string idMPVMap = detType()+" Sector MPVs";
    if(detType()=="TT"){// Cluster maps for TT
      ST::TTDetectorPlot hitMap(idMap, idMap, m_nBeetlePortsPerTTSector);
      m_2d_hitmap = book2D(hitMap.name(), hitMap.minBinX(), hitMap.maxBinX(), hitMap.nBinX(),
                           hitMap.minBinY(), hitMap.maxBinY(), hitMap.nBinY());
      ST::TTDetectorPlot MPVMap(idMPVMap, idMPVMap);
      m_2d_sectorMPVs = book2D(MPVMap.name(), MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
                               MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
      m_2d_sectorMPVsNorm = book2D(MPVMap.name()+" Normalisation", MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
				   MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
    } else if( detType() == "IT" ) {// Cluster map for IT
      ST::ITDetectorPlot hitMap(idMap, idMap, m_nBeetlePortsPerITSector);
      m_2d_hitmap = book2D(hitMap.name(), hitMap.minBinX(), hitMap.maxBinX(), hitMap.nBinX(),
                           hitMap.minBinY(), hitMap.maxBinY(), hitMap.nBinY());
      ST::ITDetectorPlot MPVMap(idMPVMap, idMPVMap);
      m_2d_sectorMPVs = book2D(MPVMap.name(), MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
                               MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
      m_2d_sectorMPVsNorm = book2D(MPVMap.name()+" Normalisation", MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
				   MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
    }
    // Create histogram of with total charge for each sector
    std::vector<DeSTSector*>::const_iterator Sectors = tracker()->sectors().begin();
    m_sectorBins1D.clear();
    int bin=1;
    for(; Sectors != tracker()->sectors().end(); ++Sectors, ++bin) {
      std::string idh = "charge_$sector"+boost::lexical_cast<std::string>((*Sectors)->elementID().uniqueSector());
      std::string title = "Total charge in "+(*Sectors)->nickname();
      m_1ds_chargeBySector[(*Sectors)->elementID().uniqueSector()] 
        = Gaudi::Utils::Aida2ROOT::aida2root ( book1D("BySector/"+idh,title,0.,100.,100) );
      //      std::cout << title+":" << (*Sectors)->elementID().uniqueSector() << "," << bin << std::endl;
      m_sectorBins1D[(*Sectors)->elementID().uniqueSector()] = bin;
    }
    // 1d representation for online monitoring
    m_nSectors = m_sectorBins1D.size();
    m_1d_nClustersVsSector = book1D("Number of clusters per sector", 0.5, m_nSectors+0.5, m_nSectors);
    m_1d_nClustersVsBeetlePort = book1D("Number of clusters per beetle port", 
                                        0.5, m_nSectors+0.5, m_nSectors*m_nBeetlePortsPerSector);
    m_prof_sectorMPVs = bookProfile1D("Cluster MPV vs Sector", 0.5, m_nSectors+0.5, m_nSectors);
  }// end of hitmaps

}

//==============================================================================
// Fill histograms
//==============================================================================
void ST::STClusterMonitor::fillHistograms(const LHCb::STCluster* cluster){

  const double totalCharge = cluster->totalCharge();
  if(totalCharge < m_chargeCut) return;
  
  // calculate MPVs
  unsigned int sectorID=cluster->channelID().uniqueSector();
  if(totalCharge > m_minMPVCharge) { 
    m_sectorMPVs[sectorID](totalCharge); 
    m_sectorMeans[sectorID](totalCharge);
    if(m_hitMaps) {
      m_1ds_chargeBySector[sectorID]->Fill(totalCharge);
    }
  }
  const unsigned int clusterSize = cluster->size();

  const unsigned int sourceID = cluster->sourceID();
  unsigned int TELL1ID = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
  m_nClustersPerTELL1[TELL1ID-1] += 1;
  m_1d_nClustersVsTELL1->fill(TELL1ID);
  m_1d_ClusterSize->fill(clusterSize);
  m_2d_ClusterSizeVsTELL1->fill(TELL1ID, clusterSize);
  const DeSTSector* sector = findSector(cluster->channelID()); 
  double noise = sector->noise(cluster->channelID());
  if (noise > 1e-3) {
    const double signalToNoise = cluster->totalCharge()/noise;
    m_2d_STNVsTELL1->fill(TELL1ID, signalToNoise);
  } else Warning("Zero S/N for some clusters",StatusCode::SUCCESS,1).ignore();
  m_2d_ChargeVsTELL1->fill(TELL1ID, totalCharge);
  if(m_plotByPort) {
    const unsigned int tell1Channel = cluster->tell1Channel();
    unsigned int port = tell1Channel/32;
    m_2d_ClustersPerPortVsTELL1->fill(TELL1ID, port);
  }
  if(m_hitMaps) {  // 1d representation of hitmaps
    unsigned int bin = m_sectorBins1D[sectorID];
    m_1d_nClustersVsSector->fill(bin);
    unsigned int port = (cluster->channelID().strip()-1)/32;
    m_1d_nClustersVsBeetlePort->fill((bin-0.5)+static_cast<double>(port)/m_nBeetlePortsPerSector);
  }
  // Always fill histograms per readout quadrant for TT
  // Get service box and set up histogram IDs
  m_1d_totalCharge->fill(totalCharge);
  std::string svcBox = readoutTool()->serviceBox(cluster->channelID());
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
  const DeSTSector* sector = findSector(cluster->channelID());
  double noise = sector->noise(cluster->channelID()); 
  if (noise > 1e-3){
    const double signalToNoise = cluster->totalCharge()/noise;
    plot1D(signalToNoise,cluster->layerName()+"/Signal to noise","S/N",0., 100.,100);
  } else Warning("Some cluster have zero noise", StatusCode::SUCCESS,1).ignore();
  // Plot cluster ADCs for 1, 2, 3, 4 strip clusters
  std::string svcBox = (this->readoutTool())->serviceBox(cluster->channelID());
  std::string idhStrip = " (" + boost::lexical_cast<std::string>(clusterSize) + " strip)";
  std::string idh;
  if(detType() == "TT") {
    idh = "Cluster ADCs in " + svcBox.substr(0,2)+idhStrip;
    plot1D(totalCharge,idh, idh,0.,200.,200);
  }
  idh = "Cluster ADCs in " + svcBox+idhStrip;
  plot1D(totalCharge,idh, idh,0.,200.,200);

  // Charge in each sector for 1, 2, 3 and 4 strip clusters
  idh = "BySector/s"+boost::lexical_cast<std::string>(cluster->size())
    +"/charge_$sector"+boost::lexical_cast<std::string>(cluster->channelID().uniqueSector());
  plot1D(totalCharge, idh, "Total charge in "+cluster->sectorName(),0.,200.,200);
}
//==============================================================================
/// Fill histograms of hitmaps
//==============================================================================
void ST::STClusterMonitor::fillHitMaps(const LHCb::STCluster* cluster) {  
  std::string idMap = detType()+" cluster map";
  if(detType()=="TT"){// Cluster maps for TT
    const DeSTSector* aSector = tracker()->findSector(cluster->channelID());
    const DeTTSector* ttSector = dynamic_cast<const DeTTSector* >(aSector);
    ST::TTDetectorPlot hitMap(idMap, idMap, m_nBeetlePortsPerTTSector);
    ST::TTDetectorPlot::Bins bins = hitMap.toBins(ttSector);
    int port = (cluster->channelID().strip()-1)/STDAQ::nstrips;
    double xBin = bins.xBin - double(port+1)/m_nBeetlePortsPerTTSector + 0.5;
    // Hack to make real x-y distribution plot (not occupancy)
    int nBins = bins.endBinY - bins.beginBinY;
    for( int yBin = bins.beginBinY; yBin != bins.endBinY; ++yBin ) {
      m_2d_hitmap->fill(xBin,yBin,1./nBins);
    }
  } else if( detType() == "IT" ) {// Cluster map for IT
    ST::ITDetectorPlot hitMap(idMap, idMap, m_nBeetlePortsPerITSector);
    ST::ITDetectorPlot::Bins bins = hitMap.toBins(cluster->channelID());
    // Hack to make real x-y distribution plot (not occupancy)
    m_2d_hitmap->fill(bins.xBin, bins.yBin, 1.);
  }
}
