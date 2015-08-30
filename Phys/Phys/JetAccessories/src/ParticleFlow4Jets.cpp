// $Id: $
// Include files
#include <boost/foreach.hpp>
#include "boost/algorithm/string/replace.hpp"

// from LHCb
#include "Event/ProtoParticle.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloPosition.h"
#include "Event/CaloHypo.h"
#include "CaloDet/DeCalorimeter.h"
#include "Kernel/CaloCellID.h"
#include "Event/Track.h"
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Kernel/TransporterFunctions.h"
#include "CaloDet/DeCalorimeter.h"

// Relations
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted2D.h"
#include "Kernel/Particle2Vertex.h"
#include "Relations/Get.h"
// local
#include "ParticleFlow4Jets.h"


//-----------------------------------------------------------------------------
// Implementation file for class : ParticleFlow4Jets
//
// 2011-05-10 : Victor Coco and Cedric Potterat
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticleFlow4Jets )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleFlow4Jets::ParticleFlow4Jets( const std::string& name,
                                      ISvcLocator* pSvcLocator)
: GaudiTupleAlg ( name , pSvcLocator )
  , m_trSel ( 0 )
  , m_ttExpectation ( 0 )
  , m_clust2TrLocationECAL ( LHCb::CaloAlgUtils::CaloIdLocation("ClusterMatch", context()) )
  , m_clust2TrLocationHCAL ( "Rec/Calo/HcalClusterMatch" )
  , m_p2s ( 0 )
  , m_filterGhost ( 0 )
  , m_protoPF ( "Rec/ProtoP/PF" )
  , m_calohypoPF ( "Rec/Calo/Hadrons" )
{
  // Default options are equivalent to StdNoPIDsPions+StdNoPIDsDownPions+StdLoosePhotonAll
  declareProperty( "PFOutputLocation"          ,  m_PFOutputLocation  =  "Phys/PFParticles/Particles"
                   ,  "Output Location of the particles 4 jets") ;

  declareProperty( "PFBannedOutputLocation"          ,  m_PFBannedOutputLocation  =  "Phys/PFBannedParticles/Particles"
                   ,  "Output Location of the Banned particles 4 jets") ;

  declareProperty( "PFHiddenNeutralOutputLocation"          ,  m_PFHiddenNeutralOutputLocation  =  "Phys/PFNeutralParticles/Particles"
                   ,  "Output Location of the Neutral particles form cluster under tracks 4 jets") ;


  declareProperty( "TrackSelectorType", m_trSelType = ""
                   , "TrackSelector for choice of track inputs");
  declareProperty( "PFProtoParticlesOutputLocation", m_protoPF
                   , "Location for the protoparticles created in PF");
  declareProperty( "PFCaloHypoOutputLocation", m_calohypoPF
                   , "Location for the calohypo created in PF");

  // Track related cuts
  declareProperty( "UsePIDInfo" , m_usePIDInfo  = true ,
                   "Use PID information to apply mass hypothwesis to tracks");
  declareProperty( "UseTTHits" , m_useTTHits  = false ,
                   "Use TT info to kill long ghost");
  declareProperty( "UseNNGhost" , m_useNNGhost  = false ,
                   "Use ghost PID NN");
  declareProperty( "MinInfMomentumCut" , m_cutInfMomTRVal  = -1. ,
                   "Minimal cut on sigma_(q/p)/(q/p)");
  declareProperty( "MaxChi2NoTT" , m_noTTChi2PerDof = 10. ,
                   "Maximum chi2 per DoF for long track with no TTHits");

  declareProperty( "ParticleLocations" ,m_particleLocations ,
                   "Location of premade particles to be used" );

  // Calo matching related cuts
  declareProperty( "MaxMatchECALTr" , m_Chi2ECALCut = 25. ,
                   "Maximal ecal<-->track matching chi2");
  declareProperty( "MaxMatchECALTr_T" , m_Chi2ECALCutTT = 25. ,
                   "Maximal ecal<-->TtrackT matching chi2");
  declareProperty( "MaxMatchHCALTr_T" , m_Chi2HCALCutTT = 25. ,
                   "Maximal hcal<-->Ttrack matching chi2");


  declareProperty( "UseHCAL" , m_useHCAL  = true ,
                   "Use HCAL Clusters");


  declareProperty( "MaxMatchHCALLowEValue" , m_Chi2HCAL0CutEValue = 1000.
                   , "Low energy when matching HCAL to tracks");
  declareProperty( "MaxMatchHCALHighEValue" , m_Chi2HCAL1CutEValue = 5000.
                   , "High energy when matching HCAL to tracks");

  declareProperty( "MaxMatchHCALTrSmallE" , m_Chi2HCAL0Cut = 10. ,
                   "Maximal hcal P < MaxMatchHCALLowEValue <-->track matching chi2");
  declareProperty( "MaxMatchHCALTrMediumE" , m_Chi2HCAL1Cut = 16. ,
                   "Maximal hcal MaxMatchHCALLowEValue < P < MaxMatchHCALHighEValue <-->track matching chi2");
  declareProperty( "MaxMatchHCALTrLargeE" , m_Chi2HCAL2Cut = 25. ,
                   "Maximal hcal P > MaxMatchHCALHighEValue <-->track matching chi2");



  // Neutral related cuts
  declareProperty( "MinPhotonID4Photon" , m_photonID4Photon = -1. ,
                   "Minimum photon pid for photon hypothesis");
  declareProperty( "MinPhotonID4PhotonTtrack" , m_photonID4PhotonTban = -2. ,
                   "Minimum photon pid for photon hypothesis of T matched cluster");
  declareProperty( "MinPhotonIDMax4ResolvedPi0" , m_photonIDMax4ResolvedPi0 = -4. ,
                   "Minimum photon pid for photon from resolved Pi0");
  declareProperty( "MinPhotonIDMin4ResolvedPi0" , m_photonIDMin4ResolvedPi0 = -2. ,
                   "Minimum photon pid for photon from resolved Pi0");
  declareProperty( "CatchBremFromElectrons" , m_catchBremFromElectrons = false ,
                   "Flag bremstrahlung phtons from electrons");

  declareProperty( "CandidateToBanLocation", m_banCandidatesLocation = "",
                   "Location of candidate not to be included for Jet reco" );
  declareProperty( "BanCloneFromCandidateAndV0", m_alsoBanClone = true ,
                   "Clones of V0 and Candidate to ban are also rejected" );

  declareProperty( "MinHCALE",m_minHCALE = 2000.,
                   "Minimum value to take a HCAL cluster into account");
  declareProperty( "UseTTrackBanning", m_banFromTTrack = true, "Use TTrack to ban neutrals");
  declareProperty( "VerticesLocation", m_verticesLocation = LHCb::RecVertexLocation::Primary
                   , "Location of vertices to which charged paritcles should be related");
  declareProperty( "CalibECAL_EovP", m_alphaECAL = 0.1
                   , "Fraction of the EovP in ECAL");
  declareProperty( "CalibHCAL_EovP", m_betaHCAL = 0.9
                   , "Fraction of the EovP in HCAL");
  declareProperty( "NSigmaForCaloRecovery", m_nSigmaE = 3
                   , "From how many sigma E we consider it a new particle");
  declareProperty( "NeutralRecovery", m_neutralRecovery = false
                   , "Turn on the neutral recovery");
  declareProperty( "MinECALE_NeutralRecovery", m_minECALE_NR = 0.
                   , "Minimal energy in ECAL to consider neutral recovery");
  declareProperty( "MinHCALE_NeutralRecovery", m_minHCALE_NR = 0.
                   , "Minimal energy in HCAL to consider neutral recovery");
}
//=============================================================================
// Destructor
//=============================================================================
ParticleFlow4Jets::~ParticleFlow4Jets() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ParticleFlow4Jets::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // get an instance of the track selector
  if( m_trSelType != "")
    m_trSel = tool<ITrackSelector>( m_trSelType, "TrackSelector", this );

  // get the TT Hit expectation tool
  if(m_ttExpectation == 0)
    m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");

  m_pf2verticesLocation = m_PFOutputLocation ;
  m_pfbanned2verticesLocation = m_PFBannedOutputLocation ;
  boost::replace_last(m_pf2verticesLocation,"/Particles","/Particle2VertexRelations");
  boost::replace_last(m_pfbanned2verticesLocation,"/Particles","/Particle2VertexRelations");

  m_pvRelator = tool<IRelatedPVFinder>("GenericParticle2PVRelator<_p2PVWithIPChi2, OfflineDistanceCalculatorName>/P2PVWithIPChi2", this);

  // ParticleProperty
  LHCb::IParticlePropertySvc* ppSvc = 0;
  sc = service("LHCb::ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    fatal() << "    Unable to locate Particle Property Service"   << endmsg;
    return sc;
  }

  if (m_particleLocations.empty()){
    m_particleLocations.push_back("Phys/StdLooseAllPhotons/Particles");
  }

  m_p2s = tool<IParticle2State>("Particle2State");


  m_protoMap[ "pi+" ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "pion"  , this )
                                        , ppSvc->find( "pi+" ) );
  m_protoMap[ "K+"  ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "kaon"  , this )
                                        , ppSvc->find( "K+" ) );
  m_protoMap[ "p+"  ] =  std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "proton"  , this )
                                         , ppSvc->find( "p+" ) );
  m_protoMap[ "e+"  ] =  std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "electron", this )
                                         , ppSvc->find( "e+" ) );
  m_protoMap[ "mu+" ] =  std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleMUONFilter"  , "muon"    , this )
                                         , ppSvc->find( "mu+" ) );

  m_filterGhost = tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "ghost", this );


  m_ecal = getDet<DeCalorimeter> ( "/dd/Structure/LHCb/DownstreamRegion/Ecal" );
  m_hcal = getDet<DeCalorimeter> ( "/dd/Structure/LHCb/DownstreamRegion/Hcal" );

  return sc;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleFlow4Jets::execute(
) {
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"enter particle flow"<<endmsg;
  using namespace LHCb::Calo2Track;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Entering execute"<<endmsg;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Few definitions ~~~~~~~~~~~~~~~~~~~~~~
  typedef std::map<int, std::pair< int , int > > BannedIDMap;

  typedef std::map< int , std::vector<double> > ClusterID2DoubleMap;
  typedef std::map< int , double > ClusterID2EMap;
  typedef std::map< int , std::vector<int>    > ClusterID2IntMap;
  typedef std::map< int , int > ClusterID2SatMap;

  m_trackKeyToBan.clear();

  m_particleContainers.clear();

  m_electronPPkeys.clear();

  // Load tables, containers, etc...
  this->loadDatas();

  // Create the output container
  LHCb::Particles* PFParticles = new LHCb::Particles();
  put( PFParticles , m_PFOutputLocation );
  LHCb::Particles* BannedPFParticles = new LHCb::Particles();
  put( BannedPFParticles , m_PFBannedOutputLocation );

  // Some staff for newly created particles
  LHCb::ProtoParticles* PFProtoParticles = new LHCb::ProtoParticles();
  put( PFProtoParticles , m_protoPF );
  LHCb::CaloHypos* PFCaloHypos = new LHCb::CaloHypos();
  put( PFCaloHypos , m_calohypoPF );

  // Create the particle to vertex relation table
  Particle2Vertex::WTable* table = new Particle2Vertex::WTable();
  Particle2Vertex::WTable* tableBanned = new Particle2Vertex::WTable();

  // Map of ECAL clusters to Ban
  BannedIDMap BannedECALClusters;
  // Map of HCAL clusters to Ban
  BannedIDMap BannedHCALClusters;
  // Map of charged particle to banned clusters
  ClusterID2DoubleMap  ECALCluster2MomentaMap;
  ClusterID2IntMap     ECALCluster2IDMap;
  ClusterID2DoubleMap  HCALCluster2MomentaMap;
  ClusterID2IntMap     HCALCluster2IDMap;
  ClusterID2EMap       ECALCluster2CaloMap;
  ClusterID2EMap       HCALCluster2CaloMap;
  ClusterID2SatMap     ECALCluster2NSatMap;
  ClusterID2SatMap     HCALCluster2NSatMap;
  ClusterID2EMap       ECALCluster2CovMap;
  ClusterID2EMap       HCALCluster2CovMap;

  // Eventually store the V0 daughters track keys
  if(m_particleContainers.count("V0") > 0.5){
    LHCb::Particle::ConstVector sortV0s;
    for( unsigned int i = 0 ; i < m_particleContainers["V0"].size(); i++)
      BOOST_FOREACH(const LHCb::Particle* v0, *m_particleContainers["V0"][i] )
        sortV0s.push_back(v0);

    std::sort( sortV0s.begin(), sortV0s.end(),sortChi2() );

    BOOST_FOREACH(const LHCb::Particle* v0, sortV0s){
      LHCb::Particle::ConstVector daughtersV0 = v0->daughtersVector();
      bool dauOverLap = false;

      for(unsigned i = 0; i<daughtersV0.size(); i++)
        if(daughtersV0.at(i) != NULL)
          if(daughtersV0.at(i)->proto() != NULL)
            if (m_trackKeyToBan.count(daughtersV0.at(i)->proto()->track()->key())>0.5){
              dauOverLap = true;
              break;
            }
      if(!dauOverLap){
        LHCb::Particle* PFV0 = v0->clone();
        relate2Vertex( PFV0 , *table );
        PFParticles->insert( PFV0 );
        for(unsigned i = 0; i<daughtersV0.size(); i++)
          if(daughtersV0.at(i) != NULL)
            if(daughtersV0.at(i)->proto() != NULL)
              m_trackKeyToBan[daughtersV0.at(i)->proto()->track()->key()]=daughtersV0.at(i)->proto()->track();
      }
    }
  }


  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Start charged Loop..."<<endmsg;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ The main loop for charged components  ~~~~~~~~~~~~~~~~~~~~~~
  // Loop over charged Protoparticles, reject bad tracks, make best PID hypo particle out of it
  // store the used ECAL cluster and HCAL digits
  if( m_protoParticles.count("Charged") > 0.5){

    BOOST_FOREACH(const LHCb::ProtoParticle* ch_pp, *m_protoParticles["Charged"] ){

      const LHCb::Track* track = ch_pp->track();


      if ( track == NULL ) continue;


      int tag =  Unknown ;

      // Track selector cuts
      if ( m_trSel!= 0 ){
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Track "<<track->type()<<" with pt: "<<track->pt()<<" chi2 per dof "<<track->chi2PerDoF()<<endmsg;
        if ( !m_trSel->accept(*track) ) continue;
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Track accepted"<<endmsg;
      }
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Tag the track"<<endmsg;
      // Extra track check
      tag = tagTrack(track);
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Track tagged as "<<tag<<endmsg;
      LHCb::Particle* theParticle = NULL ;
      // Save the particle
      if (tag == KeepInPF){
        theParticle = MakeParticle( ch_pp , tag , *table );
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Tagged as Keep in PF "<<theParticle<<endmsg;
        PFParticles->insert( theParticle );
      }
      else if (tag ==  KeepInPFBanned ){
        theParticle = MakeParticle( ch_pp , tag , *table );
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Tagged as Keep in Banned PF "<<theParticle<<endmsg;
        BannedPFParticles->insert( theParticle );
      }
      else if (tag == TurnTo0Momentum ){
        theParticle = MakeParticle( ch_pp , tag , *table );
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Tagged as Keep in PF but turn to 0 momentum "<<theParticle->proto()->track()->type()<<endmsg;
        PFParticles->insert( theParticle );
      }
      else if (tag == Unknown ){
        Warning("Unknow status for this charged particle");
      }
      else{
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"This track is banned but its cluster can be kept or not"<<endmsg;
      }
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"theParticle, after insert "<<theParticle<<endmsg;
      if ( tag== RejectDoNotBanCluster ) continue;

      // Get the ECAL clusters and ban those with small chi2
      LHCb::Calo2Track::IClusTrTable2D::InverseType::Range cRange =  m_tableTrECAL->inverse()->relations ( track ) ;
      if (cRange.size()>0){
        double chi2TrClu = cRange.front().weight();
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"ECAL Track match at "<<chi2TrClu<<" with Cluster ID : "<<cRange.front().to()->seed().all()
                                               <<" Is tagged already?: "<<BannedECALClusters.count(cRange.front().to()->seed().all())<<endmsg;
        // If the cluster have distance to track smaller than m_Chi2ECALCut, ban it
        if ( chi2TrClu < m_Chi2ECALCut  ) {
          // if already banned don't store again
          if ( BannedECALClusters.count(cRange.front().to()->seed().all())<0.5){
            if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Will tag as: "<<TrackMatch<<" from the track "<<track->key()<<endmsg;
            std::pair< double , int > tmpPair;
            tmpPair.first = TrackMatch ;
            tmpPair.second =  track->key() ;
            BannedECALClusters[cRange.front().to()->seed().all()] = tmpPair ;
            //Cluster2EnergyMap[cRange.front().to()] = cRange.front().to()->e();
          }
          // In any case, save the link to the particle if it exist
          if ( msgLevel(MSG::VERBOSE) ) verbose()<<"tag1"<<endmsg;
          if (theParticle && m_neutralRecovery){
            if ( msgLevel(MSG::VERBOSE) ) verbose()<<"tag12"<<endmsg;
            if ( msgLevel(MSG::VERBOSE) ) verbose()<<cRange.front().to()->seed().all()<<" "<<ECALCluster2MomentaMap.count(cRange.front().to()->seed().all())<<endmsg;
            ECALCluster2MomentaMap[cRange.front().to()->seed().all()].push_back(theParticle->p());
            ECALCluster2IDMap[cRange.front().to()->seed().all()].push_back(theParticle->particleID ().abspid());
            ECALCluster2CaloMap[cRange.front().to()->seed().all()] = cRange.front().to()->e();
            ECALCluster2NSatMap[cRange.front().to()->seed().all()] = numberOfSaturatedCells( cRange.front().to() , m_ecal );
            ECALCluster2CovMap[cRange.front().to()->seed().all()]= cRange.front().to()->position().covariance ()(2,2)/cRange.front().to()->e();
            if ( msgLevel(MSG::VERBOSE) ) verbose()<<"tag13"<<endmsg;
          }
        }
      }

      if ( msgLevel(MSG::VERBOSE) ) verbose()<<""<<endmsg;
      if(m_useHCAL ){
        // Get the calorimeter clusters associated to the track
        LHCb::Calo2Track::IClusTrTable2D::InverseType::Range cRange2 =  m_tableTrHCAL->inverse()->relations ( track ) ;
        if (cRange2.size()>0){
          double chi2TrClu = cRange2.front().weight();
          if ( msgLevel(MSG::VERBOSE) ) verbose()<<"HCAL Track match at "<<chi2TrClu<<" with track p "<< track->p()<<" with Cluster ID : "
                                                 <<cRange2.front().to()->seed().all()<<" Is tagged already?: "
                                                 <<BannedHCALClusters.count(cRange2.front().to()->seed().all())<<endmsg;
          if ( ((chi2TrClu < m_Chi2HCAL0Cut && cRange2.front().to()->e() <= m_Chi2HCAL0CutEValue) ||
                (chi2TrClu < m_Chi2HCAL1Cut && cRange2.front().to()->e() > m_Chi2HCAL0CutEValue
                 && cRange2.front().to()->e() <=  m_Chi2HCAL1CutEValue) ||
                (chi2TrClu < m_Chi2HCAL2Cut && cRange2.front().to()->e() > m_Chi2HCAL1CutEValue) )){
            if( BannedHCALClusters.count(cRange2.front().to()->seed().all())<0.5 ) {
              if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Will HCAL tag as: "<<TrackMatchHCAL<<" form the track "<<track->key()<<endmsg;
              std::pair< double , int > tmpPair;
              tmpPair.first = TrackMatchHCAL ;
              tmpPair.second =  track->key() ;
              BannedHCALClusters[cRange2.front().to()->seed().all()] = tmpPair ;
              //Cluster2EnergyMap[cRange.front().to()] = cRange.front().to()->e();
            }
            if ( msgLevel(MSG::VERBOSE) ) verbose()<<cRange2.front().to()<<" tag1H"<<endmsg;
            // In any case, save the link to the particle if it exist
            if (theParticle && m_neutralRecovery){
              if ( msgLevel(MSG::VERBOSE) ) verbose()<<"tag12H"<<endmsg;
              if ( msgLevel(MSG::VERBOSE) ) verbose()<<cRange2.front().to()<<" "<<cRange2.front().to()->seed()<<" "<<cRange2.front().to()->seed().all()<<" H "<<HCALCluster2MomentaMap.count(cRange2.front().to()->seed().all())<<endmsg;
              HCALCluster2MomentaMap[cRange2.front().to()->seed().all()].push_back(theParticle->p());
              HCALCluster2IDMap[cRange2.front().to()->seed().all()].push_back(theParticle->particleID ().abspid());
              HCALCluster2CaloMap[cRange2.front().to()->seed().all()] = cRange2.front().to()->e();
              HCALCluster2NSatMap[cRange2.front().to()->seed().all()] = numberOfSaturatedCells( cRange2.front().to() , m_hcal );
              HCALCluster2CovMap[cRange2.front().to()->seed().all()]= cRange2.front().to()->position().covariance ()(2,2)/cRange2.front().to()->e();
              if ( msgLevel(MSG::VERBOSE) ) verbose()<<"tag13H"<<endmsg;
            }
          }
        }
      }


      // Check if belongs to an electron hypothesis...
      if(m_catchBremFromElectrons){
        for ( unsigned int i = 0 ; i < m_electronPPkeys.size() ; i++ ){
          if (m_electronPPkeys[i] != ch_pp->key()) continue;
          const SmartRefVector< LHCb::CaloHypo > &  caloHypos = ch_pp -> calo();
          for (SmartRefVector< LHCb::CaloHypo >::const_iterator i_calo = caloHypos.begin() ; caloHypos.end()!=i_calo ;++i_calo){
            if ( (*i_calo)->hypothesis() != LHCb::CaloHypo::Photon ) {
              if ( (*i_calo)->hypothesis() != LHCb::CaloHypo::EmCharged ) {
                if ( msgLevel(MSG::VERBOSE) ) verbose()<<"One of the electron calo Hypo IS NOT Photon, NOR EmCharged "<< (*i_calo)->hypothesis()<<endmsg;
              }continue;
            }
            const SmartRefVector< LHCb::CaloCluster > & hypoClusters =  (*i_calo)->clusters ();
            if (BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5 )continue;
            std::pair< double , int > tmpPair;
            tmpPair.first = Bremsstrahlung ;
            tmpPair.second =  track->key() ;
            BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;
          }
        }
      }
      if (theParticle == NULL)delete(theParticle);
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Getting back the hidden neutrals ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Before getting the PID and removing the energy"<<endmsg;
  if (m_neutralRecovery){
    LHCb::Particles* NeutralPFParticles = new LHCb::Particles();
    put( NeutralPFParticles , m_PFHiddenNeutralOutputLocation );
    for (ClusterID2DoubleMap::iterator i_cluster = ECALCluster2MomentaMap.begin();  i_cluster !=  ECALCluster2MomentaMap.end(); ++i_cluster ){
      int clusterID = (*i_cluster).first;
      double remainingE = ECALCluster2CaloMap[clusterID] ;
      std::vector< double > momenta =  (*i_cluster).second ;
      int indexPart(0);
      for ( std::vector< double >::iterator i_mom = momenta.begin();
            i_mom != momenta.end() ; ++i_mom ){
        double momentum = *i_mom;
        int absid = ECALCluster2IDMap[clusterID][indexPart];
        if ( absid == 13 ){
          //Eventually remove a mip... nothing for the moment
          if ( msgLevel(MSG::VERBOSE) ) verbose()<<"This is a Muon don't remove anything for the moment"<<endmsg;
        }
        else if ( absid == 11 ){
          // Electron case. Remove p from the ECAL calorimeter
          if ( msgLevel(MSG::VERBOSE) ) verbose()<<"This is an electron"<<endmsg;
          remainingE-=momentum;
        }
        else {
          // Hadron case. Remove a fraction alpha in ECAL and beta in HCAL
          if ( msgLevel(MSG::VERBOSE) ) verbose()<<"This is a hadron"<<endmsg;
          remainingE-=m_alphaECAL*momentum;
        }
        indexPart++;
      }
      if ( remainingE < m_nSigmaE * ECALCluster2CovMap[clusterID] || remainingE < m_minECALE_NR) continue;
      NeutralPFParticles->insert(MakeNeuralParticle(clusterID,0,remainingE,ECALCluster2NSatMap[clusterID],ECALCluster2CaloMap[clusterID],ECALCluster2CovMap[clusterID]));
    }
    for (ClusterID2DoubleMap::iterator i_cluster = HCALCluster2MomentaMap.begin();  i_cluster !=  HCALCluster2MomentaMap.end(); ++i_cluster ){
      int clusterID = (*i_cluster).first;
      double remainingE = HCALCluster2CaloMap[clusterID];
      std::vector< double > momenta =  (*i_cluster).second;
      int indexPart(0);
      for ( std::vector< double >::iterator i_mom = momenta.begin();
            i_mom != momenta.end() ; ++i_mom ){
        double momentum = *i_mom;
        int absid = HCALCluster2IDMap[clusterID][indexPart];
        if( absid != 13 && absid != 11 ){
          remainingE-=m_betaHCAL*momentum;
        }
        indexPart++;
      }
      if ( remainingE < m_nSigmaE * HCALCluster2CovMap[clusterID] || remainingE < m_minHCALE_NR) continue;
      NeutralPFParticles->insert(MakeNeuralParticle(clusterID,1,remainingE,HCALCluster2NSatMap[clusterID],HCALCluster2CaloMap[clusterID],HCALCluster2CovMap[clusterID]));
    }
  }

  /// Here create some infinitely low momentum particles for velo tracks
  if( m_protoParticles.count("Velo") > 0.5){

    BOOST_FOREACH(const LHCb::ProtoParticle* ch_pp, *m_protoParticles["Velo"] ){

      const LHCb::Track* track = ch_pp->track();
      if ( track == NULL ) continue;

      int tag =  Unknown ;
      tag = tagTrack(track);
      // Track selector cuts
      if ( m_trSel!= 0 ){
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Track "<<track->type()<<" with pt: "<<track->pt()<<" chi2 per dof "<<track->chi2PerDoF()<<endmsg;
        if ( !m_trSel->accept(*track) ) continue;
        if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Track accepted"<<endmsg;
      }
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Tag the track"<<endmsg;
      // Extra track check
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Track tagged as "<<tag<<endmsg;
      LHCb::Particle* theParticle = NULL ;
      theParticle = MakeParticle( ch_pp , tag , *table );
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Tagged as Keep in PF but turn to 0 momentum "<<theParticle->proto()->track()->type()<<endmsg;
      PFParticles->insert( theParticle );

      if (theParticle == NULL)delete(theParticle);
    }
  }





  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Finiished the loop to create particles"<<endmsg;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ The loops for electromagnetic neutral components  ~~~~~~~~~~~~~~~~~~~~~~


  // Loop over Merged Pi0, if clusters are not tagged yet, use and tag
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Start merged pi0 Loop..."<<endmsg;
  if (m_particleContainers.count("MergedPi0") > 0.5 ){

    BOOST_FOREACH(const LHCb::Particle* MPi0, *m_particleContainers["MergedPi0"][0] ){

      const LHCb::ProtoParticle* n_pp = MPi0->proto();
      if ( !n_pp ) continue;
      const SmartRefVector< LHCb::CaloHypo > &  caloHypos = n_pp -> calo();
      const SmartRefVector< LHCb::CaloCluster > & hypoClusters =  caloHypos[0]->clusters ();
      if ( !hypoClusters[0] ) continue;

      // If the cluster have already been tagged, skip.
      if (BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5)continue;

      // Use Ttrack banning?
      /*if ( m_banFromTTrack ){
        std::pair< double , int > tmpPair;
        LHCb::Calo2Track::IClusTrTable2D::DirectType::Range tRange = m_tableTrECAL->relations( hypoClusters[0].target() );
        if (tRange.size()>0){
        if (tRange.front().to()->type() == LHCb::Track::Ttrack && tRange.front().weight()< m_Chi2ECALCutTT ){
        tmpPair.first  = TrackMatchTT ;
        tmpPair.second = tRange.front().to() ->key() ;
        BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;
        continue;
        }
        }
        }*/
      std::pair< double , int > tmpPair;
      tmpPair.first = MergedPi0 ;
      tmpPair.second =  MPi0->key() ;
      BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;
      // Add number of saturated ECAL cells
      LHCb::Particle* newP = MPi0->clone();
      newP->addInfo(955,numberOfSaturatedCells(hypoClusters[0].target(),m_ecal));
      // Save the particle
      PFParticles->insert(newP);
    }
  }

  // Loop over Resolved Pi0, if clusters are not tagged yet, use and tag
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Start resolved pi0 Loop..."<<endmsg;
  if (m_particleContainers.count("ResolvedPi0") > 0.5 ){

    BOOST_FOREACH(const LHCb::Particle* RPi0, *m_particleContainers["ResolvedPi0"][0] ){
      LHCb::Particle::ConstVector daugs = RPi0->daughtersVector ();
      double minPhotonID = -1000.;
      double maxPhotonID = -1000.;
      bool skip = false;
      std::vector< int > caloCells;
      int Nsat(0);
      // Loop over daughters
      for  (LHCb::Particle::ConstVector::const_iterator i_daug = daugs. begin()  ; daugs.end() != i_daug ; ++i_daug ){
        const LHCb::ProtoParticle* n_pp = (*i_daug)->proto();
        if (n_pp == NULL)  continue;

        // Get their minimal PhotonID
        double PhotonID = (*i_daug)->proto()->info( LHCb::ProtoParticle::PhotonID,-100.) ;
        if ( PhotonID > maxPhotonID){
          minPhotonID = maxPhotonID; maxPhotonID = PhotonID;
        }
        else if ( PhotonID > minPhotonID ) minPhotonID = PhotonID;
        // ban the corresponding cluster
        const SmartRefVector< LHCb::CaloHypo > &  caloHypos = n_pp -> calo();
        const SmartRefVector< LHCb::CaloCluster > & hypoClusters =  caloHypos[0]->clusters ();
        // Store the cell ID
        if ( !hypoClusters[0] ) continue;

        caloCells.push_back(hypoClusters[0].target()->seed().all());

        // If the cluster have already been tagged, skip.
        if (BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5){
          skip = true;
          continue;
        }
        Nsat +=numberOfSaturatedCells(hypoClusters[0].target(),m_ecal);

        // Use Ttrack banning?
        /*if ( m_banFromTTrack ){
          std::pair< double , int > tmpPair;
          LHCb::Calo2Track::IClusTrTable2D::DirectType::Range tRange = m_tableTrECAL->relations( hypoClusters[0].target() );
          if (tRange.size()>0){
          if (tRange.front().to()->type() == LHCb::Track::Ttrack && tRange.front().weight()< m_Chi2ECALCutTT ){
          tmpPair.first  = TrackMatchTT ;
          tmpPair.second = tRange.front().to() ->key() ;
          BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;
          skip = true;
          continue;
          }
          }
          }*/

      }
      // If both daugther have correct ID and are not used, save the particle and ban
      if ( minPhotonID > m_photonIDMin4ResolvedPi0 && maxPhotonID > m_photonIDMax4ResolvedPi0 && !skip ){
        // Add number of saturated ECAL cells
        LHCb::Particle* newP = RPi0->clone();
        newP->addInfo(955,Nsat);
        // Save the particle
        PFParticles->insert(newP);

        // ban the corresponding clusters
        for (unsigned int iclu = 0 ; iclu < caloCells.size(); iclu++){
          std::pair< double , int > tmpPair;
          tmpPair.first = ResolvedPi0 ;
          tmpPair.second =  RPi0->key() ;
          BannedECALClusters[caloCells[iclu]] = tmpPair ;
        }
      }
    }
  }


  // Loop over Loose Photons, if clusters are not tagged yet, use and tag
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Start Photon Loop..."<<endmsg;
  if (m_particleContainers.count("Photons") > 0.5 ){

    BOOST_FOREACH(const LHCb::Particle* photon, *m_particleContainers["Photons"][0]){
      const LHCb::ProtoParticle* n_pp = photon->proto();
      if (n_pp == NULL)  continue;
      const SmartRefVector< LHCb::CaloHypo > &  caloHypos = n_pp -> calo();
      const SmartRefVector< LHCb::CaloCluster > & hypoClusters =  caloHypos[0]->clusters ();
      // Use Ttrack banning?
      if ( !hypoClusters[0] ) continue;

      bool isTbanned = false;
      if ( m_banFromTTrack && BannedECALClusters.count(hypoClusters[0].target()->seed().all())<0.5){
        std::pair< double , int > tmpPair;
        LHCb::Calo2Track::IClusTrTable2D::DirectType::Range tRange = m_tableTrECAL->relations( hypoClusters[0].target() );
        if (tRange.size()>0){
          if (tRange.front().to()->type() == LHCb::Track::Ttrack && tRange.front().weight()< m_Chi2ECALCutTT ){
            tmpPair.first  = TrackMatchTT ;
            tmpPair.second = tRange.front().to() ->key() ;
            BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;
            isTbanned = true;
          }
        }
      }

      // If the cluster have already been tagged, or does not match the photonID requirements skip.
      if ( BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5
           || n_pp ->info( LHCb::ProtoParticle::PhotonID,-100.)< m_photonID4Photon ||
           ( isTbanned && ( n_pp ->info( LHCb::ProtoParticle::PhotonID,-100.)< m_photonID4PhotonTban
                            || n_pp ->info( LHCb::ProtoParticle::CaloNeutralSpd,-100.) > 1 ) ) ){
        int banType = Photon ;
        if ( BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5 )
          banType = BannedECALClusters[hypoClusters[0].target()->seed().all()].first ;
        // Check if it has been banned because it is charge from track matching (those one we don't want at all)
        LHCb::Particle* newP = photon->clone();
        newP->addInfo(951,banType);
        if (m_tableTrECAL->relations( hypoClusters[0].target() ).size()>0){
          newP->addInfo(952, m_tableTrECAL->relations( hypoClusters[0].target() ).front().weight());
          newP->addInfo(953, m_tableTrECAL->relations( hypoClusters[0].target() ).front().to()->type());
        }
        // Isn't that Photon only???
        if ( banType == Photon ||  banType == TrackMatchTT) BannedPFParticles->insert( newP );
        else delete(newP);
      }else{
        std::pair< double , int > tmpPair;
        tmpPair.first =  Photon ;
        tmpPair.second =  photon->key() ;
        BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;
        // Save the particle
        LHCb::Particle* newP = photon->clone();
        if (m_tableTrECAL->relations( hypoClusters[0].target() ).size()>0){
          newP->addInfo(952, m_tableTrECAL->relations( hypoClusters[0].target() ).front().weight());
          newP->addInfo(953, m_tableTrECAL->relations( hypoClusters[0].target() ).front().to()->type());
        }
        newP->addInfo(955,numberOfSaturatedCells(hypoClusters[0].target(),m_ecal));
        PFParticles->insert( newP );
      }
    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ The loops for hadronic neutral components  ~~~~~~~~~~~~~~~~~~~~~~
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"m_useHCAL "<<m_useHCAL<<endmsg;

  if(m_useHCAL){
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Start HCAL Loop..."<<endmsg;
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"m_clusterContainers.count('Hcal') "<<m_clusterContainers.count("Hcal")<<endmsg;
    if (m_clusterContainers.count("Hcal") > 0.5 ){

      BOOST_FOREACH(const LHCb::CaloCluster* cluster, *m_clusterContainers["Hcal"]){


        if ( 0 == cluster ) { continue ; }
        //cluster energy:
        const double cluen = cluster->e();
        if (cluen<m_minHCALE) continue;

        //if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Clust e "<<cluen<<endmsg;
        //cluster position
        const LHCb::CaloCluster::Position clupos = cluster->position();

        double x =  clupos.x();
        double y =  clupos.y();
        double z =  clupos.z();

        double px =  cluen*x/sqrt(x*x + y*y + z*z);
        double py =  cluen*y/sqrt(x*x + y*y + z*z);
        double pz =  cluen*z/sqrt(x*x + y*y + z*z);

        //Gaudi::LorentzVector pclu = Gaudi::LorentzVector(px,py,pz,cluen);

        LHCb::CaloHypo* hypo = new LHCb::CaloHypo();
        hypo->addToClusters(cluster);
        //hypo->setPosition ( cluster->position() );
        hypo->setHypothesis ( LHCb::CaloHypo::NeutralHadron );


        for ( unsigned ii = 0; ii<cluster->entries().size() ; ++ii )
          hypo->addToDigits(cluster->entries().at(ii).digit());

        PFCaloHypos->insert(hypo);
        LHCb::ProtoParticle* NewProto = new LHCb::ProtoParticle();
        NewProto->addToCalo(hypo);
        NewProto->addInfo(325,cluen);//NeutralEcalE ???
        NewProto->addInfo(332,cluen);//EcalE ???
        //NewProto->addInfo(310,m_tableTrHCAL->relations ( cluster ).front().weight());

        // make a new Particle

        LHCb::Particle p = LHCb::Particle();
        p.setProto(NewProto);
        if (m_tableTrHCAL->relations(cluster ).size()>0){
          p.addInfo(952, m_tableTrHCAL->relations ( cluster ).front().weight());
          p.addInfo(953, m_tableTrHCAL->relations ( cluster ).front().to()->type());
        }

        p.addInfo(955,numberOfSaturatedCells(cluster,m_hcal));
        p.setParticleID( LHCb::ParticleID(-22) );
        p.setReferencePoint( Gaudi::XYZPoint(0.,0.,0.)  );
        p.setMomentum( Gaudi::XYZTVector(px,py,pz,cluen) ) ;


        if ( BannedHCALClusters.count(cluster->seed().all())<0.5 && p.momentum().eta() < 4.2 ){
          if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Cluster not banned: "<<cluster->seed().all()<<" "<<cluen<<" "<<m_tableTrHCAL->relations ( cluster ).front().weight()<<endmsg;
          PFParticles->insert(p.clone());
          PFProtoParticles->insert(NewProto);
        }
        else {
          delete NewProto;
          if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Cluster banned: "<<cluster->seed().all()<<" "<<cluen<<" "<<m_tableTrHCAL->relations ( cluster ).front().weight()<<endmsg;
        }
      }
    }
  }
  put( table, m_pf2verticesLocation );
  put( tableBanned, m_pfbanned2verticesLocation );
  //  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"PFParticles: "<<PFParticles->size()<<" PFParticlesBanned: "<<BannedPFParticles->size()<<" NeoNeutrals: "<<NeutralPFParticles->size()<<endmsg;

  setFilterPassed ( true ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ParticleFlow4Jets::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();
}


//=============================================================================
// Create Particle from ProtoParticle with best PID
//=============================================================================
LHCb::Particle * ParticleFlow4Jets::MakeParticle( const LHCb::ProtoParticle * pp ,
                                                  int banType , Particle2Vertex::WTable& table ){

  bool pid_found(false);

  if (banType == TurnTo0Momentum){
    // make a new Particle
    LHCb::Particle p = LHCb::Particle();
    // Start filling particle with orgininating ProtoParticle
    p.setProto(pp);

    // ParticleID
    const int pID = m_protoMap["pi+"].second ->particleID().pid() * (int)(pp->charge());
    p.setParticleID( LHCb::ParticleID( pID ) );
    // Mass and error from the PID hypothesis
    p.setMeasuredMass(m_protoMap["pi+"].second ->mass());
    p.setMeasuredMassErr(0);

    if ( banType > -0.5 ) p.addInfo(951,banType);

    const LHCb::Track* tk = pp->track() ;

    //Remaining info at the first state...
    StatusCode sc = m_p2s->state2Particle( tk->firstState(), p );

    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"In make particle, original p "<<p<<endmsg;
    LHCb::Particle* PFp = p.clone();
    //Set the momentum to 0...
    Gaudi::LorentzVector newMom(PFp->momentum().Vect().Unit().x()*1e-6,PFp->momentum().Vect().Unit().y()*1e-6,PFp->momentum().Vect().Unit().z()*1e-6,1e-06);

    PFp->setMomentum (newMom);

    relate2Vertex(PFp,table);

    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"In make particle: "<<PFp<<endmsg;
    return PFp;
  }

  const LHCb::ParticleProperty* pprop = NULL ;

  if (m_usePIDInfo){

    // test RICH links
    if ( pp->hasInfo( LHCb::ProtoParticle::RichPIDStatus ) ){
      const LHCb::RichPID * rpid = pp->richPID();
      if ( !rpid ){
        Error( "ProtoParticle has RICH information but NULL RichPID SmartRef !" ).ignore();
      }
    }
    // test MUON links
    if ( pp->hasInfo( LHCb::ProtoParticle::MuonPIDStatus ) ){
      const LHCb::MuonPID * mpid = pp->muonPID();
      if ( !mpid ){
        Error( "ProtoParticle has MUON information but NULL MuonPID SmartRef !" ).ignore();
      }
    }

    //Create a map of PID and ProbNN pairs
    typedef std::map< double , std::string > MapProbNNPID;

    MapProbNNPID  probNNpid ;

    probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNpi   ,-0.5) ] =  "pi+" ;
    probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNp    ,-0.5) ] =  "p+"  ;
    probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNk    ,-0.5) ] =  "K+"  ;
    probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNe    ,-0.5) ] =  "e+"  ;
    probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNmu   ,-0.5) ] =  "mu+" ;

    // the map is by construction sorted by key value (lower first)
    MapProbNNPID::iterator iNN = probNNpid.end();
    iNN--;


    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Ordered hypothesis: type "<<(*iNN).first<<" "<<(*iNN).second<<endmsg;
    // Reverse loop, check if filter is satisfied

    if (m_protoMap[(*iNN).second].first->isSatisfied( pp ))pid_found = true ;

    while( !pid_found && iNN != probNNpid.begin() ){
      iNN--;
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Ordered hypothesis: type "<<(*iNN).first <<" with proba "<<(*iNN).second <<endmsg;
      if (m_protoMap[(*iNN).second].first->isSatisfied( pp ))pid_found = true ;
    }
    if( pid_found ){
      pprop = m_protoMap[(*iNN).second].second ;
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"PID applied: "<<(*iNN).second <<endmsg;
    }
  }

  // make a new Particle
  LHCb::Particle p = LHCb::Particle();
  // if no PID information, apply pion hypothesis
  if( !pid_found ){
    pprop = m_protoMap["pi+"].second ;
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"PID applied: pi+ (by default)" <<endmsg;
  }

  // Start filling particle with orgininating ProtoParticle
  p.setProto(pp);

  // ParticleID
  const int pID = pprop->particleID().pid() * (int)(pp->charge());
  p.setParticleID( LHCb::ParticleID( pID ) );
  // Mass and error from the PID hypothesis
  p.setMeasuredMass(pprop->mass());
  p.setMeasuredMassErr(0);

  if ( banType > -0.5 ) p.addInfo(951,banType);

  const LHCb::Track* tk = pp->track() ;

  //Remaining info at the first state...
  StatusCode sc = m_p2s->state2Particle( tk->firstState(), p );

  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"In make particle, original p "<<p<<endmsg;
  LHCb::Particle* PFp = p.clone();
  if ( tk->type() == LHCb::Track::Long ||tk->type() == LHCb::Track::Upstream ){
    relate2Vertex(PFp,table);
  }

  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"In make particle: "<<PFp<<endmsg;
  return PFp;
}

//=============================================================================
// Extra Track selection
//=============================================================================
int ParticleFlow4Jets::tagTrack( const LHCb::Track* track )
{
  // Ghost removal for long tracks
  if(track->type()== LHCb::Track::Long && m_useTTHits && !m_useNNGhost ){
    int NTT = 0;
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"selectTrack: Get track meas..."<<endmsg;
    const std::vector< LHCb::LHCbID > & meas = track->lhcbIDs();
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"selectTrack: "<<meas.size()<<" LHCbIDs"<<endmsg;

    for (std::vector< LHCb::LHCbID >::const_iterator im = meas.begin() ; meas.end() != im ; ++ im ){
      if((*im).isTT () )NTT+=1;
    }
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"selectTrack: "<<NTT<<" of them are from TT"<<endmsg;
    // Should have TT hit but don't
    if ( m_ttExpectation->nExpected(*track)!= 0 && NTT == 0 ) return RejectDoNotBanCluster;
    // Should not have TT hit and too small chi2perdof
    if ( m_ttExpectation->nExpected(*track)== 0 && track->chi2PerDoF () > m_noTTChi2PerDof ) return RejectDoNotBanCluster;
  }
  else if (m_useNNGhost){
    info()<<"NOT IMPLEMENTED ghost rejection with NN"<<endmsg;
  }
  if ( m_alsoBanClone && m_trackKeyToBan.size()>0 ){
    //Check that this track is not a clone of one of the banned ones
    for (std::map< int , const LHCb::Track* >::const_iterator itr = m_trackKeyToBan.begin(); m_trackKeyToBan.end()!= itr; ++itr){
      double cloneDist = kullbeckLieblerDist((*itr).second->firstState(),track->firstState());
      if(cloneDist<5000.){
        return RejectBanCluster;
      }

    }
  }
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"selectTrack: Check inf mom..."<<endmsg;
  if(track->type()== LHCb::Track::Long || track->type()== LHCb::Track::Downstream){
    LHCb::State& firstS = track -> firstState () ;
    if ( std::fabs( firstS.qOverP()/sqrt(firstS.errQOverP2()) ) < m_cutInfMomTRVal ){
      return KeepInPFBanned;
    }
  }
  if(track->type()== LHCb::Track::Upstream){
    LHCb::State& firstS = track -> firstState () ;
    if ( std::fabs( firstS.qOverP()/sqrt(firstS.errQOverP2()) ) < m_cutInfMomTRVal ){
      return TurnTo0Momentum;
    }
  }
  if(track->type()== LHCb::Track::Velo){
    return TurnTo0Momentum;
  }
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"selectTrack: passed all cuts"<<endmsg;
  return KeepInPF;
}

StatusCode ParticleFlow4Jets::loadDatas() {
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Enter LoadData"<<endmsg;
  // Get the reconstructed particles ( short cut not to have the reconstructed / corrected particles)
  for (std::vector< std::string >::const_iterator i_location = m_particleLocations.begin() ;
       m_particleLocations.end() != i_location ; ++i_location ){
    if ( msgLevel(MSG::VERBOSE) ) verbose()<<"Look for location: "<<*i_location<<endmsg;
    if(!exist<LHCb::Particles>(*i_location)){
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"No particle at location: "<<*i_location<<endmsg;
      continue;
    }

    if ((*i_location).find("Electron")<100){
      m_particleContainers["Electrons"].push_back( get<LHCb::Particles>(*i_location) );
    }
    if ((*i_location).find("Merged")<100){
      m_particleContainers["MergedPi0"].push_back( get<LHCb::Particles>(*i_location) );
    }
    if ((*i_location).find("Resolved")<100){
      m_particleContainers["ResolvedPi0"].push_back(get<LHCb::Particles>(*i_location) );
    }
    if ((*i_location).find("Photon")<100){
      m_particleContainers["Photons"].push_back(get<LHCb::Particles>(*i_location) ) ;
    }
    if ((*i_location).find("Ks") <100 ||(*i_location).find("Lambda") <100 ){
      m_particleContainers["V0"].push_back(get<LHCb::Particles>(*i_location));
    }
  }

  // Get Charged ProtoParticle container
  if (! exist<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Charged) ){
    return Warning("No charged protopacticles at"+LHCb::ProtoParticleLocation::Charged);
  }
  else{
    m_protoParticles["Charged"] = get<LHCb::ProtoParticles*>(LHCb::ProtoParticleLocation::Charged);
  }

  if (m_useVelo){
    if( exist<LHCb::ProtoParticles>("Rec/ProtoP/VeloProtoPMaker") )
      m_protoParticles["Velo"] = get<LHCb::ProtoParticles*>("Rec/ProtoP/VeloProtoPMaker");
  }
  // Get ECAL Clusters container
  if(exist<LHCb::CaloClusters>( "Rec/Calo/EcalClusters" ))
    m_clusterContainers["Ecal"] = get<LHCb::CaloClusters>( "Rec/Calo/EcalClusters" ) ;

  // Get the ECAL to track relations
  if (!exist<LHCb::Calo2Track::IClusTrTable2D> (m_clust2TrLocationECAL)) {
    return Warning("No track to ECAL cluster relation table");
  }
  m_tableTrECAL = get<LHCb::Calo2Track::IClusTrTable2D>(m_clust2TrLocationECAL);

  // If use HCAL, get the datas
  if (m_useHCAL){
    // Get HCAL Clsuters container
    if(exist<LHCb::CaloClusters>( "Rec/Calo/HcalClusters" ))
      m_clusterContainers["Hcal"] = get<LHCb::CaloClusters>( "Rec/Calo/HcalClusters" ) ;
    // Get the ECAL to track relations
    if (!exist<LHCb::Calo2Track::IClusTrTable2D> (m_clust2TrLocationHCAL) && m_useHCAL) {
      return Warning("No track to HCAL cluster relation table");
    }
    m_tableTrHCAL = get<LHCb::Calo2Track::IClusTrTable2D>(m_clust2TrLocationHCAL);
  }

  m_electronPPkeys.clear();
  // Eventually store electrons PP keys for the BremStralung.
  if(m_catchBremFromElectrons){
    if (m_particleContainers.count("Electrons") < 0.5 ){
      Warning("Catching bermstrahlung gamma fron e is activate but no electrons container is specified");
    }
    else{
      BOOST_FOREACH(const LHCb::Particle* electron, *m_particleContainers["Electrons"][0] )
        if (electron->proto() != NULL)
          m_electronPPkeys.push_back((electron)->proto()->key());
    }
  }

  // Eventually store track keys of candidates that need to be banned
  if(m_banCandidatesLocation!=""){
    if (exist<LHCb::Particles> (m_banCandidatesLocation)){
      const LHCb::Particles* particleToBan = get<LHCb::Particles>( m_banCandidatesLocation ) ;

      BOOST_FOREACH(const LHCb::Particle* cand, *particleToBan ){
        LHCb::Particle::ConstVector particleToBan_daug = cand->daughtersVector();
        if ( particleToBan_daug.size() == 0 ){
          if (cand->proto()==0)continue;
          if (cand->proto()->track()==0)continue;
          m_trackKeyToBan[cand->proto()->track()->key()]=cand->proto()->track();
        }
        else{
          for(LHCb::Particle::ConstVector::const_iterator i_p = particleToBan_daug.begin() ;
              particleToBan_daug.end() != i_p ; ++i_p ){
            if ((*i_p)->proto()==0)continue;
            if ((*i_p)->proto()->track()==0)continue;
            m_trackKeyToBan[(*i_p)->proto()->track()->key()]=(*i_p)->proto()->track();
          }
        }
      }
    }
  }

  // Load the vertices container
  if (! exist<LHCb::RecVertex::Range>(m_verticesLocation) ){
    return Warning("No Vertices at "+m_verticesLocation);
  }
  else{
    m_vertices = get<LHCb::RecVertex::Range>(m_verticesLocation);
  }

  return StatusCode::SUCCESS ;

}


//=============================================================================
// Clone distance calculation
//=============================================================================
double ParticleFlow4Jets::kullbeckLieblerDist( const LHCb::State& c1,
                                               const LHCb::State& c2) const
{
  const Gaudi::TrackVector diffVec = c1.stateVector() - c2.stateVector();
  const Gaudi::TrackSymMatrix diffCov   = c1.covariance()  - c2.covariance();
  Gaudi::TrackSymMatrix invCov1 = c1.covariance();
  Gaudi::TrackSymMatrix invCov2 = c2.covariance();
  TrackUnitsConverters::convertToG3( invCov1 );
  invCov1.Invert();
  TrackUnitsConverters::convertToG4( invCov1 );
  TrackUnitsConverters::convertToG3( invCov2 );
  invCov2.Invert();
  TrackUnitsConverters::convertToG4( invCov2 );

  const Gaudi::TrackSymMatrix invDiff =invCov2  - invCov1;
  const Gaudi::TrackSymMatrix invSum = invCov2  + invCov1;
  const Gaudi::TrackMatrix diff = diffCov * invDiff;

  // trace
  return Gaudi::Math::trace(diff) + Similarity(invSum,diffVec);
}

//=============================================================================
// Add the PF particle 2 Vertex relation in the main relation table
//=============================================================================
void ParticleFlow4Jets::relate2Vertex( const LHCb::Particle* p , Particle2Vertex::WTable& table)
{

  const Particle2Vertex::LightWTable bestPVTable =
    m_pvRelator->relatedPVs(p,
                            LHCb::VertexBase::ConstVector(m_vertices.begin(),
                                                          m_vertices.end()));
  const Particle2Vertex::LightWTable::Range range = bestPVTable.relations();
  table.merge(range);
}
//=============================================================================
// Count the number of saturated cells in a calo cluster
//=============================================================================
int ParticleFlow4Jets::numberOfSaturatedCells( const LHCb::CaloCluster* cluster , const DeCalorimeter* calo){
  int nbSatCells = 0 ;
  const std::vector< LHCb::CaloClusterEntry > & entries = cluster->entries();
  for (std::vector< LHCb::CaloClusterEntry >::const_iterator i_entry = entries.begin(); entries.end()!= i_entry ; i_entry++ ){
    const LHCb::CaloDigit* digit = (*i_entry).digit();
    if( digit->e()/calo->cellGain(digit->cellID())+calo->pedestalShift() > calo->adcMax()-256-0.5 ){
      nbSatCells++;
    }
  }
  return nbSatCells;
}

//=============================================================================
// Create neutral only Particle
//=============================================================================
LHCb::Particle * ParticleFlow4Jets::MakeNeuralParticle(int clusterID , int calo , double remainingE , int numberOfSat, double originalE, double covariance){
  double x(0.),y(0.),z(0.);

  if (calo == 0){
    LHCb::CaloCellID cellID;
    cellID.setAll(clusterID);
    x = m_ecal->cellX(cellID);
    y =  m_ecal->cellY(cellID);
    z =  m_ecal->cellZ(cellID);
  }
  else{
    LHCb::CaloCellID cellID;
    cellID.setAll(clusterID);
    x =  m_hcal->cellX(cellID);
    y =  m_hcal->cellY(cellID);
    z =  m_hcal->cellZ(cellID);
  }
  double px =  remainingE*x/sqrt(x*x + y*y + z*z);
  double py =  remainingE*y/sqrt(x*x + y*y + z*z);
  double pz =  remainingE*z/sqrt(x*x + y*y + z*z);

  LHCb::Particle* p = new LHCb::Particle();
  p->addInfo(901,remainingE);//remaining E
  p->addInfo(902,originalE);//is HCAL or ECAL
  p->addInfo(903,calo);//is HCAL or ECAL
  p->addInfo(904,numberOfSat);
  p->addInfo(905,covariance);//is HCAL or ECAL
  p->setParticleID( LHCb::ParticleID(22) );
  p->setReferencePoint( Gaudi::XYZPoint(0.,0.,0.)  );
  p->setMomentum( Gaudi::XYZTVector(px,py,pz,remainingE) ) ;

  return p;
}
