// $Id: ChargedProtoPAlg.cpp,v 1.14 2003-06-18 12:25:05 gcorti Exp $
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
  , m_dLLeEcalName ( "EcalPIDe" )
  , m_dLLePrsName  ( "PrsPIDe" )
  , m_dLLeBremName ( "BremPIDe" )
  , m_dLLeHcalName ( "HcalPIDe" )
  , m_dLLmuEcalName( "EcalPIDmu" )
  , m_dLLmuHcalName( "HcalPIDmu" )
  , m_upstream( false )
  , m_velott( false )
  , m_trackClassCut( 0.4 )
  , m_chiSqITracks( 500.0 )
  , m_chiSqOTracks( 100.0 )
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
  declareProperty("ElectronsInput",    m_electronPath );
  declareProperty("PhotonMatching",    m_photonMatchName );
  declareProperty("ElectronMatching",  m_electronMatchName );
  declareProperty("BremMatching",      m_bremMatchName );
  declareProperty("dLLeEcal",          m_dLLeEcalName );
  declareProperty("dLLePrs",           m_dLLePrsName );
  declareProperty("dLLeBrem",          m_dLLeBremName );
  declareProperty("dLLeHcal",          m_dLLeHcalName );
  declareProperty("dLLmuEcal",         m_dLLmuEcalName );
  declareProperty("dLLmuHcal",         m_dLLmuHcalName );
  
  // Outputs
  declareProperty("OutputData",
                  m_protoPath = ProtoParticleLocation::Charged );

  // Selections
  declareProperty("UpstreamsTracks",  m_upstream );
  declareProperty("VeloTTTracks",     m_velott );
  declareProperty("ITFracTrackClass", m_trackClassCut );
  declareProperty("Chi2NdFofITracks", m_chiSqOTracks );
  declareProperty("Chi2NdFofOTracks", m_chiSqITracks );
  
  // Monitor
  declareProperty("Monitor", m_monitor );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoPAlg::~ChargedProtoPAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ChargedProtoPAlg::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc;
  IParticlePropertySvc* ppSvc;
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to locate Particle Property Service" 
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
  sc = caloPIDTools();
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve Calo PIDs Tools" << endreq;
    return sc;
  }

  // MuonID Delta Log Likelihood tool
  sc = toolSvc()->retrieveTool( "MuonIDDLLTool",  m_muonIDdll );
  if (sc.isFailure()){
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "Unable to retrieve MuonIDDLLTool " << endreq; 
    return sc;
  }

  // Set up monitoring Counters 
  m_errorCount["1. No Tracks            "] = 0;
  m_errorCount["2. No Rich pID          "] = 0;
  m_errorCount["3. No Muon pID          "] = 0;
  m_errorCount["4. No Electron pID      "] = 0;
  m_errorCount["5. No photon table      "] = 0;
  m_errorCount["6. No electron table    "] = 0;
  m_errorCount["7. No brems table       "] = 0;
  m_errorCount["7a.No EcalPIDe table    "] = 0;
  m_errorCount["7b.No PrsPIDe  table    "] = 0;
  m_errorCount["7c.No BremPIDe table    "] = 0;
  m_errorCount["7d.No HcalPIDe table    "] = 0;
  m_errorCount["7e.No EcalPIDmu table   "] = 0;
  m_errorCount["7f.No HcalPIDmu table   "] = 0;
  m_errorCount["8. No Tracks for physics"] = 0;
  m_errorCount["9. No ProtoParticles    "] = 0;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoPAlg::execute() {

  MsgStream  msg( msgSvc(), name() );

  // Prepare output container
  ProtoParticles* chprotos = new ProtoParticles();
  StatusCode sc = eventSvc()->registerObject( m_protoPath, chprotos );
  if( !sc.isSuccess() ) {
    delete chprotos; 
    msg << MSG::ERROR  
        << "Unable to register ProtoParticles container in " 
        << m_protoPath << endreq;
    return sc;
  }

  // Load stored tracks
  SmartDataPtr<TrStoredTracks> tracks ( eventSvc(), m_tracksPath );
  if( !tracks || 0 == tracks->size() ) {
    msg << MSG::INFO << "Unable to retrieve any TrStoredTracks at "
        << m_tracksPath << endreq;
    m_errorCount["1. No Tracks            "] += 1;
    return StatusCode::SUCCESS;
  }
  else {
    if( m_monitor ) {
      msg << MSG::DEBUG << "Successfully retrieved " << tracks->size()
          << " TrStoredTracks at " << m_tracksPath << endreq;
    }
  }

  // Load richPid results
  bool richData = false;
  SmartDataPtr<RichPIDs> richpids ( eventSvc(), m_richPath );
  if( !richpids || 0 == richpids->size() ) {
    msg << MSG::INFO  << "Failed to retrieve any RichPIDs at "
        << m_richPath << endreq;
    m_errorCount["2. No Rich pID          "] += 1;
  }
  else {   
    richData = true;
    if( m_monitor ) {
      msg << MSG::DEBUG << "Successfully retrieved " << richpids->size()
          << " RichPIDs at " << m_richPath << endreq;
    }
  }
  
  // Load muonPid results
  bool muonData = false;
  SmartDataPtr<MuonIDs> muonpids ( eventSvc(), m_muonPath );
  if( !muonpids || 0 == muonpids->size() ) {
    msg << MSG::INFO << "Failed to retrieve any MuonIDs at "
        << m_muonPath << endreq;
    m_errorCount["3. No Muon pID          "] += 1;
  }
  else {
   muonData = true;
    if ( m_monitor ){
      msg << MSG::DEBUG << "Successfully retrieved " << muonpids->size()
          << " MuonIDs at " << m_muonPath << endreq;
    }
  }  

  // Load Electron results and tables
  bool caloData = false;
  SmartDataPtr<CaloHypos> electrons ( eventSvc(), m_electronPath );
  if( !electrons || 0 == electrons->size() ) {
    msg << MSG::INFO << "Failed to retrieve any CaloHypos at "
        << m_electronPath << endreq;
    m_errorCount["4. No Electron pID      "] += 1;
  }
  else {
    caloData = true;
    if ( m_monitor ) {
      msg << MSG::DEBUG << "Successfully retrieved " << electrons->size()
          << " CaloHypo at " << m_electronPath << endreq;
    } 
  }

  /// Check the tables for electronID exist
  const PhotonTable* phtable = m_photonMatch->inverse();
  if( 0 == phtable ) { 
    msg << MSG::DEBUG << "Table from PhotonMatch points to NULL" << endreq;
    caloData = false;
    m_errorCount["5. No photon table      "] += 1;
  }
  const ElectronTable* etable = m_electronMatch->inverse();
  if( 0 == etable ) { 
    msg << MSG::DEBUG << "Table from ElectronMatch points to NULL" << endreq;
    caloData = false;
    m_errorCount["6. No electron table    "] += 1;
  }
  const BremTable* brtable = m_bremMatch->inverse();
  if( 0 == brtable ) { 
    msg << MSG::DEBUG << "Table from BremMatch points to NULL" << endreq;
    m_errorCount["7. No brems table       "] += 1;
  }
  const TrkCaloPIDTable* eEcalTable = m_ecalPIDe->direct();
  if( 0 == eEcalTable ) {
    msg << MSG::DEBUG << "Table EcalPIDe points to NULL";
    m_errorCount["7a.No EcalPIDe table    "] += 1;
  }
  const TrkCaloPIDTable* ePrsTable = m_prsPIDe->direct();
  if( 0 == ePrsTable ) {
    msg << MSG::DEBUG << "Table PrsPIDe points to NULL";
    m_errorCount["7b.No PrsPIDe  table    "] += 1;
  }
  const TrkCaloPIDTable* eBremTable = m_bremPIDe->direct();
  if( 0 == eBremTable ) {
    msg << MSG::DEBUG << "Table BremPIDe points to NULL";
    m_errorCount["7c.No BremPIDe table    "] += 1;
  }
  const TrkCaloPIDTable* eHcalTable = m_hcalPIDe->direct();
  if( 0 == eHcalTable ) {
    msg << MSG::DEBUG << "Table HcalPIDe points to NULL";
    m_errorCount["7d.No HcalPIDe table    "] += 1;
  }
  const TrkCaloPIDTable* muEcalTable = m_ecalPIDmu->direct();
  if( 0 == muEcalTable ) {
    msg << MSG::DEBUG << "Table EcalPIDmu points to NULL";
    m_errorCount["7e.No EcalPIDmu table   "] += 1;
  }
  const TrkCaloPIDTable* muHcalTable = m_hcalPIDmu->direct();
  if( 0 == muHcalTable ) {
    msg << MSG::DEBUG << "Table HcalPIDmu points to NULL";
    m_errorCount["7f.No HcalPIDmu table   "] += 1;
  }
  
  // ProtoParticles should only be "good tracks"

  int countProto[4] = { 0, 0, 0, 0 };
  int countRejTracks[4] = { 0, 0, 0, 0 };

  StatusCode scpid = StatusCode::SUCCESS;
  TrStoredTracks::const_iterator iTrack;
  CombinedDLL* combDLL = new CombinedDLL();
  for( iTrack = tracks->begin(); tracks->end() != iTrack; ++iTrack ) {

    // Does the track satisfies criteria to make a ProtoParticle ?
    int reject = rejectTrack( (*iTrack) );
    if( 0 != reject ) {
      countRejTracks[reject] += 1;
      continue;
    }
    
    ProtoParticle* proto = new ProtoParticle();
    proto->setTrack( *iTrack );
    double tkcharge = proto->charge();
    if( 0 == tkcharge ) {
      if ( m_monitor ) {
        msg << MSG::DEBUG << "track charge = " << tkcharge << endreq;
      }
      delete proto;
      continue;
    }

    countProto[TrackProto]++;
 
    // Reset particleID combination numbers    
    combDLL->reset();
    
    // Add RichPID to this ProtoParticle
    if( richData ) {
      scpid = addRich( richpids, proto, combDLL );
      if( !scpid.isFailure() ) {
        countProto[RichProto]++;
      }
    }
    
    // Add MuonID to this ProtoParticle
    if( muonData ) {
      double muonDLL = -999.;
      double muonProb = -999.;
      MuonIDs::const_iterator iMuon;
      for( iMuon = muonpids->begin(); muonpids->end() != iMuon; ++iMuon ) {
        const TrStoredTrack* track = (*iMuon)->idTrack();
        if( track == (*iTrack) ) {
          proto->setMuonPID( *iMuon );
          // When MuonDet is capable of identifying it add its result
          if( (*iMuon)->IsMuon() ) {
            proto->setMuonBit(1);
            countProto[MuonProto]++;
            muonProb = (*iMuon)->MuProb();
            proto->pIDDetectors().
              push_back( std::make_pair(ProtoParticle::MuonMuon, muonProb) );
            muonProbDLL( proto, combDLL );
            scpid = m_muonIDdll->calcMuonDLL( *iMuon, muonDLL ); 
            if( !scpid.isFailure() ){
              proto->pIDDetectors().
                push_back( std::make_pair(ProtoParticle::MuonPIDmu, muonDLL) );
              (combDLL->m_muDLL) += muonDLL;
            }            
          }
          // The muon pid for track has been added so exit from loop
          break;
        }
      } // break here
    }
    
    // Add CaloElectrons to this ProtoParticle   
    if( caloData ) {  
      // Add the Electron hypothesis when available (no cuts at the moment)
      const ElectronRange erange = etable->relations( *iTrack );
      if( !erange.empty() ) {
        countProto[ElectronProto]++;
        CaloHypo* hypo = erange.begin()->to();
        proto->addToCalo( hypo );
        
        double chi2 = erange.begin()->weight();
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::CaloEMatch, chi2) );
        
        // Add the CaloCluster chi2 (only lowest)
        const PhotonRange phrange = phtable->relations( *iTrack );
        if( !phrange.empty() ) {
          chi2 = phrange.begin()->weight();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::CaloTrMatch, chi2) );
        }
        
        // Add Brem hypothesis and chi2 (only lowest)
        const BremRange brrange = brtable->relations( *iTrack );
        if( !brrange.empty() ) {
          hypo = brrange.begin()->to();
          proto -> addToCalo( hypo );
          chi2 = brrange.begin()->weight();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::BremMatch, chi2) );
        }

        // Add Delta Log Likelihood for e+/- based on Ecal
        double caloDLL = 0.0;
        const TrkCaloPIDRange eecalrange = eEcalTable->relations( *iTrack );
        if( !eecalrange.empty() ) {
          proto->setCaloeBit(1);
          caloDLL = eecalrange.front().to();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::ECalPIDe, caloDLL) );
          (combDLL->m_eDLL) += caloDLL;
        }
        const TrkCaloPIDRange eprsrange = ePrsTable->relations( *iTrack );
        if( !eprsrange.empty() ) {
          caloDLL = eprsrange.front().to();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::PrsPIDe, caloDLL) );
          (combDLL->m_eDLL) += caloDLL;
        }
        const TrkCaloPIDRange ebremrange = eBremTable->relations( *iTrack );
        if( !ebremrange.empty() ) {
          caloDLL = ebremrange.front().to();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::BremPIDe, caloDLL) );
          (combDLL->m_eDLL) += caloDLL;
        }
        const TrkCaloPIDRange ehcalrange = eHcalTable->relations( *iTrack );
        if( !ehcalrange.empty() ) {
          caloDLL = ehcalrange.front().to();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::HcalPIDe, caloDLL) );
          (combDLL->m_eDLL) += caloDLL;
        }
        const TrkCaloPIDRange muecalrange = muEcalTable->relations( *iTrack );
        if( !muecalrange.empty() ) {
          caloDLL = muecalrange.front().to();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::EcalPIDmu, caloDLL) );
          (combDLL->m_muDLL) += caloDLL;
        }
        const TrkCaloPIDRange muhcalrange = muHcalTable->relations( *iTrack );
        if( !muhcalrange.empty() ) {
          caloDLL = muhcalrange.front().to();
          proto->pIDDetectors().
            push_back( std::make_pair(ProtoParticle::HcalPIDmu, caloDLL) );
          (combDLL->m_muDLL) += caloDLL;
        }         
      }
    }
    
    // Particle IDs from various detectors, if none are available
    // set NoneBit and assume is a pion
    if( proto->richBit() || proto->muonBit() || proto->caloeBit() ) {
      double de = (combDLL->m_eDLL) - (combDLL->m_piDLL);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::LkhPIDe, de ) );
      double dmu = (combDLL->m_muDLL) - (combDLL->m_piDLL);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::LkhPIDmu, dmu ) );
      double dk = (combDLL->m_kDLL) - (combDLL->m_piDLL);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::LkhPIDK, dk ) );
      double dp = (combDLL->m_pDLL) - (combDLL->m_piDLL);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::LkhPIDp, dp ) );
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::LkhPIDpi, 0.0 ) );
    } 
    else {
      proto->setNoneBit(1);
      proto->pIDDetectors().
        push_back( std::make_pair(ProtoParticle::NoPID, 1.0) );      
      int idpion = m_idPion * (int)proto->charge();
      proto->pIDInfo().push_back( std::make_pair( idpion, 1.0 ) );
    }
    
    chprotos->insert(proto);

  }
  delete combDLL;
  
  if( 0 == countProto[TrackProto] ) {
    m_errorCount["8. No Tracks for physics"] += 1;
  }
  if( 0 == chprotos->size() ) {
    m_errorCount["9. No ProtoParticles    "] += 1;
  }

  if( m_monitor ) {
    msg << MSG::DEBUG << "Found " << countProto[TrackProto]
        << " tracks of quality to produce ProtoParticles" << endreq;
    msg << MSG::DEBUG << "Made " << countProto[RichProto] 
        << " ProtoParticle with RichPID " << endreq;
    msg << MSG::DEBUG << "Made " << countProto[MuonProto]
        << " ProtoParticle with MuonID " << endreq;
    msg << MSG::DEBUG << "Made " << countProto[ElectronProto]
        << " ProtoParticle with ElectronHypo " << endreq;
    msg << MSG::DEBUG << "Number of ProtoParticles in TES is " 
        << chprotos->size() << endreq;

    for( ProtoParticles::iterator ip = chprotos->begin();
         chprotos->end() != ip; ++ip ) {
      msg << MSG::VERBOSE << "track = " << (*ip)->track() << endreq;
      msg << MSG::VERBOSE << "charge = " << (*ip)->charge() << endreq;
      msg << MSG::VERBOSE << "richid = " << (*ip)->richPID() << endreq;
      msg << MSG::VERBOSE << "muonid = " << (*ip)->muonPID() << endreq;
      msg << MSG::VERBOSE << "richhistory = " << (*ip)->richBit() << endreq;
      msg << MSG::VERBOSE << "muonhistory = " << (*ip)->muonBit() << endreq;
      msg << MSG::VERBOSE << "bestPID = " << (*ip)->bestPID() << endreq;
      for(ProtoParticle::PIDInfoVector::iterator id = (*ip)->pIDInfo().begin();
          (*ip)->pIDInfo().end()!=id; ++id ) {
        msg << MSG::VERBOSE << "id = " << (*id).first << " , prob = " 
            << (*id).second  << endreq;
      }
      for( ProtoParticle::PIDDetVector::iterator 
             idd = (*ip)->pIDDetectors().begin(); 
           (*ip)->pIDDetectors().end()!=idd; ++idd ) {
        msg << MSG::VERBOSE << "det = " << (*idd).first << " , value = " 
            << (*idd).second  << endreq;
      }
    }
  } // end monitor
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoPAlg::finalize() {

  MsgStream msg(msgSvc(), name());

  msg << MSG::DEBUG << "Release Tools" << endreq;
  if( m_photonMatch )   toolSvc()->releaseTool( m_photonMatch );
  if( m_electronMatch ) toolSvc()->releaseTool( m_electronMatch );
  if( m_bremMatch )     toolSvc()->releaseTool( m_bremMatch );
  if( m_ecalPIDe )      toolSvc()->releaseTool( m_ecalPIDe );
  if( m_prsPIDe ) toolSvc()->releaseTool( m_prsPIDe );
  if( m_bremPIDe ) toolSvc()->releaseTool( m_bremPIDe );
  if( m_hcalPIDe ) toolSvc()->releaseTool( m_hcalPIDe );
  if( m_ecalPIDmu ) toolSvc()->releaseTool( m_ecalPIDmu );
  if( m_hcalPIDmu ) toolSvc()->releaseTool( m_hcalPIDmu );
  if( m_muonIDdll ) toolSvc()->releaseTool( m_muonIDdll );


  msg << MSG::INFO << "********* ProtoParticles production Summary ******"
      << endreq;
  msg << MSG::INFO << "Number of events with :" << endreq;

  for( ErrorTable::iterator ierr = m_errorCount.begin();
       ierr != m_errorCount.end(); ierr++ ) { 
    msg << MSG::INFO << "   " << (*ierr).first 
        << "  " << format("%9u", (*ierr).second ) 
        << endreq;
  }
  msg << MSG::INFO << "**************************************************"
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
      if( m_velott && (track->unique() && track->veloTT()) ) {
        reject = KeepTrack;
      }
    }
    else {
      reject = NoTrackType;
    }
  }

  if( track->veloTT() ) return reject;

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
                                      ProtoParticle* proto, 
                                      CombinedDLL* combDLL ) {
 
  MsgStream msg(msgSvc(), name());
  StatusCode sc = StatusCode::FAILURE;

  double richProb = -999.;
  double richDLL = -999.;
  RichPIDs::const_iterator iRich;
  ProtoParticle::PIDInfoPair idinfo;
  for( iRich = richpids->begin(); richpids->end() != iRich; ++iRich ) {
    const TrStoredTrack* track = (*iRich)->recTrack();
    if( track == proto->track() ) {
      sc = StatusCode::SUCCESS;
      proto->setRichPID( *iRich );
      proto->setRichBit(1);
      
      // store Raw probabilities for RICH as detector info
      richProb = (*iRich)->particleRawProb(Rich::Electron);
      proto->pIDDetectors().
        push_back( std::make_pair(ProtoParticle::RichElectron, richProb) );
      
      richProb = (*iRich)->particleRawProb(Rich::Muon);
      proto->pIDDetectors().
        push_back( std::make_pair(ProtoParticle::RichMuon, richProb) );
      
      richProb = (*iRich)->particleRawProb(Rich::Pion);
      proto->pIDDetectors().
        push_back( std::make_pair(ProtoParticle::RichPion, richProb) );
      
      richProb = (*iRich)->particleRawProb(Rich::Kaon);
      proto->pIDDetectors()
        .push_back( std::make_pair(ProtoParticle::RichKaon, richProb) );
          
      richProb = (*iRich)->particleRawProb(Rich::Proton);
      proto->pIDDetectors()
        .push_back( std::make_pair(ProtoParticle::RichProton, richProb) );
      
      // Store delta log likelihood values and begin to combine them
      richDLL = (*iRich)->particleDeltaLL(Rich::Electron);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::RichPIDe, richDLL) );
      (combDLL->m_eDLL) = -richDLL;
      
      richDLL = (*iRich)->particleDeltaLL(Rich::Muon);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::RichPIDmu, richDLL) );
      (combDLL->m_muDLL) = -richDLL;

      richDLL = (*iRich)->particleDeltaLL(Rich::Pion);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::RichPIDpi, richDLL) );
      (combDLL->m_piDLL) = -richDLL;

      richDLL = (*iRich)->particleDeltaLL(Rich::Kaon);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::RichPIDK, richDLL) );
      (combDLL->m_kDLL) = -richDLL;

      richDLL = (*iRich)->particleDeltaLL(Rich::Proton);
      proto->pIDDetectors().
        push_back( std::make_pair( ProtoParticle::RichPIDp, richDLL) );
      (combDLL->m_pDLL) = -richDLL;
      
      if( m_monitor ) {
        msg << MSG::VERBOSE << "Rich " 
            << Rich::Electron << " = " 
            << (*iRich)->particleRawProb(Rich::Electron) 
            << endreq;
        msg << MSG:: VERBOSE << "Rich " 
            << Rich::Muon << " = " << (*iRich)->particleRawProb(Rich::Muon)
            << endreq;
        msg << MSG::VERBOSE << "Rich " 
            << Rich::Pion << " = " << (*iRich)->particleRawProb(Rich::Pion)
            << endreq;
        msg << MSG::VERBOSE << "Rich " 
            << Rich::Kaon << " = " << (*iRich)->particleRawProb(Rich::Kaon)
            << endreq;
        msg << MSG::VERBOSE << "Rich " 
            << Rich::Proton << " = " << (*iRich)->particleRawProb(Rich::Proton)
            << endreq;
      }

      // store normalized probabilities for combined probabilities and
      // set history flag
      richProb = (*iRich)->particleNormProb(Rich::Electron);
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
// Retrieve tools for Calorimeters PIDs
//=============================================================================
StatusCode ChargedProtoPAlg::caloPIDTools() {
  
  MsgStream msg(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;

  // Associators for CaloTables
  std::string matchType="AssociatorWeighted<CaloCluster,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_photonMatchName, m_photonMatch );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/"
        << m_photonMatchName << endreq;
    return sc;
  }

  matchType = "AssociatorWeighted<CaloHypo,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_electronMatchName,
                                m_electronMatch );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/"
        << m_electronMatchName << endreq;
    return sc;
  }

  matchType = "AssociatorWeighted<CaloHypo,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_bremMatchName, m_bremMatch );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/"
        << m_bremMatchName << endreq;
    return sc;
  }

  matchType = "Associator<TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_dLLeEcalName, m_ecalPIDe );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/" 
        << m_dLLeEcalName << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( matchType, m_dLLePrsName, m_prsPIDe );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/" 
        << m_dLLePrsName << endreq;
    return sc;
  }
  
  sc = toolSvc()->retrieveTool( matchType, m_dLLeBremName, m_bremPIDe );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/" 
        << m_dLLeBremName << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( matchType, m_dLLeHcalName, m_hcalPIDe );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/" 
        << m_dLLeHcalName << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( matchType, m_dLLmuEcalName, m_ecalPIDmu );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/" 
        << m_dLLmuEcalName << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( matchType, m_dLLmuHcalName, m_hcalPIDmu );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << matchType << "/" 
        << m_dLLmuHcalName << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Muon DLL based on MuonProb
//=============================================================================
StatusCode ChargedProtoPAlg::muonProbDLL( ProtoParticle* proto, 
                                          CombinedDLL* combDLL ) {

  // Use parametrization of MuProb likelihood (in 3 bins of momentum)
  // Could be done more elegantly with arrays of constants...
  double qp = proto->trStateP()->qDivP();
    
  double mulik;
  double mubkg;
  double re  = 0.;
  double rmu = 0.;
  double rpi = 0.;
  double rk  = 0.;
  double rp  = 0.;
  
  double mmu = proto->detPIDvalue( ProtoParticle::MuonMuon );
  if (fabs(1./qp) < 10000.) {
    if (mmu < 0.01) {
      mulik = 295.;
      mubkg = 945.;
    }
    else if (mmu >= 1.0) {
      mulik = 378.;
      mubkg = 176.;
    }
    else {
      mulik = exp(1.762+2.621*mmu);
      mubkg = exp(2.243+1.026*mmu);
    }
    rmu = rmu + log(mulik / 35.47);  // Update muon log-likelihood
    re  = re  + log(mubkg / 28.27);   // and background hypotheses
    rpi = rpi + log(mubkg / 28.27);
    rk  = rk  + log(mubkg / 28.27);
    rp  = rp  + log(mubkg / 28.27);
  }
  else if (fabs(1./qp) < 20000.) {
    if (mmu < 0.01) {
      mulik = 8.;
      mubkg = 154.;
    }
    else if (mmu >= 1.0) {
      mulik = 948.;
      mubkg = 133.;
    }
    else {
      mulik = exp(-0.851+5.864*mmu);
      mubkg = exp(0.185+2.982*mmu);
    }
    rmu = rmu + log(mulik / 34.63);                               
    re  = re  + log(mubkg / 11.20);  
    rpi = rpi + log(mubkg / 11.20);
    rk  = rk  + log(mubkg / 11.20);
    rp  = rp  + log(mubkg / 11.20);
  }
  else {
    if (mmu < 0.01) {
      mulik = exp(-5.608);
      mubkg = 92.;
    }
    else if (mmu >= 1.0) {
      mulik = 3624.;
      mubkg = 138.;
    }
    else {
      mulik = exp(-5.608+11.490*mmu);
      mubkg = exp(-0.816+3.588*mmu);
    }
    rmu = rmu + log(mulik / 65.31);
    re  = re  + log(mubkg / 6.81); 
    rpi = rpi + log(mubkg / 6.81);
    rk  = rk  + log(mubkg / 6.81);
    rp  = rp  + log(mubkg / 6.81);
  }

  (combDLL->m_muDLL) += rmu;
  (combDLL->m_eDLL)  += re;
  (combDLL->m_piDLL) += rpi;
  (combDLL->m_kDLL)  += rk;
  (combDLL->m_pDLL)  += rp;  

  return StatusCode::SUCCESS;

}  

//=============================================================================
