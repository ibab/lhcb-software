// $Id: ChargedProtoPAlg.cpp,v 1.24 2005-11-21 09:42:32 cattanem Exp $
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
  : GaudiAlgorithm ( name , pSvcLocator )
  //
  , m_electronPath      ( "Rec/Calo/Electrons"     )
  , m_photonMatchName   ( "Rec/Calo/PhotonMatch"   )
  , m_electronMatchName ( "Rec/Calo/ElectronMatch" )
  , m_bremMatchName     ( "Rec/Calo/BremMatch"     )
  , m_dLLeEcalName      ( "Rec/Calo/EcalPIDe"      )
  , m_dLLePrsName       ( "Rec/Calo/PrsPIDe"       )
  , m_dLLeBremName      ( "Rec/Calo/BremPIDe"      )
  , m_dLLeHcalName      ( "Rec/Calo/HcalPIDe"      ) 
  , m_dLLmuEcalName     ( "Rec/Calo/EcalPIDmu"     )
  , m_dLLmuHcalName     ( "Rec/Calo/HcalPIDmu"     )
  //
  , m_downstream( true )
  , m_velott( true )
  , m_trackClassCut( 0.4 )
  , m_chiSqITracks( 500.0 )
  , m_chiSqOTracks( 100.0 )
  , m_chiSqVTT( 5.0 )
  , m_chiSqDowns( 3.0 )
  , m_ptVTT( 50.0 )
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
  declareProperty("DownstreamTracks", m_downstream );
  declareProperty("VeloTTTracks",     m_velott );
  declareProperty("ITFracTrackClass", m_trackClassCut );
  declareProperty("Chi2NdFofITracks", m_chiSqITracks );
  declareProperty("Chi2NdFofOTracks", m_chiSqOTracks );
  declareProperty("Chi2NdFofVTT",     m_chiSqVTT );
  declareProperty("Chi2NdFofDowns",     m_chiSqDowns );  
  declareProperty("ptVTT",     m_ptVTT );

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

  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

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
  // If adding new counters do it here before the FATAL one
  m_errorCount["FATAL: TrStoredTracks with no TrState found"] = 0;

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
  // Track->Cluster position matching relation table  
  const PhotonTable* phtable = 0 ;
  {
    const _PMT2D* pmt2d = get<_PMT2D>( m_photonMatchName ) ;
    if ( 0 != pmt2d   ) { phtable = pmt2d->inverse() ; }
    if ( 0 == phtable )
    { 
      msg << MSG::DEBUG << "Table from PhotonMatch points to NULL" << endreq;
      caloData = false;
      m_errorCount["5. No photon table      "] += 1;
    } 
  }
  // Track->Hypo energy/position matching relation table 
  const ElectronTable* etable = 0 ; 
  {
    const _EMT2D* emt2d = get<_EMT2D> ( m_electronMatchName  ) ;
    if ( 0 != emt2d  ) { etable = emt2d -> inverse() ; }
    if ( 0 == etable ) 
    { 
      msg << MSG::DEBUG << "Table from ElectronMatch points to NULL" << endreq;
      caloData = false;
      m_errorCount["6. No electron table    "] += 1;
    }
  }
  
  // Track->Hypo   bremstrahlung position matching relation table 
  const BremTable* brtable = 0 ; 
  {
    const _BMT2D* bmt2d = get<_BMT2D> ( m_bremMatchName ) ;
    if ( 0 != bmt2d   ) { brtable = bmt2d->inverse() ; }
    if ( 0 == brtable ) 
    { 
      msg << MSG::DEBUG << "Table from BremMatch points to NULL" << endreq;
      m_errorCount["7. No brems table       "] += 1;
    }    
  }
  
  const TrkCaloPIDTable* eEcalTable = 
    get<TrkCaloPIDTable>( m_dLLeEcalName ) ;
  if ( 0 == eEcalTable ) 
  {
    msg << MSG::DEBUG << "Table EcalPIDe points to NULL";
    m_errorCount["7a.No EcalPIDe table    "] += 1;
  }
  
  const TrkCaloPIDTable* ePrsTable = 
    get<TrkCaloPIDTable> ( m_dLLePrsName ) ;
  if ( 0 == ePrsTable ) 
  {
    msg << MSG::DEBUG << "Table PrsPIDe points to NULL";
    m_errorCount["7b.No PrsPIDe  table    "] += 1;
  }
  
  const TrkCaloPIDTable* eBremTable = 
    get<TrkCaloPIDTable> ( m_dLLeBremName ) ;
  if ( 0 == eBremTable ) 
  {
    msg << MSG::DEBUG << "Table BremPIDe points to NULL";
    m_errorCount["7c.No BremPIDe table    "] += 1;
  }
  
  const TrkCaloPIDTable* eHcalTable = 
    get<TrkCaloPIDTable> ( m_dLLeHcalName ) ;
  if ( 0 == eHcalTable ) 
  {
    msg << MSG::DEBUG << "Table HcalPIDe points to NULL";
    m_errorCount["7d.No HcalPIDe table    "] += 1;
  }
  
  const TrkCaloPIDTable* muEcalTable = 
    get<TrkCaloPIDTable> ( m_dLLmuEcalName ) ;
  if ( 0 == muEcalTable ) 
  {
    msg << MSG::DEBUG << "Table EcalPIDmu points to NULL";
    m_errorCount["7e.No EcalPIDmu table   "] += 1;
  }
  
  const TrkCaloPIDTable* muHcalTable = 
    get<TrkCaloPIDTable> ( m_dLLmuHcalName ) ;
  if ( 0 == muHcalTable ) 
  {
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
      if( NoTrState == reject ) { 
        msg << MSG::FATAL << "Found TrStoredTrack with no TrStates!"
            << endreq;
        m_errorCount["FATAL: TrStoredTracks with no TrState found"] += 1;
        return StatusCode::FAILURE; 
      }
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
    
    // Add the Electron hypothesis when available (no cuts at the moment)
    if ( 0 != etable )
    {
      const ElectronTable::Range range = etable->relations ( *iTrack ) ;
      if ( !range.empty() )
      {
        ++countProto[ElectronProto];
        //
        const CaloHypo* hypo = range.front().to    () ;
        const double    chi2 = range.front().weight() ;
        if ( 0 != hypo ) { proto->addToCalo ( hypo ) ; }
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::CaloEMatch, chi2) );
      } 
    }
    if ( 0 != phtable ) 
    {
      const PhotonTable::Range range = phtable->relations ( *iTrack ) ;
      if ( !range.empty() )
      {
        //
        const double    chi2 = range.front().weight() ;
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::CaloTrMatch, chi2) );
      }  
    }
    if ( 0 != brtable ) 
    {
      const BremTable::Range range = brtable->relations ( *iTrack ) ;
      const CaloHypo* hypo = range.front() .to() ;
      const double    chi2 = range.front().weight() ;
      if ( 0 != hypo ) { proto->addToCalo ( hypo ) ; }
      proto->pIDDetectors().
        push_back( std::make_pair(ProtoParticle::BremMatch, chi2) );
    }
    
    double caloDLLe  = 0 ;
    double caloDLLmu = 0 ;
    
    if ( 0 != eEcalTable ) 
    {
      const TrkCaloPIDTable::Range range = eEcalTable->relations ( *iTrack ) ;
      if ( !range.empty() ) 
      {
        //
        proto->setCaloeBit(1) ;
        //
        const double value = range.front().to() ;
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::ECalPIDe, value ) );
        //
        caloDLLe += value ; (combDLL->m_eDLL) += caloDLLe ;
      }
    }
    if ( 0 != ePrsTable ) 
    {
      const TrkCaloPIDTable::Range range = ePrsTable->relations ( *iTrack ) ;
      if ( !range.empty() ) 
      {
        //
        proto->setCaloeBit(1) ;
        //
        const double value = range.front().to() ;
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::PrsPIDe, value ) );
        //
        caloDLLe += value ; (combDLL->m_eDLL) += caloDLLe ;
      }
    }    
    if ( 0 != eBremTable ) 
    {
      const TrkCaloPIDTable::Range range = eBremTable->relations ( *iTrack ) ;
      if ( !range.empty() ) 
      {
        const double value = range.front().to() ;
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::BremPIDe, value ) );
        //
        caloDLLe += value ; (combDLL->m_eDLL) += caloDLLe ;
      }
    }
    if ( 0 != eHcalTable ) 
    {
      const TrkCaloPIDTable::Range range = eHcalTable->relations ( *iTrack ) ;
      if ( !range.empty() ) 
      {
        const double value = range.front().to() ;
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::HcalPIDe, value ) );
        //
        caloDLLe += value ; (combDLL->m_eDLL) += caloDLLe ;
      }
    }
    if ( 0 != muEcalTable ) 
    {
      const TrkCaloPIDTable::Range range = muEcalTable->relations ( *iTrack ) ;
      if ( !range.empty() ) 
      {
        const double value = range.front().to() ;
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::EcalPIDmu, value ) );
        //
        caloDLLmu += value ; (combDLL->m_muDLL) += caloDLLmu ;
      }
    }
    if ( 0 != muHcalTable ) 
    {
      const TrkCaloPIDTable::Range range = muHcalTable->relations ( *iTrack ) ;
      if ( !range.empty() ) 
      {
        const double value = range.front().to() ;
        proto->pIDDetectors().
          push_back( std::make_pair(ProtoParticle::HcalPIDmu, value ) );
        //
        caloDLLmu += value ; (combDLL->m_muDLL) += caloDLLmu ;
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

  msg << MSG::INFO << "********* ProtoParticles error Summary ***********"
      << endreq;
  msg << MSG::INFO << "Number of events with :" << endreq;
  
  for( ErrorTable::iterator ierr = m_errorCount.begin();
       ierr != m_errorCount.end() ; ierr++ ) {
    if( 0 != (*ierr).second ) {
      msg << MSG::INFO << "   " << (*ierr).first 
          << "  " << format("%9u", (*ierr).second ) 
          << endreq;
    }
  }
  msg << MSG::INFO << "**************************************************"
      << endreq;
  
  return GaudiAlgorithm::finalize() ;
}; 

//=============================================================================
// rejectTrack because of poor quality
//   keep only Long, Downstream and Upstream tracks, no clones 
//   reject tracks with fit errors
// Note that history does not accumulate:: it is only the original container
// and then a track is classified as a clone but not respect to what
//=============================================================================
int ChargedProtoPAlg::rejectTrack( const TrStoredTrack* track ) {

  int reject = NoTrack;
  if( NULL == track ) return reject;
  if( 0 == track->errorFlag() ) {
    //      if( track->unique() && (track->forward() || track->match()) ) {
    if( track->unique() && track->isLong() ) {
      reject = KeepTrack;
    }
    //      if( m_upstream && (track->unique() && track->upstream()) ) {
    if( m_downstream && (track->unique() && track->isDownstream()) ) {
      reject = KeepTrack; 
    }
    if( m_velott && (track->unique() && track->isUpstream()) ) {
      reject = KeepTrack;
    }
  }
  else {
    reject = NoTrackType;
  }
  
  //  if( track->veloTT() ) {
  if( track->isUpstream() ) {
    int nTotMeas = track->measurements().size();
    double chi2NoF = (track->lastChiSq())/((double)nTotMeas - 5);
    if( chi2NoF >= m_chiSqVTT ) {
      reject = Chi2Cut;
    }
    const TrState* trackState=track->closestState(0.0);
    // This should never happens but it seems to with unpacking problems
    if( NULL == trackState ) {
      reject = NoTrState;
      return reject;
    }    
    TrState* trackStateClone = trackState->clone();
    TrStateP* firstStateP=dynamic_cast<TrStateP*>(trackStateClone);
    if(firstStateP) {
      double  p=1./fabs(firstStateP->qDivP());
      double  slopeX=firstStateP->tx();
      double  slopeY=firstStateP->ty();
      double  pt=fabs(p)*
        sqrt(1.-1./(1.+slopeX*slopeX+slopeY*slopeY));
      if( pt < m_ptVTT ) {
        reject = Other;
      }
    }
    delete trackStateClone;
    return reject;
  }  

  if( track->isDownstream() ) {
    int nTotMeas = track->measurements().size();
    double chi2NoF = (track->lastChiSq())/((double)nTotMeas - 5);
    if( chi2NoF >= m_chiSqDowns )  return Chi2Cut;
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
                                      ProtoParticle* proto, 
                                      CombinedDLL* combDLL ) {
 
  MsgStream msg(msgSvc(), name());
  StatusCode sc = StatusCode::FAILURE;

  double richProb = -999.;
  double richDLL = -999.;
  RichPIDs::const_iterator iRich;
  ProtoParticle::PIDInfoPair idinfo;
  for( iRich = richpids->begin(); richpids->end() != iRich; ++iRich ) {
    const TrStoredTrack* track = (*iRich)->trStoredTrack();
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

