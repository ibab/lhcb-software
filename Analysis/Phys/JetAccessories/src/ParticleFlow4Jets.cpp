// $Id: $
// Include files 
#include <boost/foreach.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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


  declareProperty( "TrackSelectorType", m_trSelType = "" 
                   , "TrackSelector for choice of track inputs");

  // Track related cuts
  declareProperty( "UsePIDInfo" , m_usePIDInfo  = false ,
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


  // ParticleProperty
  LHCb::IParticlePropertySvc* ppSvc = 0;
  sc = service("LHCb::ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    fatal() << "    Unable to locate Particle Property Service"	  << endreq;
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
    

  info()<<"key cuts for Particle Flow 4 Jets:"<<endmsg;
  info()<<"|--> MinInfMomentumCut              : " << m_cutInfMomTRVal<<endmsg;
  info()<<"|--> MinPhotonID4Photon             : " << m_photonID4Photon <<endmsg;
  info()<<"|--> MinPhotonIDMax4ResolvedPi0     : " << m_photonIDMax4ResolvedPi0 <<endmsg;
  info()<<"|--> MinPhotonIDMin4ResolvedPi0     : " << m_photonIDMin4ResolvedPi0 <<endmsg;
  info()<<"|--> CatchBremFromElectrons         : " << m_catchBremFromElectrons <<endmsg;
  info()<<"|--> MaxChi2NoTT                    : " << m_noTTChi2PerDof <<endmsg; 
  info()<<"|--> MaxMatchECALTr                 : " << m_Chi2ECALCut  <<endmsg;
  info()<<"|--> MaxMatchHCALTr for E < "<<m_Chi2HCAL0CutEValue<<" : " << m_Chi2HCAL0Cut <<endmsg;
  info()<<"|--> MaxMatchHCALTr for  "<<m_Chi2HCAL0CutEValue<<" < P < "<<m_Chi2HCAL1CutEValue<<"          : " 
        << m_Chi2HCAL1Cut  <<endmsg;
  info()<<"|--> MaxMatchHCALTr for P > "<<m_Chi2HCAL1CutEValue<<" : " << m_Chi2HCAL2Cut  <<endmsg;
  
  

  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleFlow4Jets::execute() {

  using namespace LHCb::Calo2Track;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  setFilterPassed(false);

  verbose()<<"Entering execute"<<endreq;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Few definitions ~~~~~~~~~~~~~~~~~~~~~~
  typedef std::map<int, std::pair< int , int > > BannedIDMap;

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

  // Map of ECAL clusters to Ban
  BannedIDMap BannedECALClusters;  
  // Map of HCAL clusters to Ban
  BannedIDMap BannedHCALClusters;    

  // Eventually store the V0 daughters track keys
  if(m_particleContainers.count("V0") > 0.5){
    for( unsigned int i = 0 ; i < m_particleContainers["V0"].size(); i++){
      BOOST_FOREACH(const LHCb::Particle* v0, *m_particleContainers["V0"][i] ){
        LHCb::Particle::ConstVector 	daughtersV0 = v0->daughtersVector();
        for(unsigned i = 0; i<daughtersV0.size(); i++)
          if(daughtersV0.at(i) != NULL)
            if(daughtersV0.at(i)->proto() != NULL)
              m_trackKeyToBan[daughtersV0.at(i)->proto()->track()->key()]=daughtersV0.at(i)->proto()->track();
        PFParticles->insert( v0->clone());
      }
    }
  }

  verbose()<<"Start charged Loop..."<<endreq;
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
        verbose()<<"Track "<<track->type()<<" with pt: "<<track->pt()<<" chi2 per dof "<<track->chi2PerDoF()<<endreq;
        if ( !m_trSel->accept(*track) ) continue;
        verbose()<<"Track accepted"<<endreq;
      }
      
      verbose()<<"Tag the track"<<endreq;
      // Extra track check
      tag = tagTrack(track);
      verbose()<<"Track tagged as "<<tag<<endreq;

      if ( tag== RejectDoNotBanCluster ) continue;
      
      
      // Get the ECAL clusters and ban those with small chi2
      LHCb::Calo2Track::IClusTrTable2D::InverseType::Range cRange =  m_tableTrECAL->inverse()->relations ( track ) ;
      if (cRange.size()>0){
        double chi2TrClu = cRange.front().weight();
        verbose()<<"ECAL Track match at "<<chi2TrClu<<" with Cluster ID : "<<cRange.front().to()->seed().all()
                 <<" Is tagged already?: "<<BannedECALClusters.count(cRange.front().to()->seed().all())<<endreq;
        
        if ( chi2TrClu < m_Chi2ECALCut && BannedECALClusters.count(cRange.front().to()->seed().all())<0.5 ) {
          verbose()<<"Will tag as: "<<TrackMatch<<" from the track "<<track->key()<<endreq;
          std::pair< double , int > tmpPair;
          tmpPair.first = TrackMatch ;
          tmpPair.second =  track->key() ;
          BannedECALClusters[cRange.front().to()->seed().all()] = tmpPair ;
        }
      }
      
      if(m_useHCAL ){
        // Get the calorimeter clusters associated to the track
        LHCb::Calo2Track::IClusTrTable2D::InverseType::Range cRange2 =  m_tableTrHCAL->inverse()->relations ( track ) ;
        if (cRange2.size()>0){
          double chi2TrClu = cRange2.front().weight();
          verbose()<<"HCAL Track match at "<<chi2TrClu<<" with track p "<< track->p()<<" with Cluster ID : "
                   <<cRange2.front().to()->seed().all()<<" Is tagged already?: "
                   <<BannedHCALClusters.count(cRange2.front().to()->seed().all())<<endreq;
          if ( ((chi2TrClu < m_Chi2HCAL0Cut && cRange2.front().to()->e() <= m_Chi2HCAL0CutEValue) || 
                (chi2TrClu < m_Chi2HCAL1Cut && cRange2.front().to()->e() > m_Chi2HCAL0CutEValue
                 && cRange2.front().to()->e() <=  m_Chi2HCAL1CutEValue) || 
                (chi2TrClu < m_Chi2HCAL2Cut && cRange2.front().to()->e() > m_Chi2HCAL1CutEValue) )
               && BannedHCALClusters.count(cRange2.front().to()->seed().all())<0.5 ) {
            verbose()<<"Will tag as: "<<TrackMatchHCAL<<" form the track "<<track->key()<<endreq;
            std::pair< double , int > tmpPair;
            tmpPair.first = TrackMatchHCAL ;
            tmpPair.second =  track->key() ;
            BannedHCALClusters[cRange2.front().to()->seed().all()] = tmpPair ;     
          }
        }
      }
      // Save the particle
      if (tag == KeepInPF){
        PFParticles->insert( MakeParticle( ch_pp , -1 ) );
      }
      else if (tag ==  KeepInPFBanned ){
        BannedPFParticles->insert( MakeParticle( ch_pp , InfMom ) );
      }
      else if (tag == Unknown ){
        Warning("Unknow status for this charged particle");
      }
      
      
      // Check if belongs to an electron hypothesis...
      if(m_catchBremFromElectrons){      
        for ( unsigned int i = 0 ; i < m_electronPPkeys.size() ; i++ ){
          if (m_electronPPkeys[i] != ch_pp->key()) continue;
          const SmartRefVector< LHCb::CaloHypo > &  caloHypos =	ch_pp -> calo();
          for (SmartRefVector< LHCb::CaloHypo >::const_iterator i_calo = caloHypos.begin() ; caloHypos.end()!=i_calo ;++i_calo){
            if ( (*i_calo)->hypothesis() != LHCb::CaloHypo::Photon ) {
              if ( (*i_calo)->hypothesis() != LHCb::CaloHypo::EmCharged ) {
                verbose()<<"One of the electron calo Hypo IS NOT Photon, NOR EmCharged "<< (*i_calo)->hypothesis()<<endreq;
              }continue;
            }
            const SmartRefVector< LHCb::CaloCluster > & hypoClusters = 	(*i_calo)->clusters ();
            if (BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5 )continue;
            std::pair< double , int > tmpPair;
            tmpPair.first = Bremsstrahlung ;
            tmpPair.second =  track->key() ;
            BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;
          }   
        }
      }
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ The loops for electromagnetic neutral components  ~~~~~~~~~~~~~~~~~~~~~~
  
  // Loop over Merged Pi0, if clusters are not tagged yet, use and tag
  verbose()<<"Start merged pi0 Loop..."<<endreq;
  if (m_particleContainers.count("MergedPi0") > 0.5 ){
    
    BOOST_FOREACH(const LHCb::Particle* MPi0, *m_particleContainers["MergedPi0"][0] ){
      
      const LHCb::ProtoParticle* n_pp = MPi0->proto();
      if (n_pp == NULL) continue;
      const SmartRefVector< LHCb::CaloHypo > &  caloHypos =	n_pp -> calo();
      const SmartRefVector< LHCb::CaloCluster > & hypoClusters = 	caloHypos[0]->clusters ();

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
      // Save the particle
      PFParticles->insert(MPi0->clone());
    }
  }
  
  // Loop over Resolved Pi0, if clusters are not tagged yet, use and tag
  verbose()<<"Start resolved pi0 Loop..."<<endreq;
  if (m_particleContainers.count("ResolvedPi0") > 0.5 ){
    
    BOOST_FOREACH(const LHCb::Particle* RPi0, *m_particleContainers["ResolvedPi0"][0] ){
      LHCb::Particle::ConstVector daugs = RPi0->daughtersVector ();
      double minPhotonID = -1000.;
      double maxPhotonID = -1000.;
      bool skip = false;
      std::vector< int > caloCells;
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
        const SmartRefVector< LHCb::CaloHypo > &  caloHypos =	n_pp -> calo();
        const SmartRefVector< LHCb::CaloCluster > & hypoClusters = 	caloHypos[0]->clusters ();
        // Store the cell ID
        caloCells.push_back(hypoClusters[0].target()->seed().all());
        
        // If the cluster have already been tagged, skip.
        if (BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5){
          skip = true;
          continue;
        }
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
        // Save the particle
        PFParticles->insert(RPi0->clone());
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
  verbose()<<"Start Photon Loop..."<<endreq;
  if (m_particleContainers.count("Photons") > 0.5 ){
    
    BOOST_FOREACH(const LHCb::Particle* photon, *m_particleContainers["Photons"][0]){
      const LHCb::ProtoParticle* n_pp = photon->proto();
      if (n_pp == NULL)  continue;
      const SmartRefVector< LHCb::CaloHypo > &  caloHypos =	n_pp -> calo();
      const SmartRefVector< LHCb::CaloCluster > & hypoClusters = 	caloHypos[0]->clusters ();
      // Use Ttrack banning?
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
        
        PFParticles->insert( newP );
      }
    }
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ The loops for hadronic neutral components  ~~~~~~~~~~~~~~~~~~~~~~
  verbose()<<"m_useHCAL "<<m_useHCAL<<endreq;
  
  if(m_useHCAL){
    verbose()<<"Start HCAL Loop..."<<endreq;
    LHCb::ProtoParticles* PFProtoParticles = new LHCb::ProtoParticles();
    put( PFProtoParticles , m_protoPF );
    LHCb::CaloHypos* PFCaloHypos = new LHCb::CaloHypos();
    put( PFCaloHypos , m_calohypoPF );
    verbose()<<"m_clusterContainers.count('Hcal') "<<m_clusterContainers.count("Hcal")<<endreq;
    if (m_clusterContainers.count("Hcal") > 0.5 ){
      
      BOOST_FOREACH(const  LHCb::CaloCluster* cluster, *m_clusterContainers["Hcal"]){
        
        
        if ( 0 == cluster ) { continue ; }
        //cluster energy:
        const double cluen = cluster->e();
        if (cluen<m_minHCALE) continue;
        
        //verbose()<<"Clust e "<<cluen<<endreq;
        //cluster position
        const LHCb::CaloCluster::Position clupos = cluster->position();
        
        double x =  clupos.x();
        double y =  clupos.y();
        double z =  clupos.z();
        
        double px =  cluen*x/sqrt(x*x + y*y + z*z);
        double py =  cluen*y/sqrt(x*x + y*y + z*z);
        double pz =  cluen*z/sqrt(x*x + y*y + z*z);
        
        Gaudi::LorentzVector pclu = Gaudi::LorentzVector(px,py,pz,cluen);
        
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
        
        p.setParticleID( LHCb::ParticleID(-22) );
        p.setReferencePoint( Gaudi::XYZPoint(0.,0.,0.)  );
        p.setMomentum( Gaudi::XYZTVector(px,py,pz,cluen) ) ;
        

        if ( BannedHCALClusters.count(cluster->seed().all())<0.5 && p.momentum().eta() < 4.2 ){
          verbose()<<"Cluster not banned: "<<cluster->seed().all()<<" "<<cluen<<" "<<m_tableTrHCAL->relations ( cluster ).front().weight()<<endreq;
          PFParticles->insert(p.clone());
          PFProtoParticles->insert(NewProto);
        }
        else {
          delete NewProto;
          verbose()<<"Cluster banned: "<<cluster->seed().all()<<" "<<cluen<<" "<<m_tableTrHCAL->relations ( cluster ).front().weight()<<endreq;
        }
      }
    }
  }
  
  debug()<<"PFParticles: "<<PFParticles->size()<<" PFParticlesBanned: "<<BannedPFParticles->size()<<endreq;
  
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
LHCb::Particle * ParticleFlow4Jets::MakeParticle( const LHCb::ProtoParticle * pp , int banType ){
  
  bool pid_found(false);

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

    if ( !m_usePIDInfo ){ 
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNp    ,-0.5) ] =  "p+"  ;
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNk    ,-0.5) ] =  "K+"  ;
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNe    ,-0.5) ] =  "e+"  ;
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNmu   ,-0.5) ] =  "mu+" ;
    }
  
    // the map is by construction sorted by key value (lower first)
    MapProbNNPID::iterator iNN = probNNpid.end();
    iNN--;
  
  
    verbose()<<"Ordered hypothesis: type "<<(*iNN).first<<" "<<(*iNN).second<<endreq;
    // Reverse loop, check if filter is satisfied

    if (m_protoMap[(*iNN).second].first->isSatisfied( pp ))pid_found = true ;

    while( !pid_found && iNN != probNNpid.begin() ){
      iNN--;
      verbose()<<"Ordered hypothesis: type "<<(*iNN).first <<" with proba "<<(*iNN).second <<endreq;
      if (m_protoMap[(*iNN).second].first->isSatisfied( pp ))pid_found = true ;  
    }
    if( pid_found ){
      m_protoMap[(*iNN).second].second ;
      verbose()<<"PID applied: "<<(*iNN).second <<endreq;
    }
  }
  
  // make a new Particle
  LHCb::Particle p = LHCb::Particle();
  // if no PID information, apply pion hypothesis 
  if( !pid_found ){
    pprop = m_protoMap["pi+"].second ;
    verbose()<<"PID applied: pi+ (by default)" <<endreq;
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

  return p.clone();
}

//=============================================================================
// Extra Track selection
//=============================================================================
int ParticleFlow4Jets::tagTrack( const LHCb::Track* track )
{ 
  // Ghost removal for long tracks
  if(track->type()== LHCb::Track::Long && m_useTTHits && !m_useNNGhost ){
    int NTT = 0;
    verbose()<<"selectTrack: Get track meas..."<<endreq;
    const std::vector< LHCb::LHCbID > & meas = track->lhcbIDs();
    verbose()<<"selectTrack: "<<meas.size()<<" LHCbIDs"<<endreq;
    
    for (std::vector< LHCb::LHCbID >::const_iterator im = meas.begin() ; meas.end() != im ; ++ im ){
      if((*im).isTT () )NTT+=1;
    }
    verbose()<<"selectTrack: "<<NTT<<" of them are from TT"<<endreq;
    // Should have TT hit but don't
    if ( m_ttExpectation->nExpected(*track)!= 0 && NTT == 0 ) return RejectDoNotBanCluster;
    // Should not have TT hit and too small chi2perdof
    if ( m_ttExpectation->nExpected(*track)== 0 && track->chi2PerDoF () > m_noTTChi2PerDof ) return RejectDoNotBanCluster;
  }
  else if (m_useNNGhost){
    info()<<"NOT IMPLEMENTED ghost rejection with NN"<<endreq;
  }
  if ( m_alsoBanClone && m_trackKeyToBan.size()>0 ){
    //Check that this track is not a clone of one of the banned ones
    for (std::map< int , const LHCb::Track* >::const_iterator itr = m_trackKeyToBan.begin(); m_trackKeyToBan.end()!= itr; ++itr){
      double cloneDist = kullbeckLieblerDist((*itr).second->firstState(),track->firstState());
      if(cloneDist<5000.)return RejectBanCluster;
    }
  }
  verbose()<<"selectTrack: Check inf mom..."<<endreq;
  if(track->type()== LHCb::Track::Long){
    LHCb::State& firstS = track ->	firstState () ;
    if ( std::fabs( firstS.qOverP()/sqrt(firstS.errQOverP2()) ) < m_cutInfMomTRVal ){
      return KeepInPFBanned;
    }
  }
  verbose()<<"selectTrack: passed all cuts"<<endreq;
  return KeepInPF;
}

StatusCode ParticleFlow4Jets::loadDatas() {
  verbose()<<"Enter LoadData"<<endreq;
  // Get the reconstructed particles ( short cut not to have the reconstructed / corrected particles)
  for (std::vector< std::string >::const_iterator i_location = m_particleLocations.begin() ; 
       m_particleLocations.end() != i_location ; ++i_location ){ 
    verbose()<<"Look for location: "<<*i_location<<endreq;
    if(!exist<LHCb::Particles>(*i_location)){
      verbose()<<"No particle at location: "<<*i_location<<endreq;
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
    const LHCb::Particles* particleToBan = get<LHCb::Particles>( m_banCandidatesLocation ) ;
    BOOST_FOREACH(const LHCb::Particle* cand, *particleToBan ){
      LHCb::Particle::ConstVector particleToBan_daug = cand->daughtersVector();
      if ( particleToBan_daug.size() == 0 ){
        if (cand->proto()==0)continue;
        if (cand->proto()->track()==0)continue;
        m_trackKeyToBan[cand->proto()->track()->key()]=cand->proto()->track();
      }
      else{
        BOOST_FOREACH(const LHCb::Particle* cand_d, particleToBan_daug ){
          if (cand_d->proto()==0)continue;
          if (cand_d->proto()->track()==0)continue;
          m_trackKeyToBan[cand->proto()->track()->key()]=cand_d->proto()->track();
        }
      }
    }
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
