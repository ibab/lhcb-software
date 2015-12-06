// Include files 

#include <string>
#include <limits>

// local
#include "STClusterMonitor.h"
#include "Kernel/STDetSwitch.h"
#include "Event/STCluster.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/STChannelID.h"
#include "Kernel/TTNames.h"
#include "Kernel/UTNames.h"
#include "Kernel/ITNames.h"

#include "Event/ODIN.h"
#include "Kernel/TTDetectorPlot.h"
#include "Kernel/UTDetectorPlot.h"
#include "Kernel/ITDetectorPlot.h"
#include "Kernel/STDAQDefinitions.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeTTSector.h"
#include "STDet/DeUTSector.h"

// from Boost
#include <boost/assign/list_of.hpp>

// AIDA histograms
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"
#include "TH1D.h"
#include "GaudiUtils/Aida2ROOT.h"

#include <algorithm>

//#include <boost/accumulators/statistics/mean.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : STClusterMonitor
//
// 2009-03-10 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STClusterMonitor )
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
  /// Reset rate for MPV calculation
  declareProperty("ResetRate", m_resetRate=1000);

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
  else if(detType() == "UT") m_nBeetlePortsPerSector=m_nBeetlePortsPerUTSector;
  else if(detType() == "IT") m_nBeetlePortsPerSector=m_nBeetlePortsPerITSector;
  m_nSectorsPerTELL1 = STDAQ::noptlinks*STDAQ::nports/m_nBeetlePortsPerSector;

  // Book histograms
  bookHistograms();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STClusterMonitor::execute() {

  if(m_debug) debug() << "==> Execute" << endmsg;

  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  if( NULL != odin ) {
    plot1D(odin->bunchId(),"BCID","BCID",-0.5,2807.5,2808);
  } else return Warning("No ODIN bank found", StatusCode::SUCCESS,1);

  counter("Number of events") += 1; 

  // code goes here  
  monitorClusters();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Look at the clusters histogram
//=============================================================================
void ST::STClusterMonitor::monitorClusters() {

  // Check location exists
  if(m_debug) debug() << "monitorClusters" << endmsg;
  const LHCb::STClusters* clusters = getIfExists<LHCb::STClusters>(m_clusterLocation);
  if( NULL != clusters ){
    m_nClustersPerTELL1.assign(m_nTELL1s,0);

    const unsigned int nClusters = clusters->size();
    if(m_debug) debug() << "Number of clusters in " << m_clusterLocation << " is " << nClusters << endmsg;

    if(nClusters < m_minNClusters || nClusters > m_maxNClusters) return;

    m_1d_nClusters->fill(nClusters);
    if(100  < nClusters) {
      m_1d_nClusters_gt_100->fill(nClusters);
    }
    if(m_overFlowLimit < nClusters) m_1d_nClusters_overflow->fill(m_overFlowLimit);
    else  m_1d_nClusters_overflow->fill(nClusters);
    // Loop over clusters
    LHCb::STClusters::const_iterator itClus = clusters->begin();
    for(; itClus != clusters->end(); ++itClus) {
      const LHCb::STCluster* cluster = (*itClus);
      fillHistograms(cluster);
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

  } else Warning("No clusters found at "+m_clusterLocation, StatusCode::SUCCESS,0).ignore(); // End of cluster exists

}

//================================================================================================================================
// Fill the MPV cluster maps 
// Three means are calculated:
// 1) Truncated mean using 70% of the distribution
// 2) Truncated mean using only the 1st 70% of the distribution
// 3) Binned mean with 20 bins from 0 to 100 ADC counts
//================================================================================================================================
void ST::STClusterMonitor::fillMPVMap(const DeSTSector* sector, double charge) {
  unsigned int sectorID = sector->elementID().uniqueSector();
  if(charge > m_minMPVCharge) { 
    std::vector<double>* sectorCharges = &m_chargeBySector[sectorID];
    TH1D* hSec = m_1ds_chargeBySector[sectorID];
    hSec->Fill(charge);
    sectorCharges->push_back(charge);
    if(sectorCharges->size() > m_resetRate) {
      std::sort(sectorCharges->begin(), sectorCharges->end());
      unsigned int fullRange = sectorCharges->end() - sectorCharges->begin();
      unsigned int usedRange = static_cast<unsigned int>(0.7*fullRange);
      unsigned int start = (fullRange-usedRange)/2;
      unsigned int stop = usedRange + (fullRange-usedRange)/2;
      double mean1=std::accumulate(sectorCharges->begin()+start, sectorCharges->begin()+stop, 0.);
      mean1  /= usedRange;
      double mean2=std::accumulate(sectorCharges->begin(), sectorCharges->begin()+usedRange, 0.);
      mean2 /= usedRange;
      std::vector<double> binnedMPV(20,0.);
      std::vector<double>::iterator iMPV=sectorCharges->begin();
      for(; iMPV != sectorCharges->end(); ++iMPV) {
        unsigned int bin = static_cast<int>((*iMPV) / 5.);
        if(bin < binnedMPV.size()) binnedMPV[bin] += 1;
      }
      int maxBin = std::max_element( binnedMPV.begin(), binnedMPV.end() ) - binnedMPV.begin();
      double mean3 = (maxBin*5.)+2.5;
      if(m_debug) {
        debug() << "Sector " << sectorID << ": truncMean=" << mean1
                << ", mean2(70%)=" << mean2
                << ", mean3(binned)=" << mean3
                << ", full=" << std::accumulate(sectorCharges->begin(), sectorCharges->end(), 0.)/fullRange
                << ", histo=" << (hSec->GetBinCenter(hSec->GetMaximumBin()))
                << endmsg;
      }
      unsigned int bin = m_sectorBins1D[sectorID];
      m_prof_sectorTruncMean1->fill(bin, mean1);
      m_prof_sectorTruncMean2->fill(bin, mean2);
      m_prof_sectorBinnedMPV->fill(bin, mean3);
      
      double mpv=mean3;
      m_prof_sectorMPVs->fill(bin, mpv);
      if(detType() == "TT") {
        const DeTTSector* ttSector = dynamic_cast<const DeTTSector* >(sector);
        ST::TTDetectorPlot hitMap("map","map");
        ST::TTDetectorPlot::Bins bins = hitMap.toBins(ttSector);
        double xBin = bins.xBin;
        // Hack to make real x-y distribution plot (not occupancy)
        for( int yBin = bins.beginBinY; yBin != bins.endBinY; ++yBin ) {
          m_2d_sectorMPVs->fill(xBin,yBin,mpv);
          m_2d_sectorMPVsNorm->fill(xBin,yBin,1.);
        }
      } else if(detType() == "UT") {
        const DeUTSector* utSector = dynamic_cast<const DeUTSector* >(sector);
        ST::UTDetectorPlot hitMap("map","map");
        ST::UTDetectorPlot::Bins bins = hitMap.toBins(utSector);
        double xBin = bins.xBin;
        for( int yBin = bins.beginBinY; yBin != bins.endBinY; ++yBin ) {
          m_2d_sectorMPVs->fill(xBin,yBin,mpv);
          m_2d_sectorMPVsNorm->fill(xBin,yBin,1.);
        }
      } else if( detType() == "IT" ) {// Cluster map for IT
        ST::ITDetectorPlot hitMap("map", "map");
        ST::ITDetectorPlot::Bins bins = hitMap.toBins(sector->elementID());
        // Hack to make real x-y distribution plot (not occupancy)
        m_2d_sectorMPVs->fill(bins.xBin, bins.yBin, mpv);
        m_2d_sectorMPVsNorm->fill(bins.xBin, bins.yBin, 1.);
      } else Warning("Invalid Sector",0,StatusCode::SUCCESS).ignore();
      sectorCharges->clear();
      //      if(debug) exit(0);
    }
  }
}

//==============================================================================
// Book histograms
//==============================================================================
void ST::STClusterMonitor::bookHistograms() {
  // filled in monitor clusters
  m_1d_nClusters = book1D("Number of clusters",0.,20000.,2000);
  m_1d_nClusters_gt_100 = book1D("Number of clusters (N > 100)", 0., 20000., 2000);
  m_1d_nClusters_overflow = book1D("Number of clusters (no overflow)", 0., m_overFlowLimit+1, 1000);
  m_2d_nClustersVsTELL1 = book2D("Number of clusters per TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0.,100., 50);

  // Number of clusters produced by each TELL1
  m_1d_nClustersVsTELL1 = book1D("Number of clusters vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
  m_prof_nClustersVsTELL1 = bookProfile1D("Mean number of clusters per TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
  m_1d_nClustersVsTELL1Links =  book1D("Number of clusters vs TELL1 links", 1., m_nTELL1s+1., m_nTELL1s*STDAQ::noptlinks);
  m_1d_nClustersVsTELL1Sectors =  book1D("Number of clusters vs TELL1 sectors", 1., m_nTELL1s+1., 
                                         m_nTELL1s*m_nSectorsPerTELL1);

  // filled in fillHistograms
  m_1d_ClusterSize = book1D("Cluster Size", 0.5, 4.5, 4); 
  m_2d_ClusterSizeVsTELL1 = book2D("Cluster Size vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0.5, 4.5, 4);
  m_2d_STNVsTELL1 = book2D("Signal to Noise vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 100., 25);
  m_2d_ChargeVsTELL1 = book2D("Cluster Charge vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 60., 60);
                              
  if(m_plotByPort) {
    m_2d_ClustersPerPortVsTELL1 = book2D("Clusters per port vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96);
  }
  m_1d_totalCharge = book1D("Cluster ADC Values", 0., 200., 200);
  if(detType() == "TT" || detType() == "UT" || m_plotBySvcBox) {
    /// list of service boxes  
    std::vector<std::string>::const_iterator itSvcBoxes = readoutTool()->serviceBoxes().begin();
    for( ; itSvcBoxes != readoutTool()->serviceBoxes().end(); ++itSvcBoxes) {
      std::string svcBox=(*itSvcBoxes);
      if(detType() == "TT" || detType() == "UT") {
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
    } else if(detType() == "UT"){
      names = LHCb::UTNames().allDetRegions();
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
    std::string idClSize = detType()+" cluster sizes";
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
      m_2dp_clSize_map = bookProfile2D(idClSize, MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
                                       MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
    } else if(detType()=="UT"){// Cluster maps for UT
      ST::UTDetectorPlot hitMap(idMap, idMap, m_nBeetlePortsPerUTSector);
      m_2d_hitmap = book2D(hitMap.name(), hitMap.minBinX(), hitMap.maxBinX(), hitMap.nBinX(),
                           hitMap.minBinY(), hitMap.maxBinY(), hitMap.nBinY());
      ST::UTDetectorPlot MPVMap(idMPVMap, idMPVMap);
      m_2d_sectorMPVs = book2D(MPVMap.name(), MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
                               MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
      m_2d_sectorMPVsNorm = book2D(MPVMap.name()+" Normalisation", MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
				   MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
      m_2dp_clSize_map = bookProfile2D(idClSize, MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
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
//       m_2d_clSize_map = book2D(idClSize, MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
//                                MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
      m_2dp_clSize_map = bookProfile2D(idClSize, MPVMap.minBinX(), MPVMap.maxBinX(), MPVMap.nBinX(),
                                       MPVMap.minBinY(), MPVMap.maxBinY(), MPVMap.nBinY());
    }
    // Create histogram of with total charge for each sector
    std::vector<DeSTSector*>::const_iterator Sectors = tracker()->sectors().begin();
    m_sectorBins1D.clear();
    int bin=1;
    for(; Sectors != tracker()->sectors().end(); ++Sectors, ++bin) {
      std::string idh = "charge_$sector"+std::to_string((*Sectors)->elementID().uniqueSector());
      std::string title = "Total charge in "+(*Sectors)->nickname();
      m_1ds_chargeBySector[(*Sectors)->elementID().uniqueSector()] 
        = Gaudi::Utils::Aida2ROOT::aida2root ( book1D("BySector/"+idh,title,0.,100.,50) );
      //      std::cout << title+":" << (*Sectors)->elementID().uniqueSector() << "," << bin << std::endl;
      m_sectorBins1D[(*Sectors)->elementID().uniqueSector()] = bin;
    }
    // 1d representation for online monitoring
    m_nSectors = m_sectorBins1D.size();
    m_1d_nClustersVsSector = book1D("Number of clusters per sector", 0.5, m_nSectors+0.5, m_nSectors);
    m_1d_nClustersVsBeetlePort = book1D("Number of clusters per beetle port", 
                                        0.5, m_nSectors+0.5, m_nSectors*m_nBeetlePortsPerSector);
    m_prof_sectorMPVs = bookProfile1D("Cluster MPV vs Sector", 0.5, m_nSectors+0.5, m_nSectors);
    m_prof_sectorTruncMean1 = bookProfile1D("Cluster trunc mean vs Sector", 0.5, m_nSectors+0.5, m_nSectors);
    m_prof_sectorTruncMean2 = bookProfile1D("Cluster trunc mean (1st 70%) vs Sector", 0.5, m_nSectors+0.5, m_nSectors);
    m_prof_sectorBinnedMPV = bookProfile1D("Cluster binned MPV vs Sector", 0.5, m_nSectors+0.5, m_nSectors);
  }// end of hitmaps

}

//==============================================================================
// Fill histograms
//==============================================================================
void ST::STClusterMonitor::fillHistograms(const LHCb::STCluster* cluster){

  const double totalCharge = cluster->totalCharge();
  if(totalCharge < m_chargeCut) return;

  // calculate MPVs
  //  unsigned int sectorID=cluster->channelID().uniqueSector();
  const DeSTSector* sector=findSector(cluster->channelID());
  if(m_hitMaps) {
    fillClusterMaps(cluster);
    fillMPVMap(sector, totalCharge);
    unsigned int sectorID = sector->elementID().uniqueSector();
    unsigned int bin = m_sectorBins1D[sectorID];
    m_1d_nClustersVsSector->fill(bin);
    unsigned int port = (cluster->channelID().strip()-1)/STDAQ::nstrips;
    m_1d_nClustersVsBeetlePort->fill((bin-0.5)+static_cast<double>(port)/m_nBeetlePortsPerSector);
  }

  const unsigned int clusterSize = cluster->size();

  const unsigned int sourceID = cluster->sourceID();
  unsigned int TELL1ID = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
  m_nClustersPerTELL1[TELL1ID-1] += 1;
  m_1d_nClustersVsTELL1->fill(TELL1ID);
  unsigned int tell1Link = cluster->tell1Channel()/128;
  m_1d_nClustersVsTELL1Links->fill((TELL1ID)+((0.5+tell1Link)/STDAQ::noptlinks));
  unsigned int tell1Sector = cluster->tell1Channel()/(m_nBeetlePortsPerSector*STDAQ::nstrips);
  m_1d_nClustersVsTELL1Sectors->fill((TELL1ID)+((0.5+tell1Sector)/m_nSectorsPerTELL1));

  m_1d_ClusterSize->fill(clusterSize);
  m_2d_ClusterSizeVsTELL1->fill(TELL1ID, clusterSize);

  double noise = sector->noise(cluster->channelID());
  if (noise > 1e-3) {
    const double signalToNoise = cluster->totalCharge()/noise;
    m_2d_STNVsTELL1->fill(TELL1ID, signalToNoise);
  } else Warning("Zero S/N for some clusters",StatusCode::SUCCESS,1).ignore();
  m_2d_ChargeVsTELL1->fill(TELL1ID, totalCharge);
  if(m_plotByPort) {
    const unsigned int tell1Channel = cluster->tell1Channel();
    unsigned int port = tell1Channel/STDAQ::nstrips;
    m_2d_ClustersPerPortVsTELL1->fill(TELL1ID, port);
  }
  // Always fill histograms per readout quadrant for TT
  // Get service box and set up histogram IDs
  m_1d_totalCharge->fill(totalCharge);
  std::string svcBox = readoutTool()->serviceBox(cluster->channelID());
  if(detType() == "TT" || detType() == "UT") {
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
  std::string idhStrip = " (" + std::to_string(clusterSize) + " strip)";
  std::string idh;
  if(detType() == "TT" || detType() == "UT") {
    idh = "Cluster ADCs in " + svcBox.substr(0,2)+idhStrip;
    plot1D(totalCharge,idh, idh,0.,200.,200);
  }
  idh = "Cluster ADCs in " + svcBox+idhStrip;
  plot1D(totalCharge,idh, idh,0.,200.,200);

  // Charge in each sector for 1, 2, 3 and 4 strip clusters
  idh = "BySector/s"+std::to_string(cluster->size())
    +"/charge_$sector"+std::to_string(cluster->channelID().uniqueSector());
  plot1D(totalCharge, idh, "Total charge in "+cluster->sectorName(),0.,200.,200);
}
//==============================================================================
/// Fill histograms of cluster maps
//==============================================================================
void ST::STClusterMonitor::fillClusterMaps(const LHCb::STCluster* cluster) {  
  std::string idMap = detType()+" cluster map";
  if(detType()=="TT"){// Cluster maps for TT
    const DeSTSector* aSector = tracker()->findSector(cluster->channelID());
    const DeTTSector* ttSector = dynamic_cast<const DeTTSector* >(aSector);

    // make the real hit map
    ST::TTDetectorPlot hitMap(idMap, idMap, m_nBeetlePortsPerTTSector);
    ST::TTDetectorPlot::Bins bins = hitMap.toBins(ttSector);
    int port = (cluster->channelID().strip()-1)/STDAQ::nstrips;
    double xBin = bins.xBin - double(port+1)/m_nBeetlePortsPerTTSector + 0.5;
    int nBins = bins.endBinY - bins.beginBinY;

    // cluster size by sector
    ST::TTDetectorPlot sizeMap("sizeMap","sizeMap");
    double clSizeBin = sizeMap.toBins(ttSector).xBin;

    for( int yBin = bins.beginBinY; yBin != bins.endBinY; ++yBin ) {
      m_2d_hitmap->fill(xBin,yBin,1./nBins);
      m_2dp_clSize_map->fill(clSizeBin, yBin, cluster->size());
    }
  } else if(detType()=="UT"){// Cluster maps for UT
    const DeSTSector* aSector = tracker()->findSector(cluster->channelID());
    const DeUTSector* utSector = dynamic_cast<const DeUTSector* >(aSector);

    // make the real hit map
    ST::UTDetectorPlot hitMap(idMap, idMap, m_nBeetlePortsPerUTSector);
    ST::UTDetectorPlot::Bins bins = hitMap.toBins(utSector);
    int port = (cluster->channelID().strip()-1)/STDAQ::nstrips;
    double xBin = bins.xBin - double(port+1)/m_nBeetlePortsPerUTSector + 0.5;
    int nBins = bins.endBinY - bins.beginBinY;

    // cluster size by sector
    ST::UTDetectorPlot sizeMap("sizeMap","sizeMap");
    double clSizeBin = sizeMap.toBins(utSector).xBin;

    for( int yBin = bins.beginBinY; yBin != bins.endBinY; ++yBin ) {
      m_2d_hitmap->fill(xBin,yBin,1./nBins);
      m_2dp_clSize_map->fill(clSizeBin, yBin, cluster->size());
    }
  } else if( detType() == "IT" ) {// Cluster map for IT
    ST::ITDetectorPlot hitMap(idMap, idMap, m_nBeetlePortsPerITSector);
    ST::ITDetectorPlot::Bins bins = hitMap.toBins(cluster->channelID());
    m_2d_hitmap->fill(bins.xBin, bins.yBin, 1.);

    ST::ITDetectorPlot sizeMap("sizeMap","sizeMap");
    double clSizeBin = sizeMap.toBins(cluster->channelID()).xBin;
//     m_2d_clSize_map->fill(clSizeBin, bins.yBin, cluster->size());
    m_2dp_clSize_map->fill(clSizeBin, bins.yBin, cluster->size());
    
  }
}
