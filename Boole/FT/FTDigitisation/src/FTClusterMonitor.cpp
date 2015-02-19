// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from FTEvent
#include "Event/FTCluster.h"

// local
#include "FTClusterMonitor.h"

// from C
#include <sstream>

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : FTClusterMonitor
//
// 2012-07-05 : Eric Cogneras
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FTClusterMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FTClusterMonitor::FTClusterMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("ClusterLocation" , m_clusterLocation =  LHCb::FTClusterLocation::Default, "Path to Clusters");
}
//=============================================================================
// Destructor
//=============================================================================
FTClusterMonitor::~FTClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTClusterMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTClusterMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // retrieve FTCluster
  const FTClusters* clustersCont = get<FTClusters>(m_clusterLocation);
  if ( msgLevel(MSG::DEBUG) )
    debug() <<"clustersCont->size() : " << clustersCont->size()<< endmsg;


  unsigned int m_nbBanks = 48; //FTRawBank::NbBanks;
  unsigned int m_nbSipmPerTELL40 = 128; //FTRawBank::NbSiPMPerTELL40;
  std::vector<std::vector<std::vector<unsigned int> > > m_sipmData; // m_sipmData[banknr][sipmnr][array of encoded clusters]
  std::vector<std::vector<unsigned int> > temp(m_nbSipmPerTELL40);
  m_sipmData.resize( m_nbBanks, temp );
  for ( unsigned int iBank = 0; m_sipmData.size() > iBank; ++iBank ) {
    for ( unsigned int iPm = 0; m_sipmData[iBank].size() > iPm; ++iPm ) {
      m_sipmData[iBank][iPm].clear();
    }
  }

  

  // Loop over FTCluster
  for (FTClusters::const_iterator iterCluster = clustersCont->begin(); iterCluster!=clustersCont->end();++iterCluster){
    FTCluster* cluster = *iterCluster;

    plot(cluster->channelID(), "ClusChannelID", "Cluster ChannelID; Cluster ChannelID" ,  0. , 1000000. ,10000);
    plot(cluster->fraction(), "ClusFraction",  "Cluster Fractional part; Cluster fractional part" , 0 , 1);
    plot(cluster->size(),"ClusSize", "Cluster Size; Cluster Size" , 0 , 100);
    plot(cluster->charge(),"ClusCharge","Cluster Charge; Cluster Charge" , 0 , 100);
    plot(cluster->charge(),"ClusChargeZoom","Cluster Charge (Zoom); Cluster Charge" , 0. , 16., 16);
    plot2D(cluster->size(), cluster->charge(), "ClusChargevsSize",
        "Cluster Charge vs. Cluster Size; Cluster Charge [ADC counts]; Cluster size [Nb of Channels]" , 0. , 16. ,16, 0. , 100., 100);

    // encoding details
    LHCb::FTChannelID id = cluster->channelID();
    unsigned int clusBanknr = id.quarter() + 4*(id.layer()); // [0-3] + 4*[0-11] = [0-47]
    unsigned int clusSiPMnr = id.sipmId() + 16*QuarterModule(id.module());
    m_sipmData[clusBanknr][clusSiPMnr].push_back( 1 ); // could store e.g. charge

    // draw nClus over threshold
    if(clusBanknr == 0) {
      plot(clusSiPMnr, "ENC_nClus_bankID0", "nClus bankID0; sipmId + 16*QuarterModule", 0., 128., 128);
      for (unsigned int maxClus = 15; maxClus > 3; --maxClus) {
        if( m_sipmData[clusBanknr][clusSiPMnr].size() > maxClus ) {
          std::stringstream title;
          title << "ENC_nClusMissed_bankID0_maxClus" << maxClus;
          plot(clusSiPMnr, title.str().c_str(), "nClus not encoded (bankID0); sipmId + 16*QuarterModule", 0., 128., 128);
        }
      }
    }

  }

  // 
  for (unsigned int sipmID=0; sipmID < m_nbSipmPerTELL40; ++sipmID) {
    plot2D(sipmID, m_sipmData[0][sipmID].size(), "ClusterPerSiPML0",
        "Clusters per SiPM Distr. (L0); sipmID + 16*QuarterModule; nClusters", 0., 128., 0., 30., 128,30);
  }

  // draw Events over threshold
  for (unsigned int maxClus = 15; maxClus > 3; --maxClus) {
    for (unsigned int sipmID=0; sipmID < m_nbSipmPerTELL40; ++sipmID) {
      if( m_sipmData[0][sipmID].size() > maxClus ) {
        std::stringstream title;
        title << "ENC_evtOverThreshold_bankID0_maxClus" << maxClus;
        plot(sipmID, title.str().c_str(), "nEvents over threshold (bankID0); sipmId + 16*QuarterModule", 0., 128., 128);
      }
    }
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
unsigned int FTClusterMonitor::QuarterModule(unsigned int module)
{
  unsigned int quarterModuleNber = 9;
  if(module < 5) quarterModuleNber = module;
  else 
  {
    quarterModuleNber = module - 4;
    if(module == 10) quarterModuleNber = 5;
    if(module == 11) quarterModuleNber = 0;
  }
  return quarterModuleNber;
}
