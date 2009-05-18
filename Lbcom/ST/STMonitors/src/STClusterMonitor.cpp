// $Id: STClusterMonitor.cpp,v 1.2 2009-05-18 11:35:45 mtobin Exp $
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
#include "Kernel/ISTSignalToNoiseTool.h"


#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "Kernel/Trajectory.h"
// from Boost
#include <boost/assign/list_of.hpp>

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
  declareSTConfigProperty("ClusterLocation", m_clusterLocation, LHCb::STClusterLocation::TTClusters);

  /// Use signal to noise tool
  declareProperty("SignalToNoise", m_stn=false );
  // Signal to noise
  declareProperty("SignalToNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");

  /// Plots per service box
  declareProperty( "ByServiceBox", m_plotBySvcBox=false );

  /// Plots per detector region
  declareProperty( "ByDetectorRegion", m_plotByDetRegion=false );

  /// Plot hitmap for each layer
  declareProperty( "HitMaps", m_hitMaps=false );

  /// Set maximum number of clusters per TELL1
  declareProperty( "MaxClustersPerTELL1", m_maxNClusters=4000 );
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
  }

  // sig to noise tool
  if(m_stn) m_sigNoiseTool = tool<ISTSignalToNoiseTool>( m_sigNoiseToolName,
                                                         m_sigNoiseToolName + detType());

  // Get the tell1 mapping from source ID to tell1 number
  m_nTELL1s = SourceIDToTELLNumberMap().size();
  // Store number of clusters/TELL1 (48 Tell1s, 1->48)
  m_nClustersPerTELL1.resize(m_nTELL1s,0);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STClusterMonitor::execute() {

  debug() << "==> Execute" << endmsg;
  counter("Number of events") += 1; 

  // code goes here  
  monitorClusters();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ST::STClusterMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return ST::HistoAlgBase::finalize(); 
}

//=============================================================================
// Look at the clusters histogram
//=============================================================================
void ST::STClusterMonitor::monitorClusters() {
  // Check location exists
  if(exist<LHCb::STClusters>(m_clusterLocation)){
    m_nClustersPerTELL1.resize(m_nTELL1s,0);
    LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);
    LHCb::STClusters::const_iterator itClus;
    // Loop over clusters
    const unsigned int nClusters = clusters->size();
    if(m_debug) debug() << "Number of clusters in " << m_clusterLocation << " is " << nClusters << endreq;
    std::string id1DNClus="Number of clusters";
    plot1D(nClusters,id1DNClus,id1DNClus,-5.,5005.,501);
    if(100  < nClusters) {
      std::string id1DNClus="Number of clusters (N > 100)";
      plot1D(nClusters,id1DNClus,id1DNClus, 95.,5005., 491);
    }
    for(itClus = clusters->begin(); itClus != clusters->end(); ++itClus) {
      const LHCb::STCluster* cluster = (*itClus);
      fillHistograms(cluster);
      if(m_hitMaps) fillHitMaps(cluster);
      if(fullDetail()) fillDetailedHistograms(cluster);
    }// End of cluster iterator

    // Fill histogram for number of clusters/TELL1
    std::vector<unsigned int>::const_iterator itClPerTELL1;
    std::string histoIDClPerTELL1 = "Number of Clusters per TELL1";
    unsigned int TELL1=1;
    for(itClPerTELL1 = m_nClustersPerTELL1.begin(); itClPerTELL1 != m_nClustersPerTELL1.end(); ++itClPerTELL1, ++TELL1) {
      if(m_verbose) verbose() << "TELL1: " << TELL1 << ",clusters: " << (*itClPerTELL1) << endreq;
      unsigned int nClusters = (*itClPerTELL1);
      if(0 < nClusters) 
        plot2D(TELL1, nClusters,  histoIDClPerTELL1, histoIDClPerTELL1, 0.5, m_nTELL1s+0.5,-5.,505.,m_nTELL1s, 51);
    }
    m_nClustersPerTELL1.clear();
  } else Warning("No clusters found at "+m_clusterLocation, StatusCode::SUCCESS, 50).ignore(); // End of cluster exists
}
//==============================================================================
// Fill histograms
//==============================================================================
void ST::STClusterMonitor::fillHistograms(const LHCb::STCluster* cluster){
  const double totalCharge = cluster->totalCharge();
  const unsigned int clusterSize = cluster->size();
  const unsigned int sourceID = cluster->sourceID();
  unsigned int TELL1ID = (this->SourceIDToTELLNumberMap().find(sourceID))->second;
  m_nClustersPerTELL1[TELL1ID-1] += 1;
  plot2D(TELL1ID, clusterSize, "Cluster Size vs TELL1", "Cluster Size vs TELL1",  0.5, m_nTELL1s+0.5, 0.5, 4.5, m_nTELL1s, 4);
  if(m_stn) {
    plot2D(TELL1ID, m_sigNoiseTool->signalToNoise(cluster), "Signal to Noise vs TELL1", "Signal to Noise vs TELL1",  
           0.5, m_nTELL1s+0.5, -2.5, 102.5, m_nTELL1s, 21);
  }
  plot2D(TELL1ID, totalCharge, "Cluster Charge vs TELL1", "Cluster Charge vs TELL1",  
         0.5, m_nTELL1s+0.5, -5., 205., m_nTELL1s, 21);

  // Always fill histograms per readout quadrant for TT
  // Get service box and set up histogram IDs
  std::string id1DCharge = "Cluster ADC Values";
  plot1D(totalCharge, id1DCharge, id1DCharge, -1.5, 301.5, 101);
  std::string svcBox = (this->readoutTool())->serviceBox(cluster->firstChannel());
  if(detType() == "TT") {
    std::string quadrant = svcBox.substr(0,2);
    std::string id1DCharge = "Cluster ADC Values "+quadrant;    
    plot1D(totalCharge, id1DCharge, id1DCharge, -1.5, 301.5, 101);
  }
  if(m_plotBySvcBox) {
    id1DCharge += " "+svcBox;
    plot1D(totalCharge, id1DCharge, id1DCharge, -1.5, 301.5, 101);
  }
  if(m_plotByDetRegion) {
    id1DCharge += " "+cluster->detRegionName();
    plot1D(totalCharge, id1DCharge, id1DCharge, -1.5, 301.5, 101);
  }
}
//==============================================================================
/// Fill more detailed histograms
//==============================================================================
void ST::STClusterMonitor::fillDetailedHistograms(const LHCb::STCluster* cluster) {
  // high threshold
  plot(cluster->highThreshold(),"High threshold","High threshold",-0.5,1.5,2);

  // by strip, modulo a few things....
  const unsigned int strip = cluster->strip();
  plot1D(strip%8,"strip modulo 8","strip modulo 8", -0.5, 8.5, 9);
  plot1D(strip%32,"strip modulo 32","strip modulo 32", -0.5, 32.5, 33);
  plot1D(strip%128,"strip modulo 128","strip modulo 128", -0.5, 128.5, 129);

  // histogram by station
  const int station = cluster->station();
  plot1D(station, "Number of clusters per station", "Number of clusters per station", -0.5, 4.5, 5);
 
  // by layer
  const int layer = cluster->layer();
  plot((double)(10*station+layer), "Number of clusters per layer", "Number of clusters per layer", -0.5, 40.5, 41);

  const double neighbourSum = cluster->neighbourSum();
  plot1D(cluster->pseudoSize(), "Pseudo size of cluster", "Pseudo size of cluster",-0.5, 10.5,11); 

  const double totalCharge = cluster->totalCharge();
  plot1D(neighbourSum/totalCharge,"Relative neighbour sum","Relative neighbour sum", -1.02, 1.02, 51);

  const unsigned int clusterSize = cluster->size();
  if(3 > clusterSize) {
    plot1D(neighbourSum,"Neighbour sum (1- and 2-strip clusters)","Neighbour sum (1- and 2-strip clusters)",-16.5, 26.5, 43);
    plot1D(neighbourSum/totalCharge, "Relative neighbour sum (1- and 2-strip clusters)",
           "Relative neighbour sum (1- and 2-strip clusters)", -1.02, 1.02, 51);
  }
  // charge and S/N
  plot1D(totalCharge,cluster->layerName()+"/Charge", "Charge",-0.5, 500.5, 501);
  if(m_stn) plot1D(m_sigNoiseTool->signalToNoise(cluster),cluster->layerName()+"/Signal to noise","S/N",0., 100., 100);
}
//==============================================================================
/// Fill histograms of hitmaps
//==============================================================================
void ST::STClusterMonitor::fillHitMaps(const LHCb::STCluster* cluster) {  
  if(detType()=="TT"){// Cluster maps for TT
    std::string idMap = "Cluster map for "+cluster->layerName();
    const DeSTSector* aSector = (this->tracker())->findSector(cluster->channelID());
    std::auto_ptr<LHCb::Trajectory> tmpTraj = aSector->trajectory(cluster->channelID() , cluster->interStripFraction() ); 
    Gaudi::XYZPoint begin= tmpTraj->beginPoint();
    Gaudi::XYZPoint end  = tmpTraj->endPoint();
    Gaudi::XYZPoint middle = begin + (end-begin)/2;
    // Plot hitmap with 17 ladders, 4 chips, 5 bins == 17*4
    plot2D(middle.x(), middle.y(), idMap, idMap, -850., 850., -700., 700., 170, 14);
  }else if(detType() == "IT") {// Cluster map for IT
    // Look at plot and try to make differently?????
    int originx=0;
    int originy=0;
    int distToOriginx=0;
    // origin points calculation (lower left corner of each box)
    LHCb::STChannelID channel = cluster->channelID();
    int box=channel.detRegion();
    int station=channel.station();
    if(1 == box){// box c-side 
      originx=168; 
      originy=(station-1)*13+4;
    } else if(3 == box){// box bottom
      originx=84;
      originy=(station-1)*13;
    } else if(4 == box){// box top
      originx=84;
      originy=(station-1)*13+8;
    } else if(2 == box){// box a-side
      originx=0;
      originy=(station-1)*13+4;
    }
    // Calculation of distance of port hit by cluster from origin point
    distToOriginx=84-((channel.sector()-1)*384+channel.strip()-1)/32;
    const int iXcoord = originx+distToOriginx;
    const int iYcoord= channel.layer()+originy;
    plot2D(iXcoord,iYcoord, "Cluster Map", "Cluster Map", 1., 253., 1., 39., 252, 1);
  }
}
