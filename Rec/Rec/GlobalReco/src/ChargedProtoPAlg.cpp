// $Id: ChargedProtoPAlg.cpp,v 1.1.1.1 2002-07-10 16:53:05 gcorti Exp $
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
  : Algorithm ( name , pSvcLocator ) {

  declareProperty("TrStoredTracksInput", 
                  m_tracksPath = TrStoredTrackLocation::Default );
  declareProperty("RichPIDsInput",
                  m_richPath = RichPIDLocation::Default );
  declareProperty("MuonPIDsInput",
                  m_muonPath = MuonIDLocation::Default );
  declareProperty("ElectronsInput",
                  m_electronPath = "Rec/Calo/Electrons" );
  declareProperty("ProtoPsOutput",
                  m_protoPath = ProtoParticleLocation::Charged );
  declareProperty("MaxChiSquare", m_lastChiSqMax = 100 );

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

  partProp = ppSvc->find( "e-" );
  m_idElectron = abs(partProp->jetsetID());

  partProp = ppSvc->find( "mu-" );
  m_idMuon = abs(partProp->jetsetID());

  partProp = ppSvc->find( "pi+" );
  m_idPion = abs(partProp->jetsetID());

  partProp = ppSvc->find( "K+" );
  m_idKaon = abs(partProp->jetsetID());

  partProp = ppSvc->find( "p+" );
  m_idProton = abs(partProp->jetsetID());

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
  int countTrackProto = 0;
  TrStoredTracks::const_iterator iTrack;
  int errcount = 0; 
  int uncount = 0; 
  int veloonly = 0;
  int unvelo=0; int unforwr=0; int unmatch=0; int unupstr=0; int unseed=0;
  int velo = 0; int forwr = 0; int match = 0; int upstr = 0; int seed = 0;

  for( iTrack = tracks->begin(); tracks->end() != iTrack; ++iTrack ) {
    if( keepTrack( (*iTrack) ) ) countTrackProto++;
    if( (*iTrack)->errorFlag() != 0 ) errcount++;
    if( (*iTrack)->unique() ) {
      uncount++;
      if( (*iTrack)->forward() )  unforwr++;
      if( (*iTrack)->match() )    unmatch++;
      if( (*iTrack)->upstream() ) unupstr++;
      if( (*iTrack)->velo() )     unvelo++;
      if( (*iTrack)->seed() )     unseed++;
    }
    else {
      
      if( (*iTrack)->forward() )  forwr++;
      if( (*iTrack)->match() )    match++;
      if( (*iTrack)->upstream() ) upstr++;
      if( (*iTrack)->velo() )     velo++;
      if( (*iTrack)->seed() )     seed++;
       }
    if( (*iTrack)->velo() ) {
      if( !(*iTrack)->forward() && !(*iTrack)->match() && 
          !(*iTrack)->upstream() ) {
        veloonly++;
      }
    }
  }

  log << MSG::DEBUG << "Found " << countTrackProto
      << " tracks of quality to produce ProtoParticles" << endreq;

  log << MSG::DEBUG << "Summary per event" << std::endl
      << "Tracks with errors  = " << errcount << std::endl
      << "Purely velo tracks  = " << veloonly << std::endl
      << "Unique tracks       = " << uncount << std::endl
      << "         & forward  = " << unforwr << std::endl
      << "         & matched  = " << unmatch << std::endl
      << "         & upstream = " << unupstr << std::endl
      << "         & velo     = " << unvelo  << std::endl
      << "         & seed     = " << unseed  << std::endl
      << "Clone tracks         " << std::endl
      << "         & forward  = " << forwr << std::endl
      << "         & matched  = " << match << std::endl
      << "         & upstream = " << upstr << std::endl
      << "         & velo     = " << velo  << std::endl
      << "         & seed     = " << seed  << endreq;


  // Load richPid results
  int countRichProto = 0;
  SmartDataPtr<RichPIDs> richpids ( eventSvc(), m_richPath );
  if( !richpids || 0 == richpids->size() ) {
    log << MSG::ERROR << "Failed to locate RichPIDs at "
        << m_richPath << endreq;
  }
  else {   
    log << MSG::DEBUG << "Successfully located " << richpids->size()
        << " RichPIDs at " << m_richPath << endreq;

    RichPIDs::const_iterator iRich;
    for( iRich = richpids->begin(); richpids->end() != iRich; ++iRich ) {
      // keep only RichpIDs for good tracks at this point
      const SmartRef<TrStoredTrack> track = (*iRich)->recTrack();
      log << MSG::DEBUG << "track from rich = " << track << endreq;
      if( 0 != track ) {
        if( keepTrack( track ) ) {
        // create a new ProtoParticle and add it to the container
        std::auto_ptr<ProtoParticle> proto( new ProtoParticle() ); 
        proto->setTrack( track );
        proto->setRichPID( *iRich );
        proto->setCharge( track->charge() );

        // store Raw probabilities for RICH as detector info
        ProtoParticle::PIDDetVector iddetvec;
        std::pair<int,double> iddet;
        iddet.first = ProtoParticle::RichElectron;
        iddet.second = (*iRich)->particleRawProb(Rich::Electron);
        iddetvec.push_back(iddet);

        iddet.first = ProtoParticle::RichMuon;
        iddet.second = (*iRich)->particleRawProb(Rich::Muon);
        iddetvec.push_back(iddet);
        
        iddet.first = ProtoParticle::RichPion;
        iddet.second = (*iRich)->particleRawProb(Rich::Pion);
        iddetvec.push_back(iddet);
        
        iddet.first = ProtoParticle::RichKaon;
        iddet.second = (*iRich)->particleRawProb(Rich::Kaon);
        iddetvec.push_back(iddet);
        
        iddet.first = ProtoParticle::RichProton;
        iddet.second = (*iRich)->particleRawProb(Rich::Proton);
        iddetvec.push_back(iddet);
        
        proto->setPIDDetectors(iddetvec);

        // store normalized probabilities for combined probabilities and
        // set history flag
        ProtoParticle::PIDInfoVector idinfovec;
        std::pair<int,double> idinfo;
        idinfo.first = m_idElectron * (int)proto->charge();
        idinfo.second = (*iRich)->particleNormProb(Rich::Electron);
        idinfovec.push_back(idinfo);

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

        proto->setPIDInfo(idinfovec);
        proto->setRichBit(1);
        
        chprotos->insert(proto.release());
        countRichProto++;
        }
      } 
    }
  }
  
  log << MSG::DEBUG << "Made " << countRichProto 
      << " ProtoParticle with RichPID " << endreq;
  log << MSG::DEBUG << "Number of ProtoParticles in TES is " 
      << chprotos->size() << endreq;

  // Load muonPid results
  SmartDataPtr<MuonIDs> muonpids ( eventSvc(), m_muonPath );
  if( !muonpids || 0 == muonpids->size() ) {
    log << MSG::ERROR << "Failed to locate MuonIDs at "
        << m_muonPath << endreq;
  }
  else {
    log << MSG::DEBUG << "Successfully located " << muonpids->size()
        << " MuonIDs at " << m_muonPath << endreq;
    
    MuonIDs::const_iterator iMuon;
    for( iMuon = muonpids->begin(); muonpids->end() != iMuon; ++iMuon ) {
      const TrStoredTrack* track = (*iMuon)->idTrack();
      log << MSG::DEBUG << " Muon track = " << track << endreq;
    }    
  }
 
  // Load Electron results
  SmartDataPtr<CaloHypos> electrons ( eventSvc(), m_electronPath );
  if( !electrons || 0 == electrons->size() ) {
    log << MSG::ERROR << "Failed to locate CaloHypos at "
        << m_electronPath << endreq;
  }
  else {
    log << MSG::DEBUG << "Successfully located " << electrons->size()
        << " CaloHypp at " << m_electronPath << endreq;
    
    CaloHypos::const_iterator iElec;
    int counte = 0;
    for( iElec = electrons->begin(); electrons->end() != iElec; ++iElec ) {
      SmartRefVector<CaloCluster>& eclusters = (*iElec)-> clusters();
      log << MSG::DEBUG << "Number of clusters for electron " 
          << ++counte << " = " << eclusters.size() << endreq;
    }
  }
  
  
  SmartRefVector<CaloCluster>& clusters();

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoPAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  GoodTrack
//=============================================================================
bool ChargedProtoPAlg::keepTrack( const TrStoredTrack* track ) {

  bool keep = false;
  if( 0 == track->errorFlag() ) {
    if( track->lastChiSq() <= m_lastChiSqMax ) {
      if( track->unique() && 
          (track->forward() || track->match() || track->upstream()) ) {
        keep = true;
      }
    }
  }
  return keep;
}


//=============================================================================
