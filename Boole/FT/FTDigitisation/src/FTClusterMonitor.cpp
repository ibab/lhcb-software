// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Linker
#include "Linker/AllLinks.h"
#include "Linker/LinkedTo.h"
// from FTEvent
#include "Event/FTCluster.h"

// FTDet
#include "FTDet/DeFTDetector.h"
#include "FTDet/DeFTLayer.h"

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
  declareProperty("RawBankFractionGranularity" , m_rawBankFractionGranularity ="1/4","Raw Bank Fraction Granularity");
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

  /// Retrieve and initialize DeFT (no test: exception in case of failure)
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );

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


  // retrieve FTClustertoMCHitLink
  LinkedTo<LHCb::MCHit, LHCb::FTCluster> myClusterToHitLink ( evtSvc(), msgSvc(), std::string(LHCb::FTClusterLocation::Default)+"2MCHits" );

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
    //plot(cluster->charge(),"ClusChargeZoom","Cluster Charge (Zoom); Cluster Charge" , 0. , 16., 16);
    plot2D(cluster->size(), cluster->charge(), "ClusChargevsSize",
           "Cluster Charge vs. Cluster Size; Cluster size [Nb of Channels]; Cluster Charge [ADC counts]", 0., 10., 0., 50., 10, 100);

    // Start plotting of cluster position resolution according to the Cluster<->MCHit matching
    const LHCb::MCHit* largestHit = myClusterToHitLink.first(cluster);
          
         
    const DeFTFibreMat* pL = m_deFT->findFibreMat(largestHit->midPoint());
    if( pL ) {
      std::pair<LHCb::FTChannelID, double> hitMeanChannel;
      if( pL->calculateMeanChannel(largestHit, hitMeanChannel)){
        // default
        //double dXCluster = (((double)hitMeanChannel.first-(double)cluster->channelID()) + 
        //                    (hitMeanChannel.second - cluster->fraction()))*250.; // in micrometer
        // evaluate effect of 1/4 channel resolution on cluster position resolution 
        double ClusterFractionFromBank = 0;
        if(m_rawBankFractionGranularity == "1/4") ClusterFractionFromBank = std::floor(4*cluster->fraction())/4.;
        if(m_rawBankFractionGranularity == "1/2") ClusterFractionFromBank = std::floor(2*cluster->fraction())/2.;
        if(m_rawBankFractionGranularity == "full") ClusterFractionFromBank =cluster->fraction();
        double dXClusterCh = (((double)cluster->channelID()-(double)hitMeanChannel.first)+ 
                              (ClusterFractionFromBank - hitMeanChannel.second));// in channel unit
        double dXCluster = (((double)cluster->channelID() - (double)hitMeanChannel.first)+ 
                            (ClusterFractionFromBank - hitMeanChannel.second))*0.250; // in micrometer

        plot(ClusterFractionFromBank, "Cluster_FractionFromBank",  "Cluster Fractional part (Raw Bank granularity "+m_rawBankFractionGranularity+"); Cluster fractional part" , 0 , 1);
        plot2D(cluster->size(),ClusterFractionFromBank, "Cluster_Size_VS_FractionFromBank",  "Cluster Size VS Cluster Fractional part (Raw Bank granularity "+m_rawBankFractionGranularity+"); Cluster Size; Cluster fractional part" , 0. , 5., 0., 1.);
        plot2D(hitMeanChannel.first.layer(),hitMeanChannel.first.sipmId(), "Cluster_MCHit_layer_VS_SiPMID", 
               "Cluster  MCHit Layer; Cluster MCHit Layer ; Cluster MCHit SiPMID",0.,20.,0.,20.,20,20); 
        plot2D(hitMeanChannel.first.layer(),hitMeanChannel.first.sipmCell(), "Cluster_MCHit_layer_VS_SiPMCell", 
               "Cluster  MCHit Layer; Cluster MCHit Layer ; Cluster MCHit SiPMCell",0.,20.,0.,128.,20,128); 
        plot(hitMeanChannel.first.layer(), "Cluster_MCHit_layer", "Cluster  MCHit Layer; Cluster MCHit Layer ; Number of clusters",0.,20.,20); 
        plot(dXCluster, "Cluster_x_position_resolution", "Cluster x position (from MCHit) ; Cluster X position - MCHit X position [mm] ; Number of clusters",-1,1,100); 
        plot(dXCluster, "Cluster_x_position_resolutionZOOM", "Cluster x position (from MCHit) ; Cluster X position - MCHit X position [mm] ; Number of clusters",-.2,.2,400); 
        plot2D(dXCluster, cluster->fraction(),"Cluster_resolutionVSFraction", 
               "Cluster x position (from MCHit) VS Fraction; Cluster X position - MCHit X position [mm] ; Fraction"
               , -60., 60., 0, 1 ,400, 100); 
        plot2D(dXCluster, cluster->channelID().layer(),"Cluster_resolutionVSLayer", 
               "Cluster x position (from MCHit) VS Layer ; Cluster X position - MCHit X position [mm] ; Layer"
               , -60., 60., 0., 12. ,400, 12); 
        plot2D(dXCluster, hitMeanChannel.first.layer(),"Cluster_resolutionVSHitLayer", 
               "Cluster x position (from MCHit) VS Layer ; Cluster X position - MCHit X position [mm] ; Hit Layer"
               , -60., 60., 0., 16. ,400, 16); 
        plot2D(cluster->channelID().layer(), hitMeanChannel.first.layer(),"ClusterLayerVSHitLayer","Cluster Layer VS Hit Layer ; Cluster Layer ; Hit Layer", 0., 16., 0., 16. ,16, 16);
        plot2D(dXCluster, cluster->channelID().sipmCell(),"Cluster_resolutionVSCell", 
               "Cluster x position (from MCHit) VS Cell ; Cluster X position - MCHit X position [mm] ; SiPM Cell"
               , -60., 60., 0., 128., 400, 128); 
        plot2D(dXCluster, cluster->channelID(),"Cluster_resolutionVSChannelID", 
               "Cluster x position (from MCHit) VS ChannelID ; Cluster X position - MCHit X position [mm] ; SiPM ChannelID", -60., 60., 0., 589824., 100, 4068); 
        plot2D(dXCluster, cluster->fraction(),"Cluster_resolutionVSFractionZOOM", 
               "Cluster x position (from MCHit) VS Fraction; Cluster X position - MCHit X position [mm] ; Fraction"
               , -1., 1., 0, 1 , 100, 100); 
        plot2D(dXCluster, cluster->channelID().layer(),"Cluster_resolutionVSLayerZOOM", 
               "Cluster x position (from MCHit) VS Layer ; Cluster X position - MCHit X position [mm] ; Layer"
               , -1., 1., 0., 12., 100, 12); 
        plot2D(dXCluster, cluster->channelID().sipmCell(),"Cluster_resolutionVSCellZOOM", 
               "Cluster x position (from MCHit) VS Cell ; Cluster X position - MCHit X position [mm] ; SiPM Cell"
               , -1., 1., 0., 128. ,100, 128); 
        plot2D(dXCluster, cluster->channelID(),"Cluster_resolutionVSChannelIDZOOM", 
               "Cluster x position (from MCHit) VS ChannelID ; Cluster X position - MCHit X position [mm] ; SiPM ChannelID", -1., 1., 0., 589824. ,100, 4068); 
        plot2D(largestHit->midPoint().x(), cluster->channelID(),"ClusterChannelIDVSHitXPosition", 
               "Cluster ChannelID VS Hit X Position; Hit x-position [mm] ; Cluster ChannelID"
               , -3000., 3000., 0., 800000. ,2000, 8000); 
        plot2D(cluster->channelID(),largestHit->midPoint().x(), "HitXPositionVSClusterChannelID", 
               "Hit X Position VS Cluster ChannelID ; Cluster ChannelID; Hit x-position [mm]"
               ,  0., 800000., -3000., 3000., 8000 ,2000); 
        if (cluster->size() == 1 ) {
          plot(dXCluster, "Cluster_x_position_resolutionZOOM_1Ch", 
               "Cluster x position (from MCHit, 1 channel); Cluster X position - MCHit X position (1 channel) [mm];Number of clusters",-0.2, 0.2, 400);
          plot(dXCluster, "Cluster_x_position_resolution_1Ch",
               "Cluster x position (from MCHit, 1 channel); Cluster X position - MCHit X position (1 channel) [mm];Number of clusters",-1., 1., 200);
          plot(dXClusterCh, "Cluster_Channel_resolution_1Ch",
               "Cluster channel (from MCHit, 1 channel); Cluster Channel - MCHit Channel (1 channel) [channel unit];Number of clusters",-5., 5., 200);


          // Resolution of the cluster as a function of the SiPM Cell
          if(cluster->channelID().sipmCell() == 0)
          {
            plot(dXCluster, "Cluster_x_position_resolution_1Ch_Cell0",
                 "Cluster x position (from MCHit, 1 channel) Cell0 ; Cluster (1 channel) position resolution [mm];Number of clusters",-1., 1., 200);
            plot(dXClusterCh, "Cluster_Channel_resolution_1Ch_Cell0",
                 "Cluster channel (from MCHit, 1 channel) Cell0 ; Cluster Channel - MCHit Channel (1 channel) [channel unit];Number of clusters",-5., 5., 200);
          }
                  
          else if(cluster->channelID().sipmCell() == 63)
          {
            plot(dXCluster, "Cluster_x_position_resolution_1Ch_Cell63",
                 "Cluster x position (from MCHit, 1 channel) Cell63 ; Cluster X position - MCHit X position (1 channel) [mm];Number of clusters",-1., 1., 200);
            plot(dXClusterCh, "Cluster_Channel_resolution_1Ch_Cell63",
                 "Cluster channel (from MCHit, 1 channel) Cell63 ; Cluster Channel - MCHit Channel (1 channel) [channel unit];Number of clusters",-5., 5., 200);
          }
          else if(cluster->channelID().sipmCell() == 64)
          {
            plot(dXCluster, "Cluster_x_position_resolution_1Ch_Cell64",
                 "Cluster x position (from MCHit, 1 channel) Cell64 ; Cluster X position - MCHit X position (1 channel) [mm];Number of clusters",-1., 1., 200);
            plot(dXClusterCh, "Cluster_Channel_resolution_1Ch_Cell64",
                 "Cluster channel (from MCHit, 1 channel) Cell64 ; Cluster Channel - MCHit Channel (1 channel) [channel unit];Number of clusters",-5., 5., 200);
          }
          else if(cluster->channelID().sipmCell() == 127)
          {
            plot(dXCluster, "Cluster_x_position_resolution_1Ch_Cell127",
                 "Cluster x position (from MCHit, 1 channel) Cell127 ; Cluster X position - MCHit X position (1 channel) [mm];Number of clusters",-1., 1., 200);
            plot(dXClusterCh, "Cluster_Channel_resolution_1Ch_Cell127",
                 "Cluster channel (from MCHit, 1 channel) Cell127 ; Cluster Channel - MCHit Channel (1 channel) [channel unit];Number of clusters",-5., 5., 200);
          }
          else
          {
            plot(dXCluster, "Cluster_x_position_resolution_1Ch_WithNeighbouringCell",
                 "Cluster x position (from MCHit, 1 channel) WithNeighbouringCell ; Cluster X position - MCHit X position (1 channel) [mm];Number of clusters",-1., 1., 200);
            plot(dXClusterCh, "Cluster_Channel_resolution_1Ch_WithNeighbouringCell",
                 "Cluster channel (from MCHit, 1 channel) WithNeighbouringCell ; Cluster Channel - MCHit Channel (1 channel) [channel unit];Number of clusters",-5., 5., 200);
          }

          
                  

          plot2D(dXCluster,  cluster->channelID().sipmCell(), "Resol_vs_Cell_1CH", 
                 "resol vs Cell ;Cluster X position - MCHit X position (1 channel) [mm]; sipmCell"
                 , -1., 1., 0., 134., 100, 134);
          if(std::abs(dXCluster) > 200) {
            plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(), "SiPM_vs_Layer_1CH_up200", 
                   "SiPM vs Layer (1CH cluster with resol > 200 #mum) ;sipmCell ; layer", 0., 134.,0., 16., 134,16);
            if(cluster->channelID().sipmCell() <10)
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(),"SiPMbegin_vs_Layer_1CH_up200", 
                     "SiPM begin vs Layer (1CH cluster with resol > 200 #mum) ;sipmCell ; layer", 0., 10.,0., 16., 10,16);
            if(cluster->channelID().sipmCell() >100)
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(), "SiPMend_vs_Layer_1CH_up200", 
                     "SiPM end vs Layer (1CH cluster with resol > 200 #mum) ;sipmCell ; layer", 120., 130.,0., 16., 10,16);
            if((cluster->channelID().sipmCell() <70)&&(cluster->channelID().sipmCell() >60))
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(),"SiPMmiddle_vs_Layer_1CH_up200", 
                     "SiPM middle vs Layer (1CH cluster with resol > 200 #mum) ;sipmCell ; layer", 60., 70.,0., 16., 10,16);
          }
        } else if (cluster->size() == 2 ) {
          plot(dXCluster, "Cluster_x_position_resolutionZOOM_2Ch",
               "Cluster x position (from MCHit, 2 channels); Cluster (2 channels) position resolution [mm] ; Number of clusters"
               , -0.2, 0.2, 400);
          plot(dXCluster, "Cluster_x_position_resolution_2Ch",
               "Cluster x position (from MCHit, 2 channels); Cluster (2 channels) position resolution [mm] ; Number of clusters"
               , -1., 1., 100);
          if(std::abs(dXCluster) > 200) {
            plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(), "SiPM_vs_Layer_2CH_up200", 
                   "SiPM vs Layer (2CH cluster with resol > 200 #mum) ;sipmCell ; layer", 0., 134., 0., 16., 134, 16);
            if(cluster->channelID().sipmCell() <10)
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(),"SiPMbegin_vs_Layer_2CH_up200", 
                     "SiPM begin vs Layer (2CH cluster with resol > 200 #mum) ;sipmCell ; layer", 0., 10., 0., 16., 10, 16);
            if(cluster->channelID().sipmCell() >100)
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(),"SiPMend_vs_Layer_2CH_up200",
                     "SiPM end vs Layer (2CH cluster with resol > 200 #mum) ;sipmCell ; layer", 120., 130., 0., 16., 10, 16);
            if((cluster->channelID().sipmCell() <70)&&(cluster->channelID().sipmCell() >60))
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(),"SiPMmiddle_vs_Layer_2CH_up200", 
                     "SiPM middle vs Layer (2CH cluster with resol > 200 #mum) ;sipmCell ; layer", 60., 70., 0., 16., 10, 16);
          }
        } else if (cluster->size() == 3 ) {
          plot(dXCluster, "Cluster_x_position_resolutionZOOM_3Ch",
               "Cluster x position (from MCHit, 3 channels); Cluster (3 channels) position resolution [mm] ; Number of clusters"
               , -0.2, 0.2, 400);
          plot(dXCluster, "Cluster_x_position_resolution_3Ch",
               "Cluster x position (from MCHit, 3 channels); Cluster (3 channels) position resolution [mm] ; Number of clusters"
               ,-1., 1., 100);
          if(std::abs(dXCluster) > 200) {
            plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(), "SiPM_vs_Layer_3CH_up200", 
                   "SiPM vs Layer (3CH cluster with resol > 200 #mum) ;sipmCell ; layer", 0., 134., 0., 16., 134, 16);
            if(cluster->channelID().sipmCell() <10)
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(), "SiPMbegin_vs_Layer_3CH_up200", 
                     "SiPM begin vs Layer (3CH cluster with resol > 200 #mum) ;sipmCell ; layer", 0., 10., 0., 16., 10, 16);
            if(cluster->channelID().sipmCell() >100)
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(),"SiPMend_vs_Layer_3CH_up200", 
                     "SiPM end vs Layer (3CH cluster with resol > 200 #mum) ;sipmCell ; layer", 120., 130., 0., 16., 10, 16);
            if((cluster->channelID().sipmCell() <70)&&(cluster->channelID().sipmCell() >60))
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(),"SiPMmiddle_vs_Layer_3CH_up200", 
                     "SiPM middle vs Layer (3CH cluster with resol > 200 #mum) ;sipmCell ; layer", 60., 70., 0., 16., 10, 16);
          }
        } else if (cluster->size() == 4 ) {
          plot(dXCluster, "Cluster_x_position_resolutionZOOM_4Ch",
               "Cluster x position (from MCHit, 4 channels); Cluster (4 channels) position resolution [mm] ; Number of clusters"
               , -0.2, 0.2, 400);
          plot(dXCluster, "Cluster_x_position_resolution_4Ch",
               "Cluster x position (from MCHit, 4 channels); Cluster (4 channels) position resolution [mm] ; Number of clusters"
               , -1., 1., 100);
          if(std::abs(dXCluster) > 200) {
            plot2D(  cluster->channelID().sipmCell(), cluster->channelID().layer(), "SiPM_vs_Layer_4CH_up200", 
                     "SiPM vs Layer (4CH cluster with resol > 200 #mum) ;sipmCell ; layer", 0., 134., 0., 16., 134, 16);
            if(cluster->channelID().sipmCell() <10)
              plot2D(cluster->channelID().sipmCell(), cluster->channelID().layer(),"SiPMbegin_vs_Layer_4CH_up200", 
                     "SiPM begin vs Layer (4CH cluster with resol > 200 #mum) ;sipmCell ; layer", 0., 10., 0., 16., 10, 16);
            if(cluster->channelID().sipmCell() >100)
              plot2D(  cluster->channelID().sipmCell(), cluster->channelID().layer(), "SiPMend_vs_Layer_4CH_up200", 
                       "SiPM end vs Layer (4CH cluster with resol > 200 #mum) ;sipmCell ; layer", 120., 130., 0., 16., 10, 16);
            if((cluster->channelID().sipmCell() <70)&&(cluster->channelID().sipmCell() >60))
              plot2D(  cluster->channelID().sipmCell(), cluster->channelID().layer(), "SiPMmiddle_vs_Layer_4CH_up200", 
                       "SiPM middle vs Layer (4CH cluster with resol > 200 #mum) ;sipmCell ; layer", 60., 70., 0., 16., 10,16);
          }
        }
      }
    }// end plotting of cluster position resolution according to the Cluster<->MCHit matching


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
