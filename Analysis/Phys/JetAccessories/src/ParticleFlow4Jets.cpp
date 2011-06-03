// $Id: $
// Include files 

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

// Relations
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted2D.h"
// local
#include "ParticleFlow4Jets.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleFlow4Jets
//
// 2011-05-10 : Victor Coco
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticleFlow4Jets )


/*********************************************************
 *
 *                   TODO LIST
 *        ( please update when modifying )
 *
 *     - Modify the MakeParticle function to use the globalPID NN
 *     - Add the CnvPhotons
 *     - Get HCAL charged digits and Calo2Calo for others
 *     - Write a control Tuple in the end
 *
 *********************************************************/


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleFlow4Jets::ParticleFlow4Jets( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_trSel ( 0 )
  , m_ttExpectation ( 0 )
  , m_clust2TrLocation ( "" )
                                              
{ 
  declareProperty( "PFOutputLocation"          ,  m_PFOutputLocation      
                   ,  "Output Location of the charged protoparticles 4 jets") ;  
  declareProperty( "TrackSelectorType", m_trSelType = "" 
                   , "TrackSelector for choice of track inputs");
  declareProperty( "MaxMatchCaloTr" , m_Chi2CaloCut = 4. ,
                   "Maximal calo<-->track matching chi2");
  declareProperty( "MinInfMomentumCut" , m_cutInfMomTRVal  = -1. ,
                   "Minimal cut on sigma_(q/p)/(q/p)");
  declareProperty( "ParticleLocations" ,m_particleLocations ,
                   "Location of premade particles to be used" );
  declareProperty( "MinPhotonID4Photon" , m_photonID4Photon = 0. ,
                   "Minimum photon pid for photon hypothesis");
  declareProperty( "MinPhotonID4ResolvedPi0" , m_photonID4ResolvedPi0 = 0. ,
                   "Minimum photon pid for photon from resolved Pi0");
  declareProperty( "CatchBremFromElectrons" , m_catchBremFromElectrons = false ,
                   "Flag bremstrahlung phtons from electrons");
  declareProperty( "MaxChi2NoTT" , m_noTTChi2PerDof = 2. ,
                   "Maximum chi2 per DoF for long track with no TTHits");
  m_clust2TrLocation = LHCb::CaloAlgUtils::CaloIdLocation("ClusterMatch", context());
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
  m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");  

  // ParticleProperty
  LHCb::IParticlePropertySvc* ppSvc = 0;
  sc = service("LHCb::ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    fatal() << "    Unable to locate Particle Property Service"	  << endreq;
    return sc;
  }

  // TODO turn it to ProbNN
  m_protoMap.push_back( ProtoPair( ppSvc->find( "pi+" ),
                                   tool<IProtoParticleFilter>( "ProtoParticleCALOFilter", "pion", this ) ) );
  m_protoMap.push_back( ProtoPair( ppSvc->find( "e+" ),
                                   tool<IProtoParticleFilter>( "ProtoParticleCALOFilter", "electron", this ) ) );
  m_protoMap.push_back( ProtoPair( ppSvc->find( "K+" ),
                                   tool<IProtoParticleFilter>( "ProtoParticleCALOFilter", "kaon", this ) ) );
  m_protoMap.push_back( ProtoPair( ppSvc->find( "p+" ),
                                   tool<IProtoParticleFilter>( "ProtoParticleCALOFilter", "proton", this ) ) );
  m_protoMap.push_back( ProtoPair( ppSvc->find( "mu+" ),
                                   tool<IProtoParticleFilter>( "ProtoParticleMUONFilter", "muon", this ) ) );
    
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get the input particles
//=============================================================================

StatusCode ParticleFlow4Jets::getContainers(){
  for (std::vector< std::string >::const_iterator i_location = m_particleLocations.begin() ; 
       m_particleLocations.end() != i_location ; ++i_location ){ 
    if(!exist<LHCb::Particles>(*i_location)) return StatusCode::FAILURE;
    if ((*i_location).find("Electron")<100){
      m_particleContainers["Electrons"] = get<LHCb::Particles>(*i_location);
    }
    if ((*i_location).find("Merged")<100){
      m_particleContainers["MergedPi0"] = get<LHCb::Particles>(*i_location);
    }
    if ((*i_location).find("Resolved")<100){
      m_particleContainers["ResolvedPi0"] = get<LHCb::Particles>(*i_location);
    }
    if ((*i_location).find("Cnv")<100){
      m_particleContainers["CnvPhotons"] = get<LHCb::Particles>(*i_location);
    }
    if ((*i_location).find("LoosePhoton")<100){
      m_particleContainers["Photons"] = get<LHCb::Particles>(*i_location);
    }
  }
  return StatusCode::SUCCESS ;
}




//=============================================================================
// Extra Track selection
//=============================================================================
bool ParticleFlow4Jets::selectTrack( const LHCb::Track* track )
{
  verbose()<<"selectTrack: Get the first state..."<<endreq;
  // Cut for infinite momentum
  LHCb::State& firstS = track ->	firstState () ;
  verbose()<<"selectTrack: Check inf mom..."<<endreq;
  if ( std::fabs( firstS.qOverP()/sqrt(firstS.errQOverP2()) ) < m_cutInfMomTRVal )
       return false ;
  
  verbose()<<"selectTrack: pass inf mom..."<<endreq;
  // Ghost removal for long tracks
  if(track->type()== LHCb::Track::Long ){
    int NTT = 0;
    verbose()<<"selectTrack: Get track meas..."<<endreq;
    const std::vector< LHCb::LHCbID > & meas = track->lhcbIDs();
    verbose()<<"selectTrack: "<<meas.size()<<" LHCbIDs"<<endreq;
    
    for (std::vector< LHCb::LHCbID >::const_iterator im = meas.begin() ; meas.end() != im ; ++ im ){
      if((*im).isTT () )NTT+=1;
    }
    verbose()<<"selectTrack: "<<NTT<<" of them are from TT"<<endreq;
    // Should have TT hit but don't
    if ( m_ttExpectation->nExpected(*track)!= 0 && NTT == 0 ) return false;
    // Should not have TT hit and too small chi2perdof
    if ( m_ttExpectation->nExpected(*track)== 0 && track->chi2PerDoF () > m_noTTChi2PerDof ) return false;
  }
  verbose()<<"selectTrack: passed all cuts"<<endreq;
  return true;
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

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ First get all the staff ~~~~~~~~~~~~~~~~~~~~~~

  // Get the reconstructed particles ( short cut not to have the reconstructed / corrected particles)
  if( getContainers().isFailure() ) 
    return Warning("Failed to get the input particles");

  // Get Charged ProtoParticle container
  if (! exist<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Charged) ) 
    return Warning("No charged protopacticles at"+LHCb::ProtoParticleLocation::Charged);
  
  LHCb::ProtoParticles* ChargedPP = get<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Charged);

  // Get the calo to terack relations
  LHCb::Calo2Track::IClusTrTable2D::InverseType::Range cRange;
  
  if (!exist<LHCb::Calo2Track::IClusTrTable2D> (m_clust2TrLocation)) {
    return Warning("No track to cluster relation table");
  }
  LHCb::Calo2Track::IClusTrTable2D* table = get<LHCb::Calo2Track::IClusTrTable2D>(m_clust2TrLocation);

  // Eventually store electrons PP keys
  std::vector< int > electronPPkeys;
  if(m_catchBremFromElectrons){
    if (m_particleContainers.count("Electrons") < 0.5 ){
      Warning("Catching bermstrahlung gamma fron e is activate but no electrons container is specified"); 
    }
    else{  
      for (LHCb::Particles::const_iterator ie = m_particleContainers["Electrons"] -> begin()
             ; m_particleContainers["Electrons"] -> end() != ie ; ++ie ){
        if ((*ie)->proto() == NULL)  continue;
        electronPPkeys.push_back((*ie)->proto()->key());
      }
    }
  }


  // Create the output container
  LHCb::Particles* PFParticles = new LHCb::Particles();
  put( PFParticles , m_PFOutputLocation );

  // Map of ECAL clusters to Ban
  BannedIDMap BannedECALClusters;  
  // Map of HCAL clusters to Ban
  BannedIDMap BannedHCALDigits;  


  verbose()<<"Start charged Loop..."<<endreq;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ The main loop for charged components  ~~~~~~~~~~~~~~~~~~~~~~
  // Loop over charged Protoparticles, reject bad tracks, make best PID hypo particle out of it
  // store the used ECAL cluster and HCAL digits

  for (LHCb::ProtoParticles::const_iterator ich_pp = ChargedPP -> begin() ; ChargedPP -> end() != ich_pp ; ++ich_pp ){
    const LHCb::ProtoParticle* ch_pp = *ich_pp;
    const LHCb::Track* track = ch_pp->track();
    
    if ( track == NULL ) continue;
    // Check Track
    if ( m_trSel!= 0 ){
      verbose()<<"Track "<<track->type()<<" with pt: "<<track->pt()<<" chi2 per dof "<<track->chi2PerDoF()<<endreq;
      if ( !m_trSel->accept(*track) ) continue;
      verbose()<<"Track accepted"<<endreq;
    }
    
    // Extra cuts
    if ( !selectTrack(track) ) continue;

    // Get the calorimeter clusters
    cRange = table  ->inverse()->relations ( track ) ;
    if (cRange.size()>0){
      
      double chi2TrClu = cRange.front().weight();
      verbose()<<"Track match at "<<chi2TrClu<<" with Cluster ID : "<<cRange.front().to()->seed().all()
               <<" Is tagged already?: "<<BannedECALClusters.count(cRange.front().to()->seed().all())<<endreq;
      
      // TODO: look if the info cannot be taken directly from the protoP
      // TODO: remove the clusters from electrons brem
      if ( chi2TrClu < m_Chi2CaloCut && BannedECALClusters.count(cRange.front().to()->seed().all())<0.5 ) {
        verbose()<<"Will tag as: "<<TrackMatch<<" the track "<<track->key()<<endreq;
        std::pair< double , int > tmpPair;
        tmpPair.first = TrackMatch ;
        tmpPair.second =  track->key() ;
        BannedECALClusters[cRange.front().to()->seed().all()] = tmpPair ;
      }
    }
    
    // Save the particle
    PFParticles->insert(MakeParticle(ch_pp));

    // Check if belongs to an electron hypothesis...
    if(m_catchBremFromElectrons){      
      for ( unsigned int i = 0 ; i < electronPPkeys.size() ; i++ ){
        if (electronPPkeys[i] != ch_pp->key()) continue;
        const SmartRefVector< LHCb::CaloHypo > &  caloHypos =	ch_pp -> calo();
        for (SmartRefVector< LHCb::CaloHypo >::const_iterator i_calo = caloHypos.begin() ; caloHypos.end()!=i_calo ;++i_calo){
          if ( (*i_calo)->hypothesis() != LHCb::CaloHypo::Photon ) {
            if ( (*i_calo)->hypothesis() != LHCb::CaloHypo::EmCharged ) {
              always()<<"One of the electron calo Hypo IS NOT Photon, NOR EmCharged "<< (*i_calo)->hypothesis()<<endreq;
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
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ The loops for neutral components  ~~~~~~~~~~~~~~~~~~~~~~

  verbose()<<"Start merged pi0 Loop..."<<endreq;
  // Loop over Merged Pi0, if clusters are not tagged yet, use and tag
  if (m_particleContainers.count("MergedPi0") > 0.5 ){
    for (LHCb::Particles::const_iterator in_p = m_particleContainers["MergedPi0"] -> begin()
           ; m_particleContainers["MergedPi0"] -> end() != in_p ; ++in_p ){
      const LHCb::ProtoParticle* n_pp = (*in_p)->proto();
      if (n_pp == NULL) continue;
      const SmartRefVector< LHCb::CaloHypo > &  caloHypos =	n_pp -> calo();
      const SmartRefVector< LHCb::CaloCluster > & hypoClusters = 	caloHypos[0]->clusters ();

      // If the cluster have already been tagged, skip.
      if (BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5)continue;
      std::pair< double , int > tmpPair;
      tmpPair.first = MergedPi0 ;
      tmpPair.second =  (*in_p)->key() ;
      BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;

      // Save the particle
      PFParticles->insert((*in_p)->clone());
    }
  }
  
  // Loop over Resolved Pi0, if clusters are not tagged yet, use and tag

  verbose()<<"Start resolved pi0 Loop..."<<endreq;
  if (m_particleContainers.count("ResolvedPi0") > 0.5 ){
    for (LHCb::Particles::const_iterator in_p = m_particleContainers["ResolvedPi0"] -> begin()
           ; m_particleContainers["ResolvedPi0"] -> end() != in_p ; ++in_p ){
      LHCb::Particle::ConstVector daugs = (*in_p)->daughtersVector ();
      double minPhotonID = -1000.;
      bool skip = false;
      std::vector< int > caloCells;

      // Loop over daughters
      for  (LHCb::Particle::ConstVector::const_iterator i_daug = daugs. begin()  ; daugs.end() != i_daug ; ++i_daug ){
        const LHCb::ProtoParticle* n_pp = (*i_daug)->proto();
        if (n_pp == NULL)  continue;

        // Get their minimal PhotonID
        double PhotonID = (*i_daug)->proto()->info( LHCb::ProtoParticle::PhotonID,-100.) ;
        if( minPhotonID < -999.) minPhotonID = PhotonID ;
        if ( PhotonID < minPhotonID ) minPhotonID = PhotonID ;
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
      }
      // If both daugther have correct ID and are not used, save the particle and ban
      if ( minPhotonID > m_photonID4ResolvedPi0 && !skip ){

        // Save the particle
        PFParticles->insert((*in_p)->clone());

        // ban the corresponding clusters
        for (unsigned int iclu = 0 ; iclu < caloCells.size(); iclu++){ 
          std::pair< double , int > tmpPair;
          tmpPair.first = ResolvedPi0 ;
          tmpPair.second =  (*in_p)->key() ;
          BannedECALClusters[caloCells[iclu]] = tmpPair ;
        }
      }
    }
  }
  

  // Loop over Loose Photons, if clusters are not tagged yet, use and tag

  verbose()<<"Start Photon Loop..."<<endreq;
  if (m_particleContainers.count("Photons") > 0.5 ){
    for (LHCb::Particles::const_iterator in_p = m_particleContainers["Photons"] -> begin()
           ; m_particleContainers["Photons"] -> end() != in_p ; ++in_p ){
      const LHCb::ProtoParticle* n_pp = (*in_p)->proto();
      if (n_pp == NULL)  continue;
      const SmartRefVector< LHCb::CaloHypo > &  caloHypos =	n_pp -> calo();
      const SmartRefVector< LHCb::CaloCluster > & hypoClusters = 	caloHypos[0]->clusters ();

      // If the cluster have already been tagged, or does not match the photonID reqirements skip.
      if (BannedECALClusters.count(hypoClusters[0].target()->seed().all())>0.5 
          || n_pp ->info( LHCb::ProtoParticle::PhotonID,-100.)< m_photonID4Photon )continue;
      std::pair< double , int > tmpPair;
      tmpPair.first =  Photon ;
      tmpPair.second =  (*in_p)->key() ;
      BannedECALClusters[hypoClusters[0].target()->seed().all()] = tmpPair ;

      // Save the particle
      PFParticles->insert((*in_p)->clone());
    }
  }
  
  
  debug()<<"PFParticles: "<<PFParticles->size()<<endreq;
  
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
//   Adapted from BestPIDParticleMaker
// THIS HAS TO BE MODIFIED.............
//=============================================================================
LHCb::Particle * ParticleFlow4Jets::MakeParticle( const LHCb::ProtoParticle * pp ){
  
  // Test PID info consistency
  
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

  //Create a vector of PID and DLL pairs
  DLLPIDVector dllpid;
  dllpid.push_back( DLLPIDPair( "pi+", 
                                pp->info( LHCb::ProtoParticle::CombDLLpi,0) ) );
  double dll = pp->info( LHCb::ProtoParticle::CombDLLmu,0);
  if( dll > -0.01 ) dllpid.push_back( DLLPIDPair( "mu+", dll ) );
  dll = pp->info( LHCb::ProtoParticle::CombDLLe,0);
  if( dll > -0.01 ) dllpid.push_back( DLLPIDPair( "e+", dll ) );
  dll = pp->info( LHCb::ProtoParticle::CombDLLk,0);
  if( dll > -0.01 ) dllpid.push_back( DLLPIDPair( "K+", dll ) );
  dll = pp->info( LHCb::ProtoParticle::CombDLLp,0);
  if( dll > -0.01 ) dllpid.push_back( DLLPIDPair( "p+", dll ) );

  //Sort the DLL
  sort( dllpid.begin(), dllpid.end(), DLLPIDPair::cmp );

  //Find the best PID
  DLLPIDVector::iterator i = dllpid.begin();
  ProtoMap::const_iterator imap;
  while( i != dllpid.end() ){    
    std::string & pid = (*i).GetPid();      
     //Continue if filters are not satisfied
    bool sel = false;
    for ( imap = m_protoMap.begin(); imap != m_protoMap.end(); ++imap ){
      if( (*imap).first->particle() == pid ){
        sel = (*imap).second->isSatisfied( pp );
        break;
      }
    }
    if( !sel ){ i++; continue; }      
      //get out of here
    break;
  }
  
  // make a new Particle
  LHCb::Particle p = LHCb::Particle();
  const LHCb::ParticleProperty* pprop = (*imap).first;
  // Start filling particle with orgininating ProtoParticle
  p.setProto(pp);
  // ParticleID
  const int pID = pprop->particleID().pid() * (int)(pp->charge());
  p.setParticleID( LHCb::ParticleID( pID ) );
  // Mass and error
  p.setMeasuredMass(pprop->mass());
  p.setMeasuredMassErr(0);

  const LHCb::Track* tk = pp->track() ;
  //StatusCode sc = p2s()->state2Particle( tk->firstState(), p );
  //Remaining info at the first state...
  p.setReferencePoint( tk->position() );
  // momentum
  const Gaudi::XYZVector & mom = tk->momentum();
  const double mass = p.measuredMass();
  const double e = sqrt( tk->p()*tk->p()+mass*mass );
  p.setMomentum( Gaudi::XYZTVector(mom.X(),mom.Y(),mom.Z(),e) ) ;
  //error on position and momentum empty. If needed, use State2Particle

  return p.clone();
}

