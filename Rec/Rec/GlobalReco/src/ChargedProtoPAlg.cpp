// $Id: ChargedProtoPAlg.cpp,v 1.7 2002-09-09 17:16:33 gcorti Exp $
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
#include "Event/ITClusterOnStoredTrack.h"
#include "Event/OTClusterOnStoredTrack.h"

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
  , m_photonMatchName( "PhotonMatch" )
  , m_electronMatchName( "ElectronMatch" )
  , m_bremMatchName( "BremMatch" )
  , m_upstream( false )
  , m_trackClassCut( 0.4 )
  , m_chiSqITracks( 500.0 )
  , m_chiSqOTracks( 100.0 )
  , m_errorCount( )
//    , m_monitor( false )
{
  
  // Inputs
  declareProperty("TrStoredTracksInput", 
                  m_tracksPath = TrStoredTrackLocation::Default );
  declareProperty("RichPIDsInput",
                  m_richPath = RichPIDLocation::Default );
  declareProperty("MuonPIDsInput",
                  m_muonPath = MuonIDLocation::Default );
  declareProperty("ElectronsInput",    m_electronPath );
  declareProperty("PhotonMatching",    m_photonMatchName );
  declareProperty("ElectronMatching",  m_electronMatchName );
  declareProperty("BremMatching",      m_bremMatchName );
  
  // Outputs
  declareProperty("OutputData",
                  m_protoPath = ProtoParticleLocation::Charged );

  // Selections
  declareProperty("UpstreamsTracks",  m_upstream );
  declareProperty("ITFracTrackClass", m_trackClassCut );
  declareProperty("Chi2NdFofITracks", m_chiSqOTracks );
  declareProperty("Chi2NdFofOTracks", m_chiSqITracks );
  
  // Monitor
//    declareProperty("Monitor", m_monitor );

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
    log << MSG::FATAL << "Unable to locate Particle Property Service" 
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

  // Associators for CaloTables
  std::string matchType="AssociatorWeighted<CaloCluster,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_photonMatchName, m_photonMatch );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Unable to retrieve " << matchType << endreq;
    return sc;
  }
  matchType = "AssociatorWeighted<CaloHypo,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_electronMatchName,
                                m_electronMatch );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Unable to retrieve " << matchType << endreq;
    return sc;
  }
  matchType = "AssociatorWeighted<CaloHypo,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_bremMatchName, m_bremMatch );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Unable to retrieve " << matchType << endreq;
    return sc;
  }


  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoPAlg::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

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

  // Load stored tracks
  SmartDataPtr<TrStoredTracks> tracks ( eventSvc(), m_tracksPath );
  if( !tracks || 0 == tracks->size() ) {
    log << MSG::INFO << "Unable to retrieve TrStoredTracks at "
        << m_tracksPath << endreq;
    m_errorCount["No Tracks"] += 1;
    return StatusCode::SUCCESS;
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
    m_errorCount["No Rich pID"] += 1;
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
    m_errorCount["No Muon pID"] += 1;
  }
  else {
    log << MSG::DEBUG << "Successfully located " << muonpids->size()
        << " MuonIDs at " << m_muonPath << endreq;
    muonData = true;
  }

  // Load Electron results and tables
  bool caloData = false;
  SmartDataPtr<CaloHypos> electrons ( eventSvc(), m_electronPath );
  if( !electrons || 0 == electrons->size() ) {
    log << MSG::INFO << "Failed to locate CaloHypos at "
        << m_electronPath << endreq;
    m_errorCount["No electron pID"] += 1;
  }
  else {
    log << MSG::DEBUG << "Successfully located " << electrons->size()
        << " CaloHypo at " << m_electronPath << endreq;
    caloData = true;
  }

  /// Check the tables for electronID
  const PhotonTable* phtable = m_photonMatch->inverse();
  if( 0 == phtable ) { 
    log << MSG::DEBUG << "Table from PhotonMatch points to NULL";
    caloData = false;
    m_errorCount["No photon table"] += 1;
  }
  const ElectronTable* etable = m_electronMatch->inverse();
  if( 0 == etable ) { 
    log << MSG::DEBUG << "Table from PhotonMatch points to NULL";
    caloData = false;
    m_errorCount["No electron table"] += 1;
  }
  const BremTable* brtable = m_bremMatch->inverse();
  if( 0 == brtable ) { 
    log << MSG::DEBUG << "Table from PhotonMatch points to NULL";
    caloData = false;
    m_errorCount["No brems table"] += 1;
  }

  // ProtoParticles should only be "good tracks"

  int countProto[4] = { 0, 0, 0, 0 };
  int countRejTracks[4] = { 0, 0, 0, 0 };
  
  TrStoredTracks::const_iterator iTrack;
  for( iTrack = tracks->begin(); tracks->end() != iTrack; ++iTrack ) {

    // Does the track satisfies criteria to make a ProtoParticle ?
    int reject = rejectTrack( (*iTrack) );
    if( 0 != reject ) {
      countRejTracks[reject] += 1;
      continue;
    }

    ProtoParticle* proto = new ProtoParticle();
//      std::auto_ptr<ProtoParticle> proto( new ProtoParticle() );
    proto->setTrack( *iTrack );
    double tkcharge = proto->charge();
    if( 0 == tkcharge ) {
      log << MSG::DEBUG << "track charge = " << tkcharge << endreq;
      delete proto;
      continue;
    }

    countProto[TrackProto]++;
    
    //ProtoParticle::PIDDetVector iddetvec;
    //ProtoParticle::PIDInfoVector idinfovec;
    //proto->setPIDDetectors(iddetvec);
    //proto->setPIDInfo(idinfovec);

    // Add RichPID to this ProtoParticle
    if( richData ) {
//        StatusCode sc = addRich( richpids, proto->get() );
      StatusCode sc = addRich( richpids, proto );
      if( !sc.isFailure() ) {
        countProto[RichProto]++;
      }
    }
    
    // Add MuonID to this ProtoParticle
    if( muonData ) {
      MuonIDs::const_iterator iMuon;
      for( iMuon = muonpids->begin(); muonpids->end() != iMuon; ++iMuon ) {
        const TrStoredTrack* track = (*iMuon)->idTrack();
        if( track == (*iTrack) ) {
          proto->setMuonPID( *iMuon );
          // When MuonDet is capable of identifying it add its result
//            if( (*iMuon)->InAcceptance() && (*iMuon)->PreSelMomentum() ) {
          if( (*iMuon)->IsMuon() ) {
            countProto[MuonProto]++;
            double muonProb = (*iMuon)->MuProb();
            proto->pIDDetectors().
              push_back( std::make_pair(ProtoParticle::MuonMuon, muonProb) );
            proto->setMuonBit(1);
          }
          // The muon pid for track has been added so exit from loop
          break;
        }
      } // break here
    }
    
    // Add CaloElectrons to this ProtoParticle   
    if( caloData ) {  
      // Add the Electron hypothesis when available (no cuts at the moment)
      ElectronRange erange = etable->relations( *iTrack );
      if( !erange.empty() ) {
        countProto[ElectronProto]++;
        CaloHypo* hypo = erange.begin()->to();
        proto->addToCalo( hypo );

        double chi2 = erange.begin()->weight();
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::CaloEMatch, chi2) );
        proto->setCaloeBit(1);
       
        // Add the CaloCluster chi2 (only lowest)
        PhotonRange phrange = phtable->relations( *iTrack );
        if( !phrange.empty() ) {
          chi2 = phrange.begin()->weight();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::CaloTrMatch, chi2) );
        }
        
        // Add Brem hypothesis and chi2 (only lowest)
        BremRange brrange = brtable->relations( *iTrack );
        if( !brrange.empty() ) {
          hypo = brrange.begin()->to();
          proto -> addToCalo( hypo );
          chi2 = brrange.begin()->weight();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::BremMatch, chi2) );
        }
      }
    }

    // Check is at least one particleID is beeing added otherwise set NoPID
    // and assume it is a pion
    if( (0 == proto->richBit()) && (0 == proto->muonBit()) &&
        (0 == proto->caloeBit()) ) {
      proto->pIDDetectors().
        push_back( std::make_pair(ProtoParticle::NoPID, 1.0) );
      int idpion = m_idPion * (int)proto->charge();
      proto->pIDInfo().push_back( std::make_pair( idpion, 1.0 ) );
      proto->setNoneBit(1);
    }

    // If RichPID is not available since it is takes as combined, set NoneBit
    // and assume pion (keep it separate because it could change and we could
    // decide to use muon or calorimeter alone)
    if( 0 == proto->richBit() ) {
      int idpion = m_idPion * (int)proto->charge();
      proto->pIDInfo().push_back( std::make_pair( idpion, 1.0 ) );
      proto->setNoneBit(1);
    }
    
    chprotos->insert(proto);
//        chprotos->insert(proto->release());
    
  }
  
    
  log << MSG::DEBUG << "Found " << countProto[TrackProto]
      << " tracks of quality to produce ProtoParticles" << endreq;
  log << MSG::DEBUG << "Made " << countProto[RichProto] 
      << " ProtoParticle with RichPID " << endreq;
  log << MSG::DEBUG << "Made " << countProto[MuonProto]
      << " ProtoParticle with MuonID " << endreq;
  log << MSG::DEBUG << "Made " << countProto[ElectronProto]
      << " ProtoParticle with ElectronHypo " << endreq;
  log << MSG::DEBUG << "Number of ProtoParticles in TES is " 
      << chprotos->size() << endreq;

//    if( m_monitor ) {
    for( ProtoParticles::iterator ip = chprotos->begin();
         chprotos->end() != ip; ++ip ) {
      log << MSG::VERBOSE << "track = " << (*ip)->track() << endreq;
      log << MSG::VERBOSE << "charge = " << (*ip)->charge() << endreq;
      log << MSG::VERBOSE << "richid = " << (*ip)->richPID() << endreq;
      log << MSG::VERBOSE << "muonid = " << (*ip)->muonPID() << endreq;
      log << MSG::VERBOSE << "richhistory = " << (*ip)->richBit() << endreq;
      log << MSG::VERBOSE << "muonhistory = " << (*ip)->muonBit() << endreq;
      log << MSG::VERBOSE << "bestPID = " << (*ip)->bestPID() << endreq;
      for(ProtoParticle::PIDInfoVector::iterator id = (*ip)->pIDInfo().begin();
          (*ip)->pIDInfo().end()!=id; ++id ) {
        log << MSG::VERBOSE << "id = " << (*id).first << " , prob = " 
            << (*id).second  << endreq;
      }
      for( ProtoParticle::PIDDetVector::iterator 
             idd = (*ip)->pIDDetectors().begin(); 
           (*ip)->pIDDetectors().end()!=idd; ++idd ) {
        log << MSG::VERBOSE << "det = " << (*idd).first << " , value = " 
            << (*idd).second  << endreq;
      }
    }

//    } // end monitor

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoPAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  for( ErrorTable::iterator ierr = m_errorCount.begin();
       ierr != m_errorCount.end(); ierr++ ) { 
    log << MSG::INFO 
        << "Number of events with " << (*ierr).first 
        << " = " << (*ierr).second 
        << endreq;
  }
  return StatusCode::SUCCESS;
}; 

//=============================================================================
// rejectTrack because of poor quality
//   keep only Forward, Upstream and Matched tracks, no clones 
//   reject tracks with fit errors
// Note that history does not accumulate:: it is only the original container
// and then a track is classifed as a clone but not respect to what
//=============================================================================
int ChargedProtoPAlg::rejectTrack( const TrStoredTrack* track ) {

  int reject = NoTrack;
  if( NULL != track ) {
    if( 0 == track->errorFlag() ) {
      if( track->unique() && (track->forward() || track->match()) ) {
        reject = KeepTrack;
      }
      if( m_upstream && (track->unique() && track->upstream()) ) {
        reject = KeepTrack; 
      }
    }
    else {
      reject = NoTrackType;
    }
  }
  
  if( !reject ) {
    int nIT = 0;
    int nOT = 0;
    SmartRefVector<TrStoredMeasurement>::const_iterator iterMeas = 
      track->measurements().begin();
    while( iterMeas != track->measurements().end() ) {
      const TrStoredMeasurement* aMeas = *iterMeas;
      if( dynamic_cast<const ITClusterOnStoredTrack*>(aMeas) ) {
        nIT++;
      }
      if( dynamic_cast<const OTClusterOnStoredTrack*>(aMeas) ) {
        nOT++;
      }
      iterMeas++;
    }
    int nTrackerMeas = nIT + nOT;
    int nTotMeas = track->measurements().size();
    double fracIT = (double)nIT/(double)nTrackerMeas;
    double cutValue = 0.0;
    if( fracIT > m_trackClassCut ) {
      cutValue = m_chiSqITracks;
    } else {
      cutValue = m_chiSqOTracks;
    }
    
    double chi2NoF = (track->lastChiSq())/((double)nTotMeas - 5);
    if( chi2NoF >= cutValue ) {
      reject = Chi2Cut;
    }
  }
  return reject;
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
      ProtoParticle::PIDDetPair iddet;
      iddet.first = ProtoParticle::RichElectron;
      iddet.second = (*iRich)->particleRawProb(Rich::Electron);
      proto->pIDDetectors().push_back(iddet);
      
      iddet.first = ProtoParticle::RichMuon;
      iddet.second = (*iRich)->particleRawProb(Rich::Muon);
      proto->pIDDetectors().push_back(iddet);
      
      iddet.first = ProtoParticle::RichPion;
      iddet.second = (*iRich)->particleRawProb(Rich::Pion);
      proto->pIDDetectors().push_back(iddet);
      
      iddet.first = ProtoParticle::RichKaon;
      iddet.second = (*iRich)->particleRawProb(Rich::Kaon);
      proto->pIDDetectors().push_back(iddet);
          
      iddet.first = ProtoParticle::RichProton;
      iddet.second = (*iRich)->particleRawProb(Rich::Proton);
      proto->pIDDetectors().push_back(iddet);

      proto->setRichBit(1);

//        if( m_monitor ) {
      log << MSG::VERBOSE << "Rich " 
          << Rich::Electron << " = " 
          << (*iRich)->particleRawProb(Rich::Electron) 
          << endreq;
      log << MSG:: VERBOSE << "Rich " 
          << Rich::Muon << " = " << (*iRich)->particleRawProb(Rich::Muon)
          << endreq;
      log << MSG::VERBOSE << "Rich " 
          << Rich::Pion << " = " << (*iRich)->particleRawProb(Rich::Pion)
          << endreq;
      log << MSG::VERBOSE << "Rich " 
          << Rich::Kaon << " = " << (*iRich)->particleRawProb(Rich::Kaon)
          << endreq;
      log << MSG::VERBOSE << "Rich " 
          << Rich::Proton << " = " << (*iRich)->particleRawProb(Rich::Proton)
          << endreq;
//        }

      // store normalized probabilities for combined probabilities and
      // set history flag
      ProtoParticle::PIDInfoPair idinfo;
      double richProb = (*iRich)->particleNormProb(Rich::Electron);
      if( richProb > 0.0 ) {
        idinfo.first  = m_idElectron * (int)proto->charge();
        idinfo.second = richProb;
        proto->pIDInfo().push_back(idinfo);
      }
      
      richProb = (*iRich)->particleNormProb(Rich::Muon);
      if( richProb > 0.0 ) {
        idinfo.first  = m_idMuon * (int)proto->charge();
        idinfo.second = richProb;
        proto->pIDInfo().push_back(idinfo);
      }

      richProb = (*iRich)->particleNormProb(Rich::Pion);
      if( richProb > 0.0 ) {
        idinfo.first = m_idPion * (int)proto->charge();
        idinfo.second = richProb;
        proto->pIDInfo().push_back(idinfo);
      }
      
      richProb = (*iRich)->particleNormProb(Rich::Kaon);
      if( richProb > 0.0 ) {
        idinfo.first = m_idKaon * (int)proto->charge();
        idinfo.second = richProb;
        proto->pIDInfo().push_back(idinfo);
      }
      
      richProb = (*iRich)->particleNormProb(Rich::Proton);
      if( richProb > 0.0 ) {
        idinfo.first = m_idProton * (int)proto->charge();
        idinfo.second = richProb;
        proto->pIDInfo().push_back(idinfo);
      }

      proto->setRichCombined(1);
      break; // the rich pid for track has been added so exit from loop
    }
  } // break here
  
  return sc;
}

//=============================================================================
