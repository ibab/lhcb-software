//-----------------------------------------------------------------------------
/** @file ChargedProtoPAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoPAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoPAlg.cpp,v 1.35 2006-06-20 18:14:40 odescham Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ChargedProtoPAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoPAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoPAlg::ChargedProtoPAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_protos       ( NULL ),
    m_trSel        ( NULL ),
    m_nEvts        ( 0 )
{
  // Input data
  declareProperty( "InputTrackLocation",
                   m_tracksPath = TrackLocation::Default );
  declareProperty( "InputRichPIDLocation",
                   m_richPath = RichPIDLocation::Default );
  declareProperty( "InputMuonPIDLocation",
                   m_muonPath = MuonPIDLocation::Default );

  // CaloID objects : relationWeighted tables, DLL's tables, Acceptance tables  
  declareProperty("CaloClusMatchLocation",    
                  m_clusMatchPath     = CaloIdLocation::ClusterMatch  );
  declareProperty("CaloElecMatchLocation", 
                  m_elecMatchPath     = CaloIdLocation::ElectronMatch );
  declareProperty("CaloBremMatchLocation", 
                  m_bremMatchPath     = CaloIdLocation::BremMatch     );
  declareProperty("CaloEcalPIDeLocation", 
                  m_dLLeEcalPath      = CaloIdLocation::EcalPIDe      );
  declareProperty("CaloPrsPIDeLocation", 
                  m_dLLePrsPath       =  CaloIdLocation::PrsPIDe      );
  declareProperty("CaloHcalPIDeLocation", 
                  m_dLLeHcalPath      =  CaloIdLocation::HcalPIDe     );
  declareProperty("CaloBremPIDeLocation", 
                  m_dLLeBremPath      =  CaloIdLocation::BremPIDe     );
  declareProperty("CaloEcalPIDmuLocation", 
                  m_dLLmuHcalPath      =  CaloIdLocation::HcalPIDmu    );
  declareProperty("CaloHcalPIDmuLocation", 
                  m_dLLmuEcalPath       =   CaloIdLocation::EcalPIDmu   );
  declareProperty("InSpdAccLocation", 
                  m_InSpdPath       =   CaloIdLocation::InSpd   );
  declareProperty("InPrsAccLocation", 
                  m_InPrsPath       =   CaloIdLocation::InPrs   );
  declareProperty("InEcalAccLocation", 
                  m_InEcalPath       =   CaloIdLocation::InEcal   );
  declareProperty("InHcalAccLocation", 
                  m_InHcalPath       =   CaloIdLocation::InHcal  );
  declareProperty("InBremAccLocation", 
                  m_InBremPath       =   CaloIdLocation::InBrem   );


  // output data
  declareProperty( "OutputProtoParticleLocation",
                   m_protoPath = ProtoParticleLocation::Charged );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoPAlg::~ChargedProtoPAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoPAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  // get an instance of the track selector
  m_trSel = tool<ITrackSelector>( "TrackSelector", "TrackSelector", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoPAlg::execute()
{

  // Load the Track objects (manditory - should be there for each event)
  if ( !exist<Tracks>(m_tracksPath) )
  {
    return Warning( "No Tracks at '"+m_tracksPath+"'" );
  }
  const Tracks * tracks = get<Tracks>( m_tracksPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << tracks->size()
            << " Tracks from " << m_tracksPath << endreq;
  }

  // Load the RichPIDs
  const StatusCode richSc = getRichData();
  if ( richSc.isFailure() ) return richSc;

  // Load the MuonPIDs
  const StatusCode muonSc = getMuonData();
  if ( muonSc.isFailure() ) return muonSc;

  // Load the CaloPIDs
  const StatusCode caloSc = getCaloData();
  if ( caloSc.isFailure() ) return caloSc;

  // Create the ProtoParticle container
  m_protos = new ProtoParticles();
  // give to Gaudi
  put ( protos(), m_protoPath );

  // Loop over tracks
  for ( Tracks::const_iterator iTrack = tracks->begin();
        iTrack != tracks->end(); ++iTrack )
  {

    // track tally
    TrackTally & tally = m_nTracks[ (*iTrack)->type() ];
    ++tally.totTracks;

    // Select tracks
    verbose() << "Trying Track " << (*iTrack)->key() << endreq;
    if ( !m_trSel->accept(**iTrack) ) continue;
    verbose() << " -> Track selected" << (*iTrack)->key() << endreq;

    // Count selectedtracks
    ++tally.selTracks;

    // Make a proto-particle
    ProtoParticle* proto = new ProtoParticle();
    // Insert into container
    protos()->insert( proto );

    // Set track reference
    proto->setTrack( *iTrack );

    // Add minimal track info
    proto->addInfo( ProtoParticle::TrackChi2PerDof, (*iTrack)->chi2PerDoF() );
    proto->addInfo( ProtoParticle::TrackNumDof,     (*iTrack)->nDoF()       );
    proto->addInfo( ProtoParticle::TrackHistory,    (*iTrack)->history()    );
    proto->addInfo( ProtoParticle::TrackType,       (*iTrack)->type()       );

    // flag signifying if any PID info has been added for this track
    bool hasRICHInfo(false), hasMUONInfo(false), hasCALOInfo(false) ;

    // Combined DLL data object for this proto
    CombinedLL combLL(0);

    // Add RICH info
    if ( addRich(proto,combLL) )
    {
      hasRICHInfo = true;
      ++tally.richTracks;
    }

    // Add MUON info
    if ( addMuon(proto,combLL) )
    {
      hasMUONInfo = true;
      ++tally.muonTracks;
    }

    // Add CALO info (To do)
    if ( addCalo(proto,combLL) ) 
      { 
	hasCALOInfo = true; 
	++tally.caloTracks; 
      }

    // Add Velo dE/dx info (To do)

    // has any PID info been added ?
    if ( hasRICHInfo || hasMUONInfo || hasCALOInfo )
    {
      // finalise the combined DLL information
      // Store the DLLs for all hypos w.r.t. pion
      proto->addInfo( ProtoParticle::CombDLLe,  combLL.elDLL-combLL.piDLL );
      proto->addInfo( ProtoParticle::CombDLLmu, combLL.muDLL-combLL.piDLL );
      proto->addInfo( ProtoParticle::CombDLLpi, 0 ); // by definition
      proto->addInfo( ProtoParticle::CombDLLk,  combLL.kaDLL-combLL.piDLL );
      proto->addInfo( ProtoParticle::CombDLLp,  combLL.prDLL-combLL.piDLL );
    }
    else
    {
      // NO PID was added, so add a flag confirming this to the proto
      proto->addInfo(ProtoParticle::NoPID,1);
    }

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Created ProtoParticle : " << *proto << endreq;
    }

  }

  // update tallies
  ++m_nEvts;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Add RICH info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addRich( ProtoParticle * proto, CombinedLL & combLL )
{
  // Does this track have a RICH PID result ?
  TrackToRichPID::const_iterator iR = m_richMap.find( proto->track() );
  if ( m_richMap.end() == iR ) return false;

  const RichPID * richPID = (*iR).second;

  // RichPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Found RichPID data : DLls = " << richPID->particleLLValues() << endreq;
  }

  // reference to RichPID object
  proto->setRichPID( richPID );

  // Store the raw RICH PID info
  proto->addInfo( ProtoParticle::RichDLLe,  richPID->particleDeltaLL(Rich::Electron) );
  proto->addInfo( ProtoParticle::RichDLLmu, richPID->particleDeltaLL(Rich::Muon) );
  proto->addInfo( ProtoParticle::RichDLLpi, richPID->particleDeltaLL(Rich::Pion) );
  proto->addInfo( ProtoParticle::RichDLLk,  richPID->particleDeltaLL(Rich::Kaon) );
  proto->addInfo( ProtoParticle::RichDLLp,  richPID->particleDeltaLL(Rich::Proton) );
  // stored the combined DLLs
  combLL.elDLL += richPID->particleDeltaLL(Rich::Electron);
  combLL.muDLL += richPID->particleDeltaLL(Rich::Muon);
  combLL.piDLL += richPID->particleDeltaLL(Rich::Pion);
  combLL.kaDLL += richPID->particleDeltaLL(Rich::Kaon);
  combLL.prDLL += richPID->particleDeltaLL(Rich::Proton);
  // Store History
  proto->addInfo( ProtoParticle::RichPIDStatus, richPID->pidResultCode() );

  return true;
}

//=============================================================================
// Add MUON info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addMuon( LHCb::ProtoParticle * proto, CombinedLL & combLL )
{
  // Does this track have a MUON PID result ?
  TrackToMuonPID::const_iterator iM = m_muonMap.find( proto->track() );
  if ( m_muonMap.end() == iM ) return false;

  // MuonPID object is found
  const MuonPID * muonPID = (*iM).second;
  // check IsMuon flag - Reject non-muons
  if ( !muonPID->IsMuon() ) return false;

  // MuonPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Found MuonPID data : MuLL=" <<  muonPID->MuonLLMu()
              << " BkLL=" <<  muonPID->MuonLLBg()
              << " nSharedHits=" << muonPID->nShared()
              << " isMuon=" << muonPID->IsMuon()
              << endreq;
  }

  // reference to MuonPID object
  proto->setMuonPID( muonPID );

  // Store the PID info
  proto->addInfo( ProtoParticle::MuonMuLL,      muonPID->MuonLLMu() );
  proto->addInfo( ProtoParticle::MuonBkgLL,     muonPID->MuonLLBg() );
  proto->addInfo( ProtoParticle::MuonNShared,   muonPID->nShared()  );
  // stored the combined DLLs
  combLL.elDLL += muonPID->MuonLLBg();
  combLL.muDLL += muonPID->MuonLLMu();
  combLL.piDLL += muonPID->MuonLLBg();
  combLL.kaDLL += muonPID->MuonLLBg();
  combLL.prDLL += muonPID->MuonLLBg();
  // Store History
  proto->addInfo( ProtoParticle::MuonPIDStatus, muonPID->Status()   );

  return true;
}


//=============================================================================
// Add Calo info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addCalo( LHCb::ProtoParticle * proto, CombinedLL & combLL )
{
  // Does this track have a Calo PID result ?
  typedef LHCb::Calo2Track::ITrHypoTable2D::Range              HypoRange;
  typedef LHCb::Calo2Track::IClusTrTable2D::InverseType::Range ClusRange;
  typedef LHCb::Calo2Track::ITrEvalTable::Range EvalRange;
  typedef LHCb::Calo2Track::ITrAccTable::Range  AccRange;

  // Get the highest weight CaloHypo associated with that proto->track()
  const HypoRange eRange =  m_elecTrTable ->relations ( proto->track() ) ;
  if ( msgLevel(MSG::DEBUG) ){
    debug() << " -> # caloHypos associated with that track " << eRange.size() <<endreq;
  }

  if ( eRange.empty() )return false; 
  const LHCb::CaloHypo* hypo = eRange.front().to() ;
  if ( 0 == hypo ) return false; // No CaloHypo 

  // Get it :
  proto->addToCalo ( hypo ) ; // Add Electron CaloHypo

  // 3D energy/position matching -Chi2
  proto->addInfo(ProtoParticle::CaloElectronMatch , eRange.front().weight() ); 
  

  // 2D Cluster position matching
  const ClusRange cRange =  m_clusTrTable -> inverse() ->relations ( proto->track() ) ;
  if ( !cRange.empty() )proto->addInfo(ProtoParticle::CaloTrMatch , cRange.front().weight() ); 

  // 3D Brem. matching
  const HypoRange bRange =  m_bremTrTable ->relations ( proto->track() ) ;
  if ( !bRange.empty() ){
    const LHCb::CaloHypo* hypo = bRange.front().to() ;
    if ( 0 != hypo ){
      proto->addInfo(ProtoParticle::CaloBremMatch , bRange.front().weight() ); 
      proto->addToCalo ( hypo ) ;  // Add Brem. CaloHypo
    }
  }
  

  // Dll's
  EvalRange vRange ;

  vRange = m_dlleEcalTable -> relations ( proto->track() ) ;
  if( !vRange.empty() )proto->addInfo(ProtoParticle::EcalPIDe , vRange.front().to() );

  vRange = m_dllePrsTable -> relations ( proto->track() ) ;
  if( !vRange.empty() )proto->addInfo(ProtoParticle::PrsPIDe , vRange.front().to() );

  vRange = m_dlleHcalTable -> relations ( proto->track() ) ;
  if( !vRange.empty() )proto->addInfo(ProtoParticle::HcalPIDe , vRange.front().to() );
  
  vRange = m_dlleBremTable -> relations ( proto->track() ) ;
  if( !vRange.empty() )proto->addInfo(ProtoParticle::BremPIDe , vRange.front().to() );
  
  vRange = m_dllmuEcalTable -> relations ( proto->track() ) ;
  if( !vRange.empty() )proto->addInfo(ProtoParticle::EcalPIDmu , vRange.front().to() );

  vRange = m_dllmuHcalTable -> relations ( proto->track() ) ;
  if( !vRange.empty() )proto->addInfo(ProtoParticle::HcalPIDmu , vRange.front().to() );

  // Acceptance flag
  AccRange aRange ;
  
  aRange = m_InSpdTable -> relations ( proto->track() ) ;
  if( !aRange.empty() )  proto->addInfo(ProtoParticle::InAccSpd , (double) aRange.front().to() );

  aRange = m_InPrsTable -> relations ( proto->track() ) ;
  if( !aRange.empty() )  proto->addInfo(ProtoParticle::InAccPrs , (double) aRange.front().to() );

  aRange = m_InEcalTable -> relations ( proto->track() ) ;
  if( !aRange.empty() )  proto->addInfo(ProtoParticle::InAccEcal ,  (double) aRange.front().to() );

  aRange = m_InHcalTable -> relations ( proto->track() ) ;
  if( !aRange.empty() )  proto->addInfo(ProtoParticle::InAccHcal ,  (double) aRange.front().to() );

  aRange = m_InBremTable -> relations ( proto->track() ) ;
  if( !aRange.empty() )  proto->addInfo(ProtoParticle::InAccBrem ,  (double) aRange.front().to() );
  
    
  
  // CaloPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
  {
  verbose() << " -> Found CaloPID data : " 
            << " Chi2-3D    =" <<  proto->info(ProtoParticle::CaloElectronMatch, -999.)
            << " Chi2-2D    =" <<  proto->info(ProtoParticle::CaloTrMatch, -999.)
            << " Chi2-Brem  =" <<  proto->info(ProtoParticle::CaloBremMatch, -999.)
            << " Dlle (Ecal) =" <<  proto->info(ProtoParticle::EcalPIDe, -999.)
            << " Dlle (Prs)  =" <<  proto->info(ProtoParticle::PrsPIDe , -999.)
            << " Dlle (Hcal) =" <<  proto->info(ProtoParticle::HcalPIDe, -999.)
            << " Dlle (Brem) =" <<  proto->info(ProtoParticle::BremPIDe, -999.)
            << " Dllmu (Ecal) =" <<  proto->info(ProtoParticle::EcalPIDmu, -999.)
            << " Dllmu (Hcal) =" <<  proto->info(ProtoParticle::HcalPIDmu, -999.)
            << endreq;
  }

  // stored the combined DLLs

  // DLL(el)
  combLL.elDLL += proto->info(ProtoParticle::EcalPIDe, 0.);
  combLL.elDLL += proto->info(ProtoParticle::HcalPIDe, 0.);
  combLL.elDLL += proto->info(ProtoParticle::PrsPIDe , 0.);
  combLL.elDLL += proto->info(ProtoParticle::BremPIDe, 0.);
  // DLL(mu)
  combLL.muDLL += proto->info(ProtoParticle::EcalPIDmu, 0.);
  combLL.muDLL += proto->info(ProtoParticle::HcalPIDmu, 0.);

  return true;
}

//=============================================================================
// Loads the RICH data
//=============================================================================
StatusCode ChargedProtoPAlg::getRichData()
{
  // empty the map
  m_richMap.clear();

  // Do we have any RichPID results
  if ( !exist<RichPIDs>(m_richPath) )
    return Warning( "No RichPIDs at '"+m_richPath+"'" );

  // yes, so load them
  const RichPIDs * richpids = get<RichPIDs>( m_richPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << richpids->size()
            << " RichPIDs from " << m_richPath << endreq;
  }

  // refresh the reverse mapping
  for ( RichPIDs::const_iterator iR = richpids->begin();
        iR != richpids->end(); ++iR )
  {
    m_richMap[ (*iR)->track() ] = *iR;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Loads the Muon data
//=============================================================================
StatusCode ChargedProtoPAlg::getMuonData()
{
  // empty the map
  m_muonMap.clear();

  // Do we have any MuonPID results
  if ( !exist<MuonPIDs>(m_muonPath) )
    return Warning( "No MuonPIDs at '"+m_muonPath+"'" );

  // yes, so load them
  const MuonPIDs * muonpids = get<MuonPIDs>( m_muonPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << muonpids->size()
            << " MuonPIDs from " << m_muonPath << endreq;
  }

  // refresh the reverse mapping
  for ( MuonPIDs::const_iterator iM = muonpids->begin();
        iM != muonpids->end(); ++iM )
  {
    m_muonMap[ (*iM)->idTrack() ] = *iM;
  }

  return StatusCode::SUCCESS;
}



//=============================================================================
// Loads the Calo data
//=============================================================================
StatusCode ChargedProtoPAlg::getCaloData()
{

  // Load inputs 
  using namespace LHCb::Calo2Track;
  
  /////////////////////////////////////////////
  // CaloHypo<->Track relation table (Electron) 3D matching

  if ( !exist<ITrHypoTable2D>(m_elecMatchPath)  )
    return Warning( "No Electron-Track relation table at '"+m_elecMatchPath+"'" );
  m_elecTrTable = get<ITrHypoTable2D>( m_elecMatchPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "Electron-Track relation tables uccessfully loaded at " << m_caloPath <<"'"<< endreq;
  }

  ////////////////////////////////////////
  // CaloCluster<-> Track relation tables 2D matching

  if ( !exist<IClusTrTable2D>(m_clusMatchPath)  )
    return Warning("No Cluster-Track relation table at  '"+m_clusMatchPath+"'" );
  m_clusTrTable = get<IClusTrTable2D>( m_clusMatchPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "Cluster-Track table sucessfully loaded at '" << m_clusMatchPath <<"'"<< endreq;
  }
  
  ////////////////////////////////////////// 
  // CaloHypo<->Track relation table (Brem.) 3D matching
  if ( !exist<ITrHypoTable2D>(m_bremMatchPath)  )
    return Warning("No Brem-Track relation table at    '"+m_bremMatchPath+"'" );
  m_bremTrTable = get<ITrHypoTable2D>( m_bremMatchPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "Brem-Track table sucessfully loaded at '" << m_bremMatchPath <<"'"<< endreq;
  }


  ////////////////
  // CaloId DLL's

  if ( !exist<ITrEvalTable>(m_dLLeEcalPath)  )
    return Warning("No DlleEcal  relation table at     '"+m_dLLeEcalPath+"'" );
  m_dlleEcalTable  = get<ITrEvalTable>( m_dLLeEcalPath );
                   if ( msgLevel(MSG::DEBUG) ){
    debug() << "DLLeEcal-Track table sucessfully loaded at '" << m_dLLeEcalPath <<"'"<< endreq;
  }
  
    if ( !exist<ITrEvalTable>(m_dLLePrsPath)   )
    return Warning("No DLLePrs   relation table at     '"+m_dLLePrsPath+"'" );
  m_dllePrsTable   = get<ITrEvalTable>( m_dLLePrsPath  );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "DLLePrs-Track table sucessfully loaded at '" << m_dLLePrsPath <<"'"<< endreq;
  }
  
  if ( !exist<ITrEvalTable>(m_dLLeHcalPath)  )
    return Warning("No DLLeHcal  relation table at     '"+m_dLLeHcalPath+"'" );
  m_dlleHcalTable  = get<ITrEvalTable>( m_dLLeHcalPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "DLLeHcal-Track table sucessfully loaded at '" << m_dLLeHcalPath <<"'"<< endreq;
  }
  
  if ( !exist<ITrEvalTable>(m_dLLeBremPath)  )
    return Warning("No DLLeBrem  relation table at     '"+m_dLLeBremPath+"'" );
  m_dlleBremTable  = get<ITrEvalTable>( m_dLLeBremPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "DLLeBrem-Track table sucessfully loaded at '" << m_dLLeBremPath <<"'"<< endreq;
  }
  
  if ( !exist<ITrEvalTable>(m_dLLmuHcalPath) )
    return Warning("No DLLmuEcal relation table at     '"+m_dLLmuHcalPath+"'" );
  m_dllmuHcalTable = get<ITrEvalTable>( m_dLLmuHcalPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "DLLmuHcal-Track table sucessfully loaded at '" << m_dLLmuHcalPath<<"'"<< endreq;
  }
  
  if ( !exist<ITrEvalTable>(m_dLLmuEcalPath) )
    return Warning("No DLLmuEcal relation table at     '"+m_dLLmuEcalPath+"'" );
  m_dllmuEcalTable = get<ITrEvalTable>( m_dLLmuEcalPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "DLLmuEcal-Track table sucessfully loaded at '" << m_dLLmuEcalPath<<"'"<< endreq;
  }

  ////////////////////////////
  // CaloId Acceptance filters

  if ( !exist<ITrAccTable>(m_InSpdPath) )
    return Warning("No InSpd Acceptance table at     '"+m_InSpdPath+"'" );
  m_InSpdTable = get<ITrAccTable>( m_InSpdPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "InSpd Acceptance filter table sucessfully loaded at '" << m_InSpdPath <<"'"<< endreq;
  }

  if ( !exist<ITrAccTable>(m_InPrsPath) )
    return Warning("No InPrs Acceptance table at     '"+m_InPrsPath+"'" );
  m_InPrsTable = get<ITrAccTable>( m_InPrsPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "InPrs Acceptance filter table sucessfully loaded at '" << m_InPrsPath <<"'"<< endreq;
  }

  if ( !exist<ITrAccTable>(m_InEcalPath) )
    return Warning("No InEcal Acceptance table at     '"+m_InEcalPath+"'" );
  m_InEcalTable = get<ITrAccTable>( m_InEcalPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "InEcal Acceptance filter table sucessfully loaded at '" << m_InEcalPath <<"'"<< endreq;
  }
  
  if ( !exist<ITrAccTable>(m_InHcalPath) )
    return Warning("No InHcal Acceptance table at     '"+m_InHcalPath+"'" );
  m_InHcalTable = get<ITrAccTable>( m_InHcalPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "InHcal Acceptance filter table sucessfully loaded at '" << m_InHcalPath <<"'"<< endreq;
  }
  
  if ( !exist<ITrAccTable>(m_InBremPath) )
    return Warning("No InBrem Acceptance table at     '"+m_InBremPath+"'" );
  m_InBremTable = get<ITrAccTable>( m_InBremPath );
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "InBrem Acceptance filter table sucessfully loaded at '" << m_InBremPath <<"'"<< endreq;
  }
  

  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoPAlg::finalize()
{
  // summary printout

  info() << "Number of events processed = " << m_nEvts << endreq;
  for ( TrackMap::const_iterator iT = m_nTracks.begin();
        iT != m_nTracks.end(); ++iT )
  {
    info() << "Track Type = '" << (*iT).first << "' :-" << endreq;
    const TrackTally & tally = (*iT).second;
    const double tkSel = 100 * ( tally.totTracks>0 ? (double)tally.selTracks/(double)tally.totTracks : 0 );
    const double protos = ( m_nEvts>0 ? (double)tally.selTracks/(double)m_nEvts : 0 );
    const double protoRICH = 100 * ( tally.selTracks>0 ? (double)tally.richTracks/(double)tally.selTracks : 0 );
    const double protoCALO = 100 * ( tally.selTracks>0 ? (double)tally.caloTracks/(double)tally.selTracks : 0 );
    const double protoMUON = 100 * ( tally.selTracks>0 ? (double)tally.muonTracks/(double)tally.selTracks : 0 );

    info() << " -> Created. = " << protos << " ProtoParticles/event : Track Sel. Eff. "
           << tkSel << "%" << endreq;
    info() << "  -> " << protoRICH << "% with RichPID information" << endreq;
    info() << "  -> " << protoMUON << "% with MuonPID information" << endreq;
    info() << "  -> " << protoCALO << "% with CaloPID information" << endreq;
  }

  // execute base class finalise and return
  return GaudiAlgorithm::finalize();
}

//=============================================================================
