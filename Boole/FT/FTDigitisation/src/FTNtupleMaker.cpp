// Include files 

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 
//#include "GaudiAlg/Tuples.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// Linker
#include "Linker/AllLinks.h"
#include "Linker/LinkedTo.h"
// from FTEvent
#include "Event/MCHeader.h"
#include "Event/MCFTDigit.h"
#include "Event/FTCluster.h"

 #include "CLHEP/Matrix/Matrix.h"

// FTDet
#include "FTDet/DeFTDetector.h"
#include "FTDet/DeFTLayer.h"

// local
#include "FTNtupleMaker.h"


using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : FTNtupleMaker
//
// 2015-04-14 : Eric Cogneras
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FTNtupleMaker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FTNtupleMaker::FTNtupleMaker( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "NtupleName"    , m_tupleName       = "FTClusterCheck"    );
  declareProperty("FTHitLocation"  , m_hitLocation     = LHCb::MCHitLocation::FT, "Path to MCFTHits");
  declareProperty("DigitLocation"  , m_digitLocation   = LHCb::MCFTDigitLocation::Default, "Path to MCFTDeposits");
  declareProperty("ClusterLocation", m_clusterLocation = LHCb::FTClusterLocation::Default, "Path to Clusters");
  declareProperty("NtupHitMaxSize", m_ntupHitMaxSize = 150000, "Max Size of Hit Leafs in Ntuple");
  declareProperty("NtupDigitMaxSize", m_ntupDigitMaxSize = 150000, "Max Size of Digit Leafs in Ntuple");
  declareProperty("NtupClusterMaxSize", m_ntupClusterMaxSize = 50000, "Max Size of Cluster Leafs in Ntuple");
}
//=============================================================================
// Destructor
//=============================================================================
FTNtupleMaker::~FTNtupleMaker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTNtupleMaker::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Retrieve and initialize DeFT (no test: exception in case of failure)
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTNtupleMaker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;



  Tuple tuple = nTuple(m_tupleName);

  // Get the run and event number from the MC Header
  LHCb::MCHeader* evt = get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default,
                                              IgnoreRootInTES                 );
  // fill Ntuple with MC information
  tuple->column("RunId",evt->runNumber());
  tuple->column("EvtId",evt->evtNumber());

  // retrieve FTHits
  const MCHits* mcHitsCont = getIfExists<MCHits>(m_hitLocation);
  if ( msgLevel(MSG::DEBUG) ) 
    debug() <<"mcHitsCont->size() : " << mcHitsCont->size()<< endmsg;  

  int nbHit  = 0;
  std::vector<double> HitX;
  std::vector<double> HitY;
  std::vector<double> HitZ;
  std::vector<double> HitEnergy;
  std::vector<double> HitPID;
  std::vector<double> HitPartPx;
  std::vector<double> HitPartPy;
  std::vector<double> HitPartPz;
  std::vector<double> HitPartE;
  std::vector<uint> HitChannelID;
  std::vector<uint> HitLayer;
  std::vector<uint> HitQuarter;
  std::vector<uint> HitModule;
  std::vector<uint> HitMat;
  std::vector<uint> HitSiPMId;
  std::vector<uint> HitSiPMCell;
  std::vector<double> HitFraction;
  std::vector<uint> HitSize;
  std::vector<uint> HitChannelID_Start;
  std::vector<uint> HitLayer_Start;
  std::vector<uint> HitQuarter_Start;
  std::vector<uint> HitModule_Start;
  std::vector<uint> HitMat_Start;
  std::vector<uint> HitSiPMId_Start;
  std::vector<uint> HitSiPMCell_Start;
  std::vector<double> HitFraction_Start;
  std::vector<uint> HitChannelID_End;
  std::vector<uint> HitLayer_End;
  std::vector<uint> HitQuarter_End;
  std::vector<uint> HitModule_End;
  std::vector<uint> HitMat_End;
  std::vector<uint> HitSiPMId_End;
  std::vector<uint> HitSiPMCell_End;
  std::vector<double> HitFraction_End;
  std::vector<uint> HitChannelID_List;
  std::vector<uint> HitLayer_List;
  std::vector<uint> HitQuarter_List;
  std::vector<uint> HitModule_List;
  std::vector<uint> HitMat_List;
  std::vector<uint> HitSiPMId_List;
  std::vector<uint> HitSiPMCell_List;
  std::vector<double> HitFraction_List;

  for (MCHit* iterHit : *mcHitsCont){
    ++nbHit;
    MCHit* mcHit = iterHit; 
    const DeFTFibreMat* pL = m_deFT->findFibreMat(mcHit->midPoint());
    std::pair<LHCb::FTChannelID, double>  hitchannel;
    std::vector <std::pair<LHCb::FTChannelID, double> > hitchannelVec;
    if( ( pL ) && ( pL->calculateMeanChannel( mcHit, hitchannel)) ) {
      HitX.push_back( mcHit->midPoint().x() );
      HitY.push_back( mcHit->midPoint().y() );
      HitZ.push_back( mcHit->midPoint().z() );
      HitEnergy.push_back( mcHit->energy() );
      HitPID.push_back( mcHit->mcParticle()->particleID().pid() );
      HitPartPx.push_back( mcHit->mcParticle()->momentum().x() );
      HitPartPy.push_back( mcHit->mcParticle()->momentum().y() );
      HitPartPz.push_back( mcHit->mcParticle()->momentum().z() );
      HitPartE.push_back( mcHit->mcParticle()->momentum().e() );
      HitChannelID.push_back( hitchannel.first.channelID() );
      HitLayer.push_back( hitchannel.first.layer() );
      HitQuarter.push_back( hitchannel.first.quarter() );
      HitModule.push_back( hitchannel.first.module() );
      HitMat.push_back( hitchannel.first.mat() );
      HitSiPMId.push_back( hitchannel.first.sipmId() );
      HitSiPMCell.push_back( hitchannel.first.sipmCell() );
      HitFraction.push_back( hitchannel.second );
      if( pL->calculateListOfFiredChannels( mcHit, hitchannelVec)){
        if ( msgLevel( MSG::DEBUG) ) info() << "Could not calculate list of fired channels" << endmsg;
        HitChannelID_Start.push_back( hitchannelVec[0].first.channelID() );
        HitLayer_Start.push_back( hitchannelVec[0].first.layer() );
        HitQuarter_Start.push_back( hitchannelVec[0].first.quarter() );
        HitModule_Start.push_back( hitchannelVec[0].first.module() );
        HitMat_Start.push_back( hitchannelVec[0].first.mat() );
        HitSiPMId_Start.push_back( hitchannelVec[0].first.sipmId() );
        HitSiPMCell_Start.push_back( hitchannelVec[0].first.sipmCell() );
        HitFraction_Start.push_back( hitchannelVec[0].second );

        HitChannelID_End.push_back( hitchannelVec[hitchannelVec.size() - 1].first.channelID() );
        HitLayer_End.push_back( hitchannelVec[hitchannelVec.size() - 1].first.layer() );
        HitQuarter_End.push_back( hitchannelVec[hitchannelVec.size() - 1].first.quarter() );
        HitModule_End.push_back( hitchannelVec[hitchannelVec.size() - 1].first.module() );
        HitMat_End.push_back( hitchannelVec[hitchannelVec.size() - 1].first.mat() );
        HitSiPMId_End.push_back( hitchannelVec[hitchannelVec.size() - 1].first.sipmId() );
        HitSiPMCell_End.push_back( hitchannelVec[hitchannelVec.size() - 1].first.sipmCell() );
        HitFraction_End.push_back( hitchannelVec[hitchannelVec.size() - 1].second );
        HitSize.push_back( hitchannelVec.size() );
        for(std::vector <std::pair<LHCb::FTChannelID, double> >::const_iterator hiter = hitchannelVec.begin(); 
            hiter !=hitchannelVec.end() ; ++hiter)


        {
          HitChannelID_List.push_back( hiter->first.channelID() );
          HitLayer_List.push_back( hiter->first.layer() );
          HitQuarter_List.push_back( hiter->first.quarter() );
          HitModule_List.push_back( hiter->first.module() );
          HitMat_List.push_back( hiter->first.mat() );
          HitSiPMId_List.push_back( hiter->first.sipmId() );
          HitSiPMCell_List.push_back( hiter->first.sipmCell() );
          HitFraction_List.push_back( hiter->second );         

        }
        

      }else
      {
        HitChannelID_Start.push_back( -1000 );
        HitLayer_Start.push_back( -1000 );
        HitQuarter_Start.push_back( -1000 );
        HitModule_Start.push_back( -1000 );
        HitMat_Start.push_back( -1000 );
        HitSiPMId_Start.push_back( -1000 );
        HitSiPMCell_Start.push_back( -1000 );
        HitFraction_Start.push_back( -1000 );
        HitChannelID_End.push_back(-1000); 
        HitLayer_End.push_back( -1000 );
        HitQuarter_End.push_back( -1000 );
        HitModule_End.push_back( -1000 );
        HitMat_End.push_back( -1000 );
        HitSiPMId_End.push_back( -1000 );
        HitSiPMCell_End.push_back( -1000 );
        HitFraction_End.push_back( -1000 );
        HitSize.push_back( 0 );
        HitChannelID_List.push_back( 0 );
        HitLayer_List.push_back( 0 );
        HitQuarter_List.push_back( 0 );
        HitModule_List.push_back( 0 );
        HitMat_List.push_back( 0 );
        HitSiPMId_List.push_back( 0 );
        HitSiPMCell_List.push_back( 0 );
        HitFraction_List.push_back( 0 );  
      }
      
    }
    
    if ( msgLevel(MSG::DEBUG) ) {
      // debug() <<"______________________________________________________________________________"
      //         << "\nChannelId=" << mcHit->channelID() << "\n\t [ L:" << mcHit->channelID().layer() 
      //      << " Q:" << mcHit->channelID().quarter() << " M:" << mcHit->channelID().module()
      //      << " m:" << mcHit->channelID().mat()  << " S:" << mcHit->channelID().sipmId()
      //      << " C:" << mcHit->channelID().sipmCell() << " ] Charge=" << mcHit->adcCount()
      //      << " nbHit=" << nbHit
      //      << endmsg;
    }
    

  }
  tuple->farray("Hit_X",HitX,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Y",HitY,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Z",HitZ,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Energy",HitEnergy,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_PID",HitPID,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_PartPx",HitPartPx,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_PartPy",HitPartPy,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_PartPz",HitPartPz,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_PartE",HitPartE,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_ChannelID" ,HitChannelID,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Layer" ,HitLayer,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Quarter" ,HitQuarter,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Module" ,HitModule,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Mat" ,HitMat,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_SiPMId" ,HitSiPMId,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_SiPMCell" ,HitSiPMCell,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Fraction" ,HitFraction,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Size" ,HitSize,"Nhits",m_ntupHitMaxSize );

  tuple->farray("Hit_ChannelID_Start" ,HitChannelID_Start,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Layer_Start" ,HitLayer_Start,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Quarter_Start" ,HitQuarter_Start,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Module_Start" ,HitModule_Start,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Mat_Start" ,HitMat_Start,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_SiPMId_Start" ,HitSiPMId_Start,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_SiPMCell_Start" ,HitSiPMCell_Start,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Fraction_Start" ,HitFraction_Start,"Nhits",m_ntupHitMaxSize );

  tuple->farray("Hit_ChannelID_End" ,HitChannelID_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Layer_End" ,HitLayer_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Quarter_End" ,HitQuarter_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Module_End" ,HitModule_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Mat_End" ,HitMat_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_SiPMId_End" ,HitSiPMId_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_SiPMCell_End" ,HitSiPMCell_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Fraction_End" ,HitFraction_End,"Nhits",m_ntupHitMaxSize );

  tuple->farray("Hit_ChannelID_End" ,HitChannelID_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Layer_End" ,HitLayer_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Quarter_End" ,HitQuarter_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Module_End" ,HitModule_End,"Nhits",100000 );
  tuple->farray("Hit_Mat_End" ,HitMat_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_SiPMId_End" ,HitSiPMId_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_SiPMCell_End" ,HitSiPMCell_End,"Nhits",m_ntupHitMaxSize );
  tuple->farray("Hit_Fraction_End" ,HitFraction_End,"Nhits",m_ntupHitMaxSize );


  // retrieve FTDigits
  const MCFTDigits* mcDigitsCont = get<MCFTDigits>(m_digitLocation);
  if ( msgLevel(MSG::DEBUG) ) 
    debug() <<"mcDigitsCont->size() : " << mcDigitsCont->size()<< endmsg;

  // Loop over MCFTDigits
  if ( msgLevel(MSG::DEBUG) ) debug() << "######   EvtNb=" << evt->evtNumber() << " RunBb=" << evt->runNumber() << endmsg;
  int nbDigit  = 0;
  
  std::vector<uint> DigiChannelID;
  std::vector<uint> DigiLayer;
  std::vector<uint> DigiQuarter;
  std::vector<uint> DigiModule;
  std::vector<uint>  DigiMat;
  std::vector<uint>  DigiSiPMId;
  std::vector<uint>  DigiSiPMCell;
  std::vector<double>  DigiCharge;

  for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
    ++nbDigit;
    MCFTDigit* mcDigit = *iterDigit; 
    if ( msgLevel(MSG::DEBUG) ) {
      debug() <<"______________________________________________________________________________"
              << "\nChannelId=" << mcDigit->channelID() << "\n\t [ L:" << mcDigit->channelID().layer() 
           << " Q:" << mcDigit->channelID().quarter() << " M:" << mcDigit->channelID().module()
           << " m:" << mcDigit->channelID().mat()  << " S:" << mcDigit->channelID().sipmId()
           << " C:" << mcDigit->channelID().sipmCell() << " ] Charge=" << mcDigit->adcCount()
           << " nbDigit=" << nbDigit
           << endmsg;
    }
    
    DigiChannelID.push_back( mcDigit->channelID() );
    DigiLayer.push_back( mcDigit->channelID().layer() );
    DigiQuarter.push_back( mcDigit->channelID().quarter() );
    DigiModule.push_back( mcDigit->channelID().module() );
    DigiMat.push_back( mcDigit->channelID().mat() );
    DigiSiPMId.push_back( mcDigit->channelID().sipmId() );
    DigiSiPMCell.push_back( mcDigit->channelID().sipmCell() );
    DigiCharge.push_back( mcDigit->adcCount() );
  }

  tuple->farray("Digit_ChannelID" ,DigiChannelID,"Ndigits",m_ntupDigitMaxSize );
  tuple->farray("Digit_Layer" ,DigiLayer,"Ndigits",m_ntupDigitMaxSize );
  tuple->farray("Digit_Quarter" ,DigiQuarter,"Ndigits",m_ntupDigitMaxSize );
  tuple->farray("Digit_Module" ,DigiModule,"Ndigits",m_ntupDigitMaxSize );
  tuple->farray("Digit_Mat" ,DigiMat,"Ndigits",m_ntupDigitMaxSize );
  tuple->farray("Digit_SiPMId" ,DigiSiPMId,"Ndigits",m_ntupDigitMaxSize );
  tuple->farray("Digit_SiPMCell" ,DigiSiPMCell,"Ndigits",m_ntupDigitMaxSize );
  tuple->farray("Digit_Charge" ,DigiCharge,"Ndigits",m_ntupDigitMaxSize );
  

  // Store Cluster information with only largest hit info
  //ClusterWithLargestMCHitInfo(tuple);
  // or store Cluster information with all hits info
  ClusterWithAllMCHitInfo(tuple);


  tuple->write();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTNtupleMaker::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;


  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
//  Fill Cluster data with largest MC Hit information
//=============================================================================
StatusCode FTNtupleMaker::ClusterWithLargestMCHitInfo(Tuple &tpl)
{
 // retrieve FTCluster
  const FTClusters* clustersCont = get<FTClusters>(m_clusterLocation);
  if ( msgLevel(MSG::DEBUG) )
    debug() <<"clustersCont->size() : " << clustersCont->size()<< endmsg;

  // retrieve FTClustertoMCHitLink
  LinkedTo<LHCb::MCHit, LHCb::FTCluster> myClusterToHitLink ( evtSvc(), msgSvc(), 
                                                              std::string(LHCb::FTClusterLocation::Default)+"2MCHits" );

    // Loop over FTCluster
 int nbCluster  = 0;
  std::vector<uint> ClusterChannelID;
  std::vector<uint> ClusterLayer;
  std::vector<uint> ClusterQuarter;
  std::vector<uint> ClusterModule;
  std::vector<uint>  ClusterMat;
  std::vector<uint>  ClusterSiPMId;
  std::vector<uint>  ClusterSiPMCell;
  std::vector<double> ClusterFraction;
  std::vector<double> ClusterCharge;
  std::vector<int> ClusterSize;
  std::vector<int> ClusterHitNb;


 // Look at larger mcHit from clusters
  std::vector<double> HitclusX;
  std::vector<double> HitclusY;
  std::vector<double> HitclusZ;
  std::vector<double> HitclusEnergy;
  std::vector<double> HitclusPID;
  std::vector<double> HitclusPartPx;
  std::vector<double> HitclusPartPy;
  std::vector<double> HitclusPartPz;
  std::vector<double> HitclusPartE;
  std::vector<uint> HitclusChannelID;
  std::vector<uint> HitclusLayer;
  std::vector<uint> HitclusQuarter;
  std::vector<uint> HitclusModule;
  std::vector<uint> HitclusMat;
  std::vector<uint> HitclusSiPMId;
  std::vector<uint> HitclusSiPMCell;
  std::vector<double> HitclusFraction;

  for (FTClusters::const_iterator iterCluster = clustersCont->begin(); iterCluster!=clustersCont->end();++iterCluster){
    ++nbCluster;

    FTCluster* cluster = *iterCluster;
    ClusterChannelID.push_back( cluster->channelID() );
    ClusterLayer.push_back( cluster->channelID().layer() );
    ClusterQuarter.push_back( cluster->channelID().quarter() );
    ClusterModule.push_back( cluster->channelID().module() );
    ClusterMat.push_back( cluster->channelID().mat() );
    ClusterSiPMId.push_back( cluster->channelID().sipmId() );
    ClusterSiPMCell.push_back( cluster->channelID().sipmCell() );
    ClusterFraction.push_back( cluster->fraction() );
    ClusterCharge.push_back( cluster->charge() );
    ClusterSize.push_back( cluster->size() );
    ClusterHitNb.push_back( myClusterToHitLink.range(cluster).size() );

    if ( msgLevel(MSG::DEBUG) ) {
      debug()<<"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "
              << "\nChannelId=" << cluster->channelID() << "\n\t [ L:" << cluster->channelID().layer() 
              << " Q:" << cluster->channelID().quarter() << " M:" << cluster->channelID().module()
              << " m:" << cluster->channelID().mat()  << " S:" << cluster->channelID().sipmId()
              << " C:" << cluster->channelID().sipmCell() << " ]\n Fraction=" << cluster->fraction()
              << " Charge=" << cluster->charge() << " Size=" << cluster->size()
              << " nbCluster=" << nbCluster
              << endmsg;
      debug() << "###   myClusterToHitLink.size()="<< myClusterToHitLink.range(cluster).size() <<endmsg;
    }



    const  LHCb::MCHit* pHit = myClusterToHitLink.first( cluster );

  // Look at largest mcHit from clusters

    HitclusX.push_back( pHit->midPoint().x());
    HitclusY.push_back(pHit->midPoint().y()) ;
    HitclusZ.push_back(pHit->midPoint().z()) ;
    HitclusEnergy.push_back( pHit->energy());
    HitclusPID.push_back(pHit->mcParticle()->particleID().pid()) ;
    HitclusPartPx.push_back( pHit->mcParticle()->momentum().x());
    HitclusPartPy.push_back( pHit->mcParticle()->momentum().y());
    HitclusPartPz.push_back( pHit->mcParticle()->momentum().z());
    HitclusPartE.push_back(pHit->mcParticle()->momentum().e()) ;

    const DeFTFibreMat* pL = m_deFT->findFibreMat(pHit->midPoint());
    std::pair<LHCb::FTChannelID, double>  hitchannel;
    if( ( pL ) && ( pL->calculateMeanChannel( pHit, hitchannel)) ) {
      
      
        HitclusChannelID.push_back(hitchannel.first) ;
        HitclusLayer.push_back((hitchannel.first).layer()) ;
        HitclusQuarter.push_back((hitchannel.first).quarter()) ;
        HitclusModule.push_back((hitchannel.first).module()) ;
        HitclusMat.push_back((hitchannel.first).mat()) ;
        HitclusSiPMId.push_back((hitchannel.first).sipmId()) ;
        HitclusSiPMCell.push_back((hitchannel.first).sipmCell()) ;
        HitclusFraction.push_back(( hitchannel.second ));
    }else
    {
      HitclusChannelID.push_back(100000000) ;
      HitclusLayer.push_back(100000000) ;
      HitclusQuarter.push_back(100000000) ;
      HitclusModule.push_back(100000000) ;
      HitclusMat.push_back(100000000) ;
      HitclusSiPMId.push_back(100000000) ;
      HitclusSiPMCell.push_back(100000000) ;
      HitclusFraction.push_back( 100000000. );
    }
  }

  tpl->farray("Cluster_ChannelID" ,ClusterChannelID,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Layer" ,ClusterLayer,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Quarter" ,ClusterQuarter,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Module" ,ClusterModule,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Mat" ,ClusterMat,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_SiPMId" ,ClusterSiPMId,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_SiPMCell" ,ClusterSiPMCell,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Fraction" ,ClusterFraction,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Charge" ,ClusterCharge,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Size" ,ClusterSize,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_HitNb" ,ClusterHitNb,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_X",HitclusX,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Y",HitclusY,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Z",HitclusZ,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Energy",HitclusEnergy,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PID",HitclusPID,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PartPx",HitclusPartPx,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PartPy",HitclusPartPy,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PartPz",HitclusPartPz,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PartE",HitclusPartE,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_ChannelID",HitclusChannelID,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Layer",HitclusLayer,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Quarter",HitclusQuarter,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Module",HitclusModule,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Mat",HitclusMat,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_SiPMId",HitclusSiPMId,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_SiPMCell",HitclusSiPMCell,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Fraction",HitclusFraction,"Nclusters",m_ntupClusterMaxSize );

            
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Fill Cluster data with all MC Hit information
//=============================================================================
StatusCode FTNtupleMaker::ClusterWithAllMCHitInfo(Tuple &tpl){
 // retrieve FTCluster
  const FTClusters* clustersCont = get<FTClusters>(m_clusterLocation);
  if ( msgLevel(MSG::DEBUG) )
    debug() <<"clustersCont->size() : " << clustersCont->size()<< endmsg;

  // retrieve FTClustertoMCHitLink
  LinkedTo<LHCb::MCHit, LHCb::FTCluster> myClusterToHitLink ( evtSvc(), msgSvc(), 
                                                              std::string(LHCb::FTClusterLocation::Default)+"2MCHits" );

    // Loop over FTCluster
 int nbCluster  = 0;
  std::vector<uint> ClusterChannelID;
  std::vector<uint> ClusterLayer;
  std::vector<uint> ClusterQuarter;
  std::vector<uint> ClusterModule;
  std::vector<uint>  ClusterMat;
  std::vector<uint>  ClusterSiPMId;
  std::vector<uint>  ClusterSiPMCell;
  std::vector<double> ClusterFraction;
  std::vector<double> ClusterCharge;
  std::vector<int> ClusterSize;
  std::vector<int> ClusterHitNb;


 // Loop over all mcHit from clusters
  std::vector<double> HitclusX;
  std::vector<double> HitclusY;
  std::vector<double> HitclusZ;
  std::vector<double> HitclusEnergy;
  std::vector<double> HitclusPID;
  std::vector<double> HitclusPartPx;
  std::vector<double> HitclusPartPy;
  std::vector<double> HitclusPartPz;
  std::vector<double> HitclusPartE;
  std::vector<uint> HitclusChannelID;
  std::vector<uint> HitclusLayer;
  std::vector<uint> HitclusQuarter;
  std::vector<uint> HitclusModule;
  std::vector<uint> HitclusMat;
  std::vector<uint> HitclusSiPMId;
  std::vector<uint> HitclusSiPMCell;
  std::vector<double> HitclusFraction;

  for (FTClusters::const_iterator iterCluster = clustersCont->begin(); iterCluster!=clustersCont->end();++iterCluster){
    ++nbCluster;

    FTCluster* cluster = *iterCluster;
    ClusterChannelID.push_back( cluster->channelID() );
    ClusterLayer.push_back( cluster->channelID().layer() );
    ClusterQuarter.push_back( cluster->channelID().quarter() );
    ClusterModule.push_back( cluster->channelID().module() );
    ClusterMat.push_back( cluster->channelID().mat() );
    ClusterSiPMId.push_back( cluster->channelID().sipmId() );
    ClusterSiPMCell.push_back( cluster->channelID().sipmCell() );
    ClusterFraction.push_back( cluster->fraction() );
    ClusterCharge.push_back( cluster->charge() );
    ClusterSize.push_back( cluster->size() );
    ClusterHitNb.push_back( myClusterToHitLink.range(cluster).size() );

    if ( msgLevel(MSG::DEBUG) ) {
      debug()<<"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "
              << "\nChannelId=" << cluster->channelID() << "\n\t [ L:" << cluster->channelID().layer() 
              << " Q:" << cluster->channelID().quarter() << " M:" << cluster->channelID().module()
              << " m:" << cluster->channelID().mat()  << " S:" << cluster->channelID().sipmId()
              << " C:" << cluster->channelID().sipmCell() << " ]\n Fraction=" << cluster->fraction()
              << " Charge=" << cluster->charge() << " Size=" << cluster->size()
              << " nbCluster=" << nbCluster
              << endmsg;
      debug() << "###   myClusterToHitLink.size()="<< myClusterToHitLink.range(cluster).size() <<endmsg;
    }


    int HitCounter = 0;
    int nbOfHits = myClusterToHitLink.range(cluster).size();
    
    const  LHCb::MCHit* pHit = myClusterToHitLink.first( cluster );

  // Loop over all mcHit from clusters
    if(0 != nbOfHits )
    {
    while(pHit != NULL)
    {
      
      HitclusX.push_back(pHit->midPoint().x());
      HitclusY.push_back(pHit->midPoint().y()) ;
      HitclusZ.push_back(pHit->midPoint().z()) ;
    HitclusEnergy.push_back( pHit->energy());
    HitclusPID.push_back(pHit->mcParticle()->particleID().pid()) ;
    HitclusPartPx.push_back( pHit->mcParticle()->momentum().x());
    HitclusPartPy.push_back( pHit->mcParticle()->momentum().y());
    HitclusPartPz.push_back( pHit->mcParticle()->momentum().z());
    HitclusPartE.push_back(pHit->mcParticle()->momentum().e()) ;

    const DeFTFibreMat* pL = m_deFT->findFibreMat(pHit->midPoint());
    std::pair<LHCb::FTChannelID, double>  hitchannel;
    if( ( pL ) && ( pL->calculateMeanChannel( pHit, hitchannel)) ) {
        HitclusChannelID.push_back(hitchannel.first) ;
        HitclusLayer.push_back((hitchannel.first).layer()) ;
        HitclusQuarter.push_back((hitchannel.first).quarter()) ;
        HitclusModule.push_back((hitchannel.first).module()) ;
        HitclusMat.push_back((hitchannel.first).mat()) ;
        HitclusSiPMId.push_back((hitchannel.first).sipmId()) ;
        HitclusSiPMCell.push_back((hitchannel.first).sipmCell()) ;
        HitclusFraction.push_back(( hitchannel.second ));
    }else
    {
      HitclusChannelID.push_back(100000000) ;
      HitclusLayer.push_back(100000000) ;
      HitclusQuarter.push_back(100000000) ;
      HitclusModule.push_back(100000000) ;
      HitclusMat.push_back(100000000) ;
      HitclusSiPMId.push_back(100000000) ;
      HitclusSiPMCell.push_back(100000000) ;
      HitclusFraction.push_back( 100000000. );
    }
    ++HitCounter;
      
    pHit = myClusterToHitLink.next();
    }
    }
  }

  tpl->farray("Cluster_ChannelID" ,ClusterChannelID,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Layer" ,ClusterLayer,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Quarter" ,ClusterQuarter,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Module" ,ClusterModule,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Mat" ,ClusterMat,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_SiPMId" ,ClusterSiPMId,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_SiPMCell" ,ClusterSiPMCell,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Fraction" ,ClusterFraction,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Charge" ,ClusterCharge,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_Size" ,ClusterSize,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Cluster_HitNb" ,ClusterHitNb,"Nclusters",m_ntupClusterMaxSize );
  tpl->farray("Hitclus_X" ,HitclusX,"Nhitclus", 100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Y" ,HitclusY,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Z" ,HitclusZ,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Energy" ,HitclusEnergy,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PID" ,HitclusPID,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PartPx" ,HitclusPartPx,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PartPy" ,HitclusPartPy,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PartPz" ,HitclusPartPz,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_PartE" ,HitclusPartE,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_ChannelID" ,HitclusChannelID,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Layer" ,HitclusLayer,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Quarter" ,HitclusQuarter,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Module" ,HitclusModule,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Mat" ,HitclusMat,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_SiPMId" ,HitclusSiPMId,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_SiPMCell" ,HitclusSiPMCell,"Nhitclus",100*m_ntupClusterMaxSize );
  tpl->farray("Hitclus_Fraction" ,HitclusFraction,"Nhitclus",100*m_ntupClusterMaxSize );

  return StatusCode::SUCCESS;
}
