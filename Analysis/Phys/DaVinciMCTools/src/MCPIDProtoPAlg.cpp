// $Id: MCPIDProtoPAlg.cpp,v 1.7 2002-10-14 09:59:57 gcorti Exp $
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
#include "MCPIDProtoPAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCPIDProtoPAlg
//
// 2002-07-08 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCPIDProtoPAlg>          s_factory ;
const        IAlgFactory& MCPIDProtoPAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCPIDProtoPAlg::MCPIDProtoPAlg( const std::string& name,
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
  , m_trackAsctName( "Track2MCParticleAsct" )
  , m_errorCount( )
  , m_monitor( false )
{

  // Inputs
  declareProperty("TrStoredTracksInput", 
                  m_tracksPath = TrStoredTrackLocation::Default );
  declareProperty("RichPIDsInput",
                  m_richPath = RichPIDLocation::Default );
  declareProperty("MuonPIDsInput",
                  m_muonPath = MuonIDLocation::Default );
  declareProperty("ElectronsInput",   m_electronPath );
  declareProperty("PhotonMatching",   m_photonMatchName );
  declareProperty("ElectronMatching", m_electronMatchName );
  declareProperty("BremMatching",     m_bremMatchName );
  
  // Outputs
  declareProperty("OutputData",
                  m_protoPath = ProtoParticleLocation::Charged );

  // Perfect PID
  declareProperty("TrackAsct", m_trackAsctName );
  
  // Selections
  declareProperty("UpstreamsTracks",  m_upstream );
  declareProperty("ITFracTrackClass", m_trackClassCut );
  declareProperty("Chi2NdFofITracks", m_chiSqOTracks );
  declareProperty("Chi2NdFofOTracks", m_chiSqITracks );
  
  // Monitor
  declareProperty("Monitor", m_monitor );
  
}

//=============================================================================
// Destructor
//=============================================================================
MCPIDProtoPAlg::~MCPIDProtoPAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCPIDProtoPAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialize" << endreq;

  StatusCode sc;
  IParticlePropertySvc* ppSvc;
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to locate Particle Property Service" 
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

  // Retrieve track associator
  sc = toolSvc()->retrieveTool(
       "AssociatorWeighted<TrStoredTrack,MCParticle,double>", 
       m_trackAsctName, m_track2MCParticleAsct);
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Tracks associator not found" << endreq;
    return sc;
  }

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

  if( m_monitor ) {
    std::string ntname = "/NTUPLES/FILE1";
    NTupleFilePtr ntfile(ntupleSvc(), ntname) ;
    if( ntfile ) {
      ntname += "/"+(this->name())+"/1";
      NTuplePtr ntmoni(ntupleSvc(), ntname);
      if( !ntmoni ) {
        ntmoni = ntupleSvc()->book(ntname,CLID_RowWiseTuple,"Monitor");
        if( ntmoni ) {
          StatusCode scnt = ntmoni->addItem("Ntracks", m_ntrk);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nunforw", m_nunforw);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nunmath", m_nunmatc);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nunupst", m_nunupst);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Ntkrej1", m_ntkrej1);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Ntkrej2", m_ntkrej2);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Ntkrej3", m_ntkrej3);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nrich", m_nrich);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nmuon", m_nmuon);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nelec", m_nelec);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Ntkpro", m_ntkpro);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nripro", m_nripro);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nmupro", m_nmupro);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nelpro", m_nelpro);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nprasc", m_naspro);
          if( scnt.isSuccess() ) scnt = ntmoni->addItem("Nproto", m_nproto);
          if( !scnt.isSuccess() ) {
            log << MSG::ERROR << "Not able to add items to ntuple" << endreq;
            return StatusCode::FAILURE;
          }
          m_ntuple = ntmoni;
        }
        else { 
          log << MSG::ERROR << "Not able to book ntuple " << ntname 
              << endreq;
          return StatusCode::FAILURE;
        }
      }
      else {
        log << MSG::ERROR << "Ntuple " << ntname << " already exists"
            << endreq;
        return StatusCode::FAILURE;
      }  
    }
    else {
      log << MSG::ERROR << "Monitor requested by ntuple file not set" 
          << endreq;
      return StatusCode::FAILURE;
    }
  }

  // Set up monitoring Counters 
  m_errorCount["1. No Tracks            "] = 0;
  m_errorCount["2. No Rich pID          "] = 0;
  m_errorCount["3. No Muon pID          "] = 0;
  m_errorCount["4. No Electron pID      "] = 0;
  m_errorCount["5. No photon table      "] = 0;
  m_errorCount["6. No electron table    "] = 0;
  m_errorCount["7. No brems table       "] = 0;
  m_errorCount["8. No Tracks for physics"] = 0;
  m_errorCount["9. No ProtoParticles    "] = 0;

  return StatusCode::SUCCESS;

};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCPIDProtoPAlg::execute() {

  MsgStream  log( msgSvc(), name() );

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
    m_errorCount["1. No Tracks            "] += 1;
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
    m_errorCount["2. No Rich pID          "] += 1;
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
    m_errorCount["3. No Muon pID          "] += 1;
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
    m_errorCount["4. No Electron pID      "] += 1;
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
    m_errorCount["5. No photon table      "] += 1;
  }
  const ElectronTable* etable = m_electronMatch->inverse();
  if( 0 == etable ) { 
    log << MSG::DEBUG << "Table from PhotonMatch points to NULL";
    caloData = false;
    m_errorCount["6. No electron table    "] += 1;
  }
  const BremTable* brtable = m_bremMatch->inverse();
  if( 0 == brtable ) { 
    log << MSG::DEBUG << "Table from PhotonMatch points to NULL";
    caloData = false;
    m_errorCount["7. No brems table       "] += 1;
  }

  // ProtoParticles should only be "good tracks"
  
  int countProto[5] = { 0, 0, 0, 0, 0 };
  int countRejTracks[4] = { 0, 0, 0, 0 };
  int countTypeTracks[4] = { 0, 0, 0, 0 };

  TrStoredTracks::const_iterator iTrack;
  for( iTrack = tracks->begin(); tracks->end() != iTrack; ++iTrack ) {
    
    // Does the track satisfies criteria to make a ProtoParticle ?
    if( (*iTrack)->unique() ) {
      if( (*iTrack)->forward() )  countTypeTracks[UniqueForward]++;
      if( (*iTrack)->match() )    countTypeTracks[UniqueMatch]++;
      if( (*iTrack)->upstream() ) countTypeTracks[UniqueUpstream]++;
    }
    
    int reject = rejectTrack( (*iTrack) );
    if( 0 != reject ) {
      countRejTracks[reject] += 1;
      continue;
    }
    
    ProtoParticle* proto = new ProtoParticle();
    proto->setTrack( *iTrack );
    double tkcharge = 0.0;
    int pidmc = 0;
    // Retrieve the MCParticle associated with this ProtoParticle
    MCParticle* mcPart = m_track2MCParticleAsct->associatedFrom( *iTrack );
    if( 0 != mcPart ) {
      countProto[AsctProto]++;
      pidmc = mcPart->particleID().pid();
      tkcharge = (mcPart->particleID().threeCharge())/3.0;
    }
    else {   
      const TrStateP* tkstate = proto->trStateP();
      if( tkstate ) {
        tkcharge = proto->charge();
        pidmc = m_idPion * (int)tkcharge;
      }
    }
    if( 0 == tkcharge ) {
      log << MSG::DEBUG << "track charge = " << tkcharge << endreq;
      delete proto;
      continue;
    }

    countProto[TrackProto]++;

    // Set combined particle ID as from MC truth when available
    ProtoParticle::PIDInfoVector idinfovec;
    ProtoParticle::PIDInfoPair idinfo;
    idinfo.first = pidmc;
    idinfo.second = 1.0;
    idinfovec.push_back(idinfo);
    proto->setPIDInfo(idinfovec);

    ProtoParticle::PIDDetVector iddetvec;
    proto->setPIDDetectors(iddetvec);

    // Add RichPID to this ProtoParticle
    if( richData ) {
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
          if( (*iMuon)->IsMuon() ) {
            countProto[MuonProto]++;
            ProtoParticle::PIDDetPair iddet;
            iddet.first = ProtoParticle::MuonMuon;
            iddet.second = (*iMuon)->MuProb();
            proto->pIDDetectors().push_back(iddet);
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
        ProtoParticle::PIDDetPair iddet;
        iddet.first = ProtoParticle::CaloEMatch;
        iddet.second = chi2;
        proto->pIDDetectors().push_back(iddet);
        proto->setCaloeBit(1);
       
        // Add the CaloCluster chi2 (only lowest)
        PhotonRange phrange = phtable->relations( *iTrack );
        if( !phrange.empty() ) {
          chi2 = phrange.begin()->weight();
          iddet.first = ProtoParticle::CaloTrMatch;
          iddet.second = chi2;
          proto->pIDDetectors().push_back(iddet);
        }
        
        // Add Brem hypothesis and chi2 (only lowest)
        BremRange brrange = brtable->relations( *iTrack );
        if( !brrange.empty() ) {
          hypo = brrange.begin()->to();
          proto -> addToCalo( hypo );
          chi2 = brrange.begin()->weight();
          iddet.first = ProtoParticle::BremMatch;
          iddet.second = chi2;
          proto->pIDDetectors().push_back(iddet);
        }
      }
    }
    // Check is at least one particleID is beeing added otherwise set NoPID
    if( (0 == proto->richBit()) && (0 == proto->muonBit()) &&
        (0 == proto->caloeBit()) ) {
      proto->setNoneBit(1);
      proto->pIDDetectors().
        push_back( std::make_pair(ProtoParticle::NoPID, 1.0) );      
    }

    chprotos->insert(proto);
    
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

  if( 0 == countProto[TrackProto] ) {
    m_errorCount["8. No Tracks for physics"] += 1;
  }
  if( 0 == chprotos->size() ) {
    m_errorCount["9. No ProtoParticles    "] += 1;
  }

  if( m_monitor ) {

    // Fill Ntuple
    m_ntrk = tracks->size();
    m_nunforw = countTypeTracks[UniqueForward];
    m_nunmatc = countTypeTracks[UniqueMatch];
    m_nunupst = countTypeTracks[UniqueUpstream];
    m_ntkrej1 = countRejTracks[NoTrack];
    m_ntkrej2 = countRejTracks[NoTrackType];
    m_ntkrej3 = countRejTracks[Chi2Cut];
    m_nrich = richpids->size();
    m_nmuon = muonpids->size();
    m_nelec = electrons->size();
    m_ntkpro = countProto[TrackProto];
    m_naspro = countProto[AsctProto];
    m_nripro = countProto[RichProto];
    m_nmupro = countProto[MuonProto];
    m_nelpro = countProto[ElectronProto];
    m_nproto = chprotos->size();
    
    m_ntuple->write();

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
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCPIDProtoPAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "********* ProtoParticles production Summary ******"
      << endreq;
  log << MSG::INFO << "Number of events with :" << endreq;

  for( ErrorTable::iterator ierr = m_errorCount.begin();
       ierr != m_errorCount.end(); ierr++ ) { 
    log << MSG::INFO << "   " << (*ierr).first 
        << "  " << format("%9u", (*ierr).second ) 
        << endreq;
  }
  log << MSG::INFO << "**************************************************"
      << endreq;
  
  return StatusCode::SUCCESS;
}; 

//=============================================================================
// rejectTrack because of poor quality
//   keep only Forward, Upstream and Matched tracks, no clones 
//   reject tracks with fit errors
// Note that history does not accumulate:: it is only the original container
// and then a track is classifed as a clone but not respect to what
//=============================================================================
int MCPIDProtoPAlg::rejectTrack( const TrStoredTrack* track ) {

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
StatusCode MCPIDProtoPAlg::addRich( SmartDataPtr<RichPIDs>& richpids, 
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
      log << MSG::VERBOSE
          << "Rich " << Rich::Electron << " = " 
          << (*iRich)->particleRawProb(Rich::Electron)
          << endreq;
      
      iddet.first = ProtoParticle::RichMuon;
      iddet.second = (*iRich)->particleRawProb(Rich::Muon);
      proto->pIDDetectors().push_back(iddet);
      log << MSG:: VERBOSE 
          << "Rich " << Rich::Muon << " = " 
          << (*iRich)->particleRawProb(Rich::Muon)
          << endreq;
      
      iddet.first = ProtoParticle::RichPion;
      iddet.second = (*iRich)->particleRawProb(Rich::Pion);
      proto->pIDDetectors().push_back(iddet);
      log << MSG::VERBOSE
          << "Rich " << Rich::Pion << " = "
          << (*iRich)->particleRawProb(Rich::Pion)
          << endreq;
      
      iddet.first = ProtoParticle::RichKaon;
      iddet.second = (*iRich)->particleRawProb(Rich::Kaon);
      proto->pIDDetectors().push_back(iddet);
      log << MSG::VERBOSE
          << "Rich " << Rich::Kaon << " = "
          << (*iRich)->particleRawProb(Rich::Kaon)
          << endreq;
      
          
      iddet.first = ProtoParticle::RichProton;
      iddet.second = (*iRich)->particleRawProb(Rich::Proton);
      proto->pIDDetectors().push_back(iddet);
      log << MSG::VERBOSE 
          << "Rich " << Rich::Proton << " = "
          << (*iRich)->particleRawProb(Rich::Proton)
          << endreq;
      
      proto->setRichBit(1);
        
      break; // the rich pid for track has been added so exit from loop
    }
  } // break here
  
  return sc;
}

//=============================================================================
