// $Id: ChargedProtoPAlg.cpp,v 1.4 2002-07-25 19:41:14 gcorti Exp $
// Include files 
#include <memory>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from EventSys
#include "Event/TrStoredTrack.h"
#include "Event/RichPID.h"
#include "Event/MuonID.h"
#include "Event/ProtoParticle.h"

// local
#include "ChargedProtoPAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedProtoPAlg
//
// 2002-07-08 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ChargedProtoPAlg>          s_factory ;
const        IAlgFactory& ChargedProtoPAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoPAlg::ChargedProtoPAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_electronPath( "Rec/Calo/Electrons" )
  , m_trkmatchPath( "AssociatorWeighted<CaloCluster,TrStoredTrack,float>"
                    + std::string("/PhotonMatch") )
  , m_caloematchPath( "AssociatorWeighted<CaloHypo,TrStoredTrack,float>"
                      + std::string("/ElectronMatch") )
  , m_bremmatchPath( "AssociatorWeighted<CaloHypo,TrStoredTrack,float>"
                     + std::string("/BremMatch") ) 
  , m_lastChiSqMax( 100 )
  //, m_upstreamTracks( false )
  //, m_zFirstPoint( 3000.0*mm )
{
  
  //, m_trkmatch(0)
  //, m_spdPrs    ( 0 ) {

  // Inputs
  declareProperty("TrStoredTracksInput", 
                  m_tracksPath = TrStoredTrackLocation::Default );
  declareProperty("RichPIDsInput",
                  m_richPath = RichPIDLocation::Default );
  declareProperty("MuonPIDsInput",
                  m_muonPath = MuonIDLocation::Default );
  declareProperty("ElectronsInput", m_electronPath );
  declareProperty("TrackMatching", m_trkmatchPath );
  declareProperty("CaloEMatching", m_caloematchPath );
  declareProperty("BremMatching", m_bremmatchPath );
  
  // Outputs
  declareProperty("OutputData",
                  m_protoPath = ProtoParticleLocation::Charged );

  // Selections
  declareProperty("MaxChiSquare", m_lastChiSqMax );
  //declareProperty("UpstreamsTracks", m_upstream );
  //declareProperty("ZFirstState", m_zFirstPoint );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoPAlg::~ChargedProtoPAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ChargedProtoPAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc;
  IParticlePropertySvc* ppSvc;
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return sc;
  }
  ParticleProperty* partProp;

  partProp = ppSvc->find( "e+" );
  m_idElectron = partProp->jetsetID();

  partProp = ppSvc->find( "mu+" );
  m_idMuon = partProp->jetsetID();

  partProp = ppSvc->find( "pi+" );
  m_idPion = partProp->jetsetID();

  partProp = ppSvc->find( "K+" );
  m_idKaon = partProp->jetsetID();

  partProp = ppSvc->find( "p+" );
  m_idProton = partProp->jetsetID();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoPAlg::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // Load stored tracks
  SmartDataPtr<TrStoredTracks> tracks ( eventSvc(), m_tracksPath );
  if( !tracks || 0 == tracks->size() ) {
    log << MSG::DEBUG << "Unable to retrieve TrStoredTracks at "
        << m_tracksPath << endreq;
    return StatusCode::FAILURE;
  }
  else {   
    log << MSG::DEBUG << "Successfully retrieved " << tracks->size()
        << " TrStoredTracks at " << m_tracksPath << endreq;
  }

  // Load richPid results
  bool richData;
  SmartDataPtr<RichPIDs> richpids ( eventSvc(), m_richPath );
  if( !richpids || 0 == richpids->size() ) {
    log << MSG::INFO  << "Failed to locate RichPIDs at "
        << m_richPath << endreq;
  }
  else {   
    log << MSG::DEBUG << "Successfully located " << richpids->size()
        << " RichPIDs at " << m_richPath << endreq;
    richData = true;
  }
  
  // Load muonPid results
  bool muonData;
  SmartDataPtr<MuonIDs> muonpids ( eventSvc(), m_muonPath );
  if( !muonpids || 0 == muonpids->size() ) {
    log << MSG::INFO << "Failed to locate MuonIDs at "
        << m_muonPath << endreq;
  }
  else {
    log << MSG::DEBUG << "Successfully located " << muonpids->size()
        << " MuonIDs at " << m_muonPath << endreq;
    muonData = true;
  }

  // Load Electron results
  bool caloData = false;
  SmartDataPtr<CaloHypos> electrons ( eventSvc(), m_electronPath );
  if( !electrons || 0 == electrons->size() ) {
    log << MSG::INFO << "Failed to locate CaloHypos at "
        << m_electronPath << endreq;
  }
  else {
    log << MSG::DEBUG << "Successfully located " << electrons->size()
        << " CaloHypp at " << m_electronPath << endreq;
    caloData = true;
  }

  // Prepare output container
  ProtoParticles* chprotos = new ProtoParticles();
  StatusCode sc = eventSvc()->registerObject( m_protoPath, chprotos );
  if( !sc.isSuccess() ) {
    delete chprotos; 
    log << MSG::ERROR  
        << "Unable to register ProtoParticles container in " 
        << m_protoPath << endreq;
    return sc;
  }

  // ProtoParticles should only be "good tracks"
  //   keep only Forward, Upstream and Matched tracks, no clones 
  //   reject tracks with fit errors
  // Note that history does not accumulate:: it is only the nput container
  // and then a track is classifed as a clone but not respect to what
  int errcount = 0; 
  int uncount = 0; 
  int unvelo=0; int unforwr=0; int unmatch=0; int unupstr=0; int unseed=0;
  int chvelo=0; int chforwr=0; int chmatch=0; int chupstr=0; int chseed=0;

  int countTrackProto = 0;
  int countRichProto = 0;
  int countMuonProto = 0;
  TrStoredTracks::const_iterator iTrack;
  for( iTrack = tracks->begin(); tracks->end() != iTrack; ++iTrack ) {
    // Quality of tracks: for debugging only
    if( (*iTrack)->errorFlag() != 0 ) errcount++;
    if( (*iTrack)->unique() ) {
      uncount++;
      if( (*iTrack)->forward() )  unforwr++;
      if( (*iTrack)->match() )    unmatch++;
      if( (*iTrack)->upstream() ) unupstr++;
      if( (*iTrack)->velo() )     unvelo++;
      if( (*iTrack)->seed() )     unseed++;
      if( (*iTrack)->forward() && ((*iTrack)->charge() == 0) )  chforwr++;
      if( (*iTrack)->match() && ((*iTrack)->charge() == 0) )    chmatch++;
      if( (*iTrack)->upstream() && ((*iTrack)->charge() == 0) ) chupstr++;
      if( (*iTrack)->velo() && ((*iTrack)->charge() == 0) )     chvelo++;
      if( (*iTrack)->seed() && ((*iTrack)->charge() == 0) )     chseed++;
    }
    // Track satisfy criteria to make a ProtoParticle ?
    if( !keepTrack( (*iTrack) ) ) continue;
    
    countTrackProto++;
    //std::auto_ptr<ProtoParticle> proto( new ProtoParticle() ); 
    ProtoParticle* proto = new ProtoParticle();
    proto->setTrack( *iTrack );
    double tkcharge = proto->charge();
    /*double tkcharge = (*iTrack)->charge();
    if( 0 == tkcharge ) {
      const TrStateP* tkstate = proto->trStateP();
      if( tkstate ) {
        log << MSG::DEBUG << "tkstate q/p =" << tkstate->qDivP() << endreq;
        tkcharge = tkstate->qDivP() > 0.0 ? 1.0 : -1.0;
      }
      }*/
    log << MSG::DEBUG << "track charge = " << tkcharge << endreq;
    if( 0 == tkcharge ) continue;
    //proto->setCharge( tkcharge );

    ProtoParticle::PIDDetVector iddetvec;
    ProtoParticle::PIDInfoVector idinfovec;
    proto->setPIDDetectors(iddetvec);
    proto->setPIDInfo(idinfovec);

    // Add RichPID to this ProtoParticle
    if( richData ) {
      StatusCode sc = addRich( richpids, proto );
      if( !sc.isFailure() ) {
        countRichProto++;
      }  
    }
    
    // Add MuonID to this ProtoParticle
    if( muonData ) {
      MuonIDs::const_iterator iMuon;
      for( iMuon = muonpids->begin(); muonpids->end() != iMuon; ++iMuon ) {
        const TrStoredTrack* track = (*iMuon)->idTrack();
        if( track == (*iTrack) ) {
          countMuonProto++;
          proto->setMuonPID( *iMuon );
          if( (*iMuon)->InAcceptance() && (*iMuon)->PreSelMomentum() ) {
            ProtoParticle::PIDDetPair iddet;
            iddet.first = ProtoParticle::MuonMuon;
            iddet.second = (*iMuon)->MuProb();
            ProtoParticle::PIDDetVector& iddetvec = proto->pIDDetectors();
            iddetvec.push_back(iddet);
            proto->setMuonBit(1);
          }          
          // the muon pid for track has been added so exit from loop
          break;
        }
      } // break here
    }
    
    // Add CaloElectrons to this ProtoParticle
    /*
    if( caloData ) {
      CaloHypos::const_iterator iElec;
      int counte = 0;
      for( iElec = electrons->begin(); electrons->end() != iElec; ++iElec ) {
        SmartRefVector<CaloCluster>& eclusters = (*iElec)-> clusters();
        log << MSG::DEBUG << "Number of clusters for electron " 
            << ++counte << " = " << eclusters.size() << endreq;
      }
    } 
    */

    //chprotos->insert(proto.release());
    chprotos->insert(proto);
    
  }

  log << MSG::DEBUG << "Summary per event" << std::endl
      << "Tracks with errors  = " << errcount << std::endl
      << "Unique tracks       = " << uncount << std::endl
      << "         & forward  = " << unforwr << std::endl
      << "         & matched  = " << unmatch << std::endl
      << "         & upstream = " << unupstr << std::endl
      << "         & velo     = " << unvelo  << std::endl
      << "         & seed     = " << unseed  << endreq;
  log << "Tracks with charge = 0 " << std::endl
      << "         & forward  = " << chforwr << std::endl
      << "         & matched  = " << chmatch << std::endl
      << "         & upstream = " << chupstr << std::endl
      << "         & velo     = " << chvelo  << std::endl
      << "         & seed     = " << chseed  << endreq;

  
  log << MSG::DEBUG << "Found " << (unforwr+unmatch+unupstr)
      << " forward + upstream + matched unique tracks" << endreq;
  log << MSG::DEBUG << "Found " << countTrackProto
      << " tracks of quality to produce ProtoParticles" << endreq;
  log << MSG::DEBUG << "Made " << countRichProto 
      << " ProtoParticle with RichPID " << endreq;
  log << MSG::DEBUG << "Made " << countMuonProto 
      << " ProtoParticle with MuonID " << endreq;
  log << MSG::DEBUG << "Number of ProtoParticles in TES is " 
      << chprotos->size() << endreq;

  for( ProtoParticles::iterator ip = chprotos->begin(); chprotos->end() != ip;
       ++ip ) {
    log << MSG::VERBOSE << "track = " << (*ip)->track() << endreq;
    log << MSG::VERBOSE << "charge = " << (*ip)->charge() << endreq;
    log << MSG::VERBOSE << "richid = " << (*ip)->richPID() << endreq;
    log << MSG::VERBOSE << "muonid = " << (*ip)->muonPID() << endreq;
    log << MSG::VERBOSE << "richhistory " << (*ip)->richBit() << endreq;
    log << MSG::VERBOSE << "muonhistory " << (*ip)->muonBit() << endreq;
    log << MSG::VERBOSE << "bestPID " << (*ip)->bestPID() << endreq;
    for( ProtoParticle::PIDInfoVector::iterator id = (*ip)->pIDInfo().begin(); 
         (*ip)->pIDInfo().end()!=id; ++id ) {
      log << MSG::VERBOSE << "id = " << (*id).first << " , prob = " 
          << (*id).second  << endreq;
    }
    for( ProtoParticle::PIDDetVector::iterator idd = (*ip)->pIDDetectors().begin(); 
         (*ip)->pIDDetectors().end()!=idd; ++idd ) {
       log << MSG::VERBOSE << "det = " << (*idd).first << " , value = " 
          << (*idd).second  << endreq;
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoPAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}; 

//=============================================================================
//  keepTrack because of good quality
//=============================================================================
bool ChargedProtoPAlg::keepTrack( const TrStoredTrack* track ) {

  bool keep = false;
  if( 0 != track ) {
    if( 0 == track->errorFlag() ) {
      if( track->lastChiSq() <= m_lastChiSqMax ) {
        if( track->unique() && 
            (track->forward() || track->match() || track->upstream()) ) {
          keep = true;
        }
      }
    }
  }
  return keep;
};

//=============================================================================
// Add rich info to ProtoParticle
//=============================================================================
StatusCode ChargedProtoPAlg::addRich( SmartDataPtr<RichPIDs>& richpids, 
                                      ProtoParticle* proto) {
 
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::FAILURE;

  RichPIDs::const_iterator iRich;
  for( iRich = richpids->begin(); richpids->end() != iRich; ++iRich ) {
    const TrStoredTrack* track = (*iRich)->recTrack();
    if( track == proto->track() ) {
      sc = StatusCode::SUCCESS;
      proto->setRichPID( *iRich );

      // store Raw probabilities for RICH as detector info
      ProtoParticle::PIDDetVector& iddetvec = proto->pIDDetectors();
      ProtoParticle::PIDDetPair iddet;
      iddet.first = ProtoParticle::RichElectron;
      iddet.second = (*iRich)->particleRawProb(Rich::Electron);
      iddetvec.push_back(iddet);
      log << MSG::VERBOSE
          << "Rich " << Rich::Electron << " = " 
          << (*iRich)->particleRawProb(Rich::Electron)
          << endreq;
      
      iddet.first = ProtoParticle::RichMuon;
      iddet.second = (*iRich)->particleRawProb(Rich::Muon);
      iddetvec.push_back(iddet);
      log << MSG:: VERBOSE 
          << "Rich " << Rich::Muon << " = " 
          << (*iRich)->particleRawProb(Rich::Muon)
          << endreq;
      
      iddet.first = ProtoParticle::RichPion;
      iddet.second = (*iRich)->particleRawProb(Rich::Pion);
      iddetvec.push_back(iddet);
      log << MSG::VERBOSE
          << "Rich " << Rich::Pion
          << (*iRich)->particleRawProb(Rich::Pion)
          << endreq;
      
      iddet.first = ProtoParticle::RichKaon;
      iddet.second = (*iRich)->particleRawProb(Rich::Kaon);
      iddetvec.push_back(iddet);
      log << MSG::VERBOSE
          << "Rich " << Rich::Kaon
          << (*iRich)->particleRawProb(Rich::Kaon)
          << endreq;
      
          
      iddet.first = ProtoParticle::RichProton;
      iddet.second = (*iRich)->particleRawProb(Rich::Proton);
      iddetvec.push_back(iddet);
      log << MSG::VERBOSE 
          << "Rich " << Rich::Proton 
          << (*iRich)->particleRawProb(Rich::Proton)
          << endreq;
      
      proto->setRichBit(1);
        
      // store normalized probabilities for combined probabilities and
      // set history flag
      ProtoParticle::PIDInfoVector& idinfovec = proto->pIDInfo();
      ProtoParticle::PIDInfoPair idinfo;
      double richprob = (*iRich)->particleNormProb(Rich::Electron);
      if( richprob > 0.0 ) {  
        idinfo.first = m_idElectron * (int)proto->charge();
        idinfo.second = (*iRich)->particleNormProb(Rich::Electron);
        idinfovec.push_back(idinfo);
      }

      idinfo.first = m_idMuon * (int)proto->charge();
      idinfo.second = (*iRich)->particleNormProb(Rich::Muon);
      idinfovec.push_back(idinfo);
      
      idinfo.first = m_idPion * (int)proto->charge();
      idinfo.second = (*iRich)->particleNormProb(Rich::Pion);
      idinfovec.push_back(idinfo);
      
      idinfo.first = m_idKaon * (int)proto->charge();
      idinfo.second = (*iRich)->particleNormProb(Rich::Kaon);
      idinfovec.push_back(idinfo);
      
      idinfo.first = m_idProton * (int)proto->charge();
      idinfo.second = (*iRich)->particleNormProb(Rich::Proton);
      idinfovec.push_back(idinfo);
          
      proto->setRichCombined(1);
      break; // the rich pid for track has been added so exit from loop
    }
  } // break here
  
  return sc;
}

//=============================================================================
