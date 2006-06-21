//-----------------------------------------------------------------------------
/** @file ChargedProtoPAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoPAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoPAlg.cpp,v 1.38 2006-06-21 22:12:41 odescham Exp $
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

  // Calo PID : activate sub-det PID
  declareProperty("CaloPrsPID",  m_PrsPID    =   true  );
  declareProperty("CaloSpdPID",  m_SpdPID    =   true  );
  declareProperty("CaloEcalPID", m_EcalPID   =   true  );
  declareProperty("CaloHcalPID", m_HcalPID   =   true  );
  declareProperty("CaloBremPID", m_BremPID   =   true  );


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

  //
  
  if(!m_EcalPID)warning()  << " >>> ECAL PID HAS BEEN DISABLED "<< endreq;
  if(!m_PrsPID)warning()   << " >>> PRS  PID HAS BEEN DISABLED "<< endreq;
  if(!m_HcalPID)warning()  << " >>> HCAL PID HAS BEEN DISABLED "<< endreq;
  if(!m_BremPID)warning()  << " >>> BREM PID HAS BEEN DISABLED "<< endreq;
  if(!m_SpdPID)warning()   << " >>> SPD  PID HAS BEEN DISABLED "<< endreq;


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
    verbose() << " -> Track selected " << (*iTrack)->key() << endreq;
    verbose() << " -> Track type " << (*iTrack)->type() << endreq;

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
  EvalRange vRange ;
  AccRange  aRange ;
  HypoRange hRange;
  ClusRange cRange ;


  ////////////
  // Ecal PID
  ////////////
  if (m_EcalPID){
    aRange = m_InEcalTable -> relations ( proto->track() ) ;
    
    if( !aRange.empty() ){
      
      proto->addInfo(ProtoParticle::InAccEcal ,  (double) aRange.front().to() );
      
      if( aRange.front().to() ){
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Ecal acceptance"  << endreq;

        
        
        // Get the highest weight associated electron CaloHypo (3D matching)
        hRange =  m_elecTrTable ->relations ( proto->track() ) ;
        if ( !hRange.empty() ){
          proto->addToCalo ( hRange.front().to() );
          proto->addInfo(ProtoParticle::CaloElectronMatch , hRange.front().weight() ); 
        }else{
          proto->addInfo(ProtoParticle::CaloElectronMatch , 9999. );          
        }

        // Get the highest weight associated CaloCluster (2D matching)
        cRange =  m_clusTrTable -> inverse() ->relations ( proto->track() ) ;
        if ( !cRange.empty() ){
          proto->addInfo(ProtoParticle::CaloTrMatch , cRange.front().weight() ); 
        }else{
          proto->addInfo(ProtoParticle::CaloTrMatch , 9999. ); 
        }

        // Get EcalE (intermediate) estimator
        vRange = m_EcalETable -> relations ( proto->track() ) ;
        if( !vRange.empty() ) {
          proto->addInfo(ProtoParticle::CaloEcalE ,  vRange.front().to() );
        }else {
          proto->addInfo(ProtoParticle::CaloEcalE,   0. );
        }

        // Get EcalChi2 (intermediate) estimator
        vRange = m_EcalChi2Table -> relations ( proto->track() ) ;
        if( !vRange.empty() ){  proto->addInfo(ProtoParticle::CaloEcalChi2,  vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::CaloEcalChi2,  9999. );
        }

        // Get ClusChi2 (intermediate) estimator
        vRange = m_ClusChi2Table -> relations ( proto->track() ) ;
        if( !vRange.empty() ){  proto->addInfo(ProtoParticle::CaloClusChi2,  vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::CaloClusChi2,  9999. );
        }

        // Get Ecal DLL(e)
        vRange = m_dlleEcalTable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){proto->addInfo(ProtoParticle::EcalPIDe , vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::EcalPIDe , -9999. );
        }
        
        // Get Ecal DLL(mu)
        vRange = m_dllmuEcalTable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){proto->addInfo(ProtoParticle::EcalPIDmu , vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::EcalPIDmu , -9999. );
        }
        
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Ecal PID : " 
                    << " Chi2-3D    =" <<  proto->info(ProtoParticle::CaloElectronMatch, -999.)
                    << " Chi2-2D    =" <<  proto->info(ProtoParticle::CaloTrMatch, -999.)
                    << " EcalE      =" <<  proto->info(ProtoParticle::CaloEcalE , -999.)
                    << " ClusChi2   =" <<  proto->info(ProtoParticle::CaloClusChi2, -999.)
                    << " EcalChi2   =" <<  proto->info(ProtoParticle::CaloEcalChi2, -999.)
                    << " Dlle (Ecal) =" <<  proto->info(ProtoParticle::EcalPIDe, -999.)
                    << " Dllmu (Ecal) =" <<  proto->info(ProtoParticle::EcalPIDmu, -999.)
                  << endreq;
  
      }else{
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Ecal acceptance"  << endreq;
      }
    }else{
      
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Ecal acceptance table "  << endreq;
    }
  }else{
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> Ecal PID has been disabled"  << endreq;
  }
  //////////////
  // Brem PID //
  //////////////
  if( m_BremPID  ){

    aRange = m_InBremTable -> relations ( proto->track() ) ;

    if( !aRange.empty() ){
      
      proto->addInfo(ProtoParticle::InAccBrem ,  (double) aRange.front().to() );
      
      if( aRange.front().to() ){
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The Brem. extrapolated line is in Ecal acceptance"  << endreq;
        
        
        
        // Get the highest weight associated brem. CaloHypo (3D matching)
        hRange =  m_bremTrTable ->relations ( proto->track() ) ;
        if ( !hRange.empty() ){
          proto->addToCalo ( hRange.front().to() );
          proto->addInfo(ProtoParticle::CaloBremMatch , hRange.front().weight() ); 
        }else{
          proto->addInfo(ProtoParticle::CaloBremMatch , 9999. );
        } 
      
        // Get the BremChi2 (intermediate) estimator
        vRange = m_BremChi2Table -> relations ( proto->track() ) ;
        if( !vRange.empty() ){  proto->addInfo(ProtoParticle::CaloBremChi2,  vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::CaloBremChi2,  9999. );
        }          

        // Get the Brem DLL(e)
        vRange = m_dlleBremTable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){proto->addInfo(ProtoParticle::BremPIDe , vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::BremPIDe , -9999. );
        }

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> BremStrahlung PID : " 
                    << " Chi2-Brem  =" <<  proto->info(ProtoParticle::CaloBremMatch, -999.)
                    << " BremChi2   =" <<  proto->info(ProtoParticle::CaloBremChi2, -999.)
                    << " Dlle (Brem) =" <<  proto->info(ProtoParticle::BremPIDe, -999.)
                    << endreq;        
      }else{
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The Brem. extrapolated line is NOT in Ecal acceptance"  << endreq;
      }
    }else{
      
      if ( msgLevel(MSG::VERBOSE) )verbose() << " ->  No entry for that track in the Brem acceptance table"  << endreq;
    }
  }else{
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> Brem PID has been disabled"  << endreq;
  }

  //////////////
  // HCAL PID //
  //////////////
  if( m_HcalPID  ){
    
    aRange = m_InHcalTable -> relations ( proto->track() ) ;
    
    if( !aRange.empty() ) {
      
      proto->addInfo(ProtoParticle::InAccHcal ,  (double) aRange.front().to() );
      
      if( aRange.front().to() ){
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Hcal acceptance"  << endreq;
        
        
        // Get the HcalE (intermediate) estimator
        vRange = m_HcalETable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){  proto->addInfo(ProtoParticle::CaloHcalE,  vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::CaloHcalE, 0. );
        }
        // Get the Hcal DLL(e)
        vRange = m_dlleHcalTable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){proto->addInfo(ProtoParticle::HcalPIDe , vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::HcalPIDe , -9999. );
        }
        
        // Get the Hcal DLL(mu)
        vRange = m_dllmuHcalTable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){proto->addInfo(ProtoParticle::HcalPIDmu , vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::HcalPIDmu, -9999. );
        }
        
        
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Hcal PID  : " 
                    << " HcalE      =" <<  proto->info(ProtoParticle::CaloHcalE, -999.)
                    << " Dlle (Hcal) =" <<  proto->info(ProtoParticle::HcalPIDe, -999.)
                    << " Dllmu (Hcal) =" <<  proto->info(ProtoParticle::HcalPIDmu, -999.)
                    << endreq;
        
        
      }else{
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Hcal acceptance"  << endreq;
      }
    }else{
      
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Hcal acceptance table"  << endreq;
    }
  }else{
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> Hcal PID has been disabled"  << endreq;
  }

  //////////////
  // Prs PID //
  //////////////
  if(m_PrsPID){
    aRange = m_InPrsTable -> relations ( proto->track() ) ;
    
    if( !aRange.empty() ) {
      
      proto->addInfo(ProtoParticle::InAccPrs ,  (double) aRange.front().to() );
      
      if( aRange.front().to() ){
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Prs acceptance"  << endreq;
        
        
        // Get the PrsE (intermediate) estimator
        vRange = m_PrsETable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){  proto->addInfo(ProtoParticle::CaloPrsE,  vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::PrsPIDe , -9999. );
        }
        
        // Get the Prs DLL(e)
        vRange = m_dllePrsTable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){proto->addInfo(ProtoParticle::PrsPIDe , vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::CaloPrsE, 0. );
        }
        
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Prs PID : " 
                    << " PrsE       =" <<  proto->info(ProtoParticle::CaloPrsE, -999.)
                    << " Dlle (Prs)  =" <<  proto->info(ProtoParticle::PrsPIDe , -999.)
                    << endreq;
        
      }else{
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Prs acceptance"  << endreq;
      }
    }else{
      
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Prs acceptance table"  << endreq;
    }
  }else{
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> Prs PID has been disabled"  << endreq;
  }
  //////////////
  // Spd PID //
  //////////////
  if(m_SpdPID){

    aRange = m_InSpdTable -> relations ( proto->track() ) ;
    
    if( !aRange.empty() ) {
      
      proto->addInfo(ProtoParticle::InAccSpd ,  (double) aRange.front().to() );
      
      if( aRange.front().to() ){
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Spd acceptance"  << endreq;
        
        
        // Get the PrsE (intermediate) estimator
        vRange = m_SpdETable -> relations ( proto->track() ) ;
        if( !vRange.empty() ){  proto->addInfo(ProtoParticle::CaloSpdE,  vRange.front().to() );
        }else{
          proto->addInfo(ProtoParticle::CaloSpdE, 0. );
        }

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Spd PID : " 
                    << " SpdE       =" <<  proto->info(ProtoParticle::CaloSpdE, -999.)
                    << endreq;
      
      }else{
        
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Spd acceptance"  << endreq;
      }
    }else{
      
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Spd acceptance table"  << endreq;
    }
  }else{
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> Spd PID has been disabled"  << endreq;
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


  /* Ecal PID */
  if( m_EcalPID ){

    if ( !exist<ITrAccTable>(CaloIdLocation::InEcal ) )
      return Warning("No InEcal Acceptance table at     '"+CaloIdLocation::InEcal+"'" );
    m_InEcalTable = get<ITrAccTable>( CaloIdLocation::InEcal );
    
    if ( !exist<ITrHypoTable2D>( CaloIdLocation::ElectronMatch )  )
      return Warning( "No Electron-Track relation table at '"+ CaloIdLocation::ElectronMatch+"'" );
    m_elecTrTable = get<ITrHypoTable2D>(  CaloIdLocation::ElectronMatch );
    
    if ( !exist<IClusTrTable2D>( CaloIdLocation::ClusterMatch )  )
      return Warning("No Cluster-Track relation table at  '"+CaloIdLocation::ClusterMatch+"'" );
    m_clusTrTable = get<IClusTrTable2D>( CaloIdLocation::ClusterMatch );
    
    if ( !exist<ITrEvalTable>( CaloIdLocation::EcalChi2 ) )
      return Warning("No EcalChi2 relation table at     '"+CaloIdLocation::EcalChi2+"'" );
    m_EcalChi2Table = get<ITrEvalTable>(  CaloIdLocation::EcalChi2 );
    
    if ( !exist<ITrEvalTable>( CaloIdLocation::EcalE ) )
      return Warning("No EcalE relation table at     '"+ CaloIdLocation::EcalE+"'" );
    m_EcalETable = get<ITrEvalTable>(  CaloIdLocation::EcalE );
    
    if ( !exist<ITrEvalTable>( CaloIdLocation::ClusChi2 ) )
      return Warning("No ClusChi2 relation table at     '"+CaloIdLocation::ClusChi2+"'" );
    m_ClusChi2Table = get<ITrEvalTable>( CaloIdLocation::ClusChi2 );
    
    
    if ( !exist<ITrEvalTable>( CaloIdLocation::EcalPIDe )  )
      return Warning("No DlleEcal  relation table at     '"+ CaloIdLocation::EcalPIDe+"'" );
    m_dlleEcalTable  = get<ITrEvalTable>(  CaloIdLocation::EcalPIDe );
    
    if ( !exist<ITrEvalTable>( CaloIdLocation::EcalPIDmu ) )
      return Warning("No DLLmuEcal relation table at     '"+ CaloIdLocation::EcalPIDmu+"'" );
    m_dllmuEcalTable = get<ITrEvalTable>(  CaloIdLocation::EcalPIDmu );
    
    debug() << "Ecal PID SUCCESFULLY LOADED" << endreq;
  
  }else{
    debug() << "Ecal PID HAS BEEN DISABLED" << endreq;    
  }


  /* Brem PID */
  if( m_BremPID ){

    if ( !exist<ITrAccTable>( CaloIdLocation::InBrem )  )
      return Warning("No InBrem Acceptance table at '"+ CaloIdLocation::InBrem+"'" );
    m_InBremTable = get<ITrAccTable>(  CaloIdLocation::InBrem  );
   
    if ( !exist<ITrHypoTable2D>( CaloIdLocation::BremMatch ) )
      return Warning(" No Brem-Track relation table at '"+ CaloIdLocation::BremMatch +"'" );
    m_bremTrTable = get<ITrHypoTable2D>(  CaloIdLocation::BremMatch );
      
    if ( !exist<ITrEvalTable>( CaloIdLocation::BremChi2 ) )
      return Warning("No BremlChi2 relation table at     '"+ CaloIdLocation::BremChi2+"'" );
    m_BremChi2Table = get<ITrEvalTable>( CaloIdLocation::BremChi2  );
    
    if ( !exist<ITrEvalTable>( CaloIdLocation::BremPIDe ) )
      return Warning("No DLLeBrem relation table at     '"+ CaloIdLocation::BremPIDe+"'" );
    m_dlleBremTable = get<ITrEvalTable>( CaloIdLocation::BremPIDe  );
    
    debug() << "BREM PID SUCCESFULLY LOADED" << endreq;

  }else{
    debug() << "BREM PID HAS BEEN DISABLED" << endreq;    
  }


  /* Spd PID */
  if( m_SpdPID ){

    if ( !exist<ITrAccTable>( CaloIdLocation::InSpd ) )
      return Warning("No InSpd Acceptance table at     '"+CaloIdLocation::InSpd+"'" );
    m_InSpdTable = get<ITrAccTable>( CaloIdLocation::InSpd );

    if ( !exist<ITrEvalTable>( CaloIdLocation::SpdE ) )
      return Warning("No SpdE relation table at     '"+ CaloIdLocation::SpdE+"'" );
    m_SpdETable = get<ITrEvalTable>(  CaloIdLocation::SpdE );

    debug() << "SPD PID SUCCESFULLY LOADED" << endreq;

  }else{
    debug() << "SPD PID HAS BEEN DISABLED" << endreq;
  }

  /* Prs PID */
  if( m_SpdPID ){

    if ( !exist<ITrAccTable>( CaloIdLocation::InPrs ) )
      return Warning("No InPrs Acceptance table at     '"+ CaloIdLocation::InPrs+"'" );
    m_InPrsTable = get<ITrAccTable>(  CaloIdLocation::InPrs );
    
    if ( !exist<ITrEvalTable>( CaloIdLocation::PrsE ) )
      return Warning("No PrsE relation table at     '"+ CaloIdLocation::PrsE+"'" );
    m_PrsETable = get<ITrEvalTable>(  CaloIdLocation::PrsE  );    

    if ( !exist<ITrEvalTable>( CaloIdLocation::PrsPIDe )   )
      return Warning("No DLLePrs   relation table at     '"+ CaloIdLocation::PrsPIDe+"'" );
    m_dllePrsTable   = get<ITrEvalTable>( CaloIdLocation::PrsPIDe );

    debug() << "PRS PID SUCCESFULLY LOADED" << endreq;

  }else{
    debug() << "PRS PID HAS BEEN DISABLED" << endreq;
  }



  /* Hcal PID */
  if( m_HcalPID ){

    if ( !exist<ITrAccTable>( CaloIdLocation::InHcal ) )
      return Warning("No InHcal Acceptance table at     '"+ CaloIdLocation::InHcal+"'" );
    m_InHcalTable = get<ITrAccTable>(  CaloIdLocation::InHcal );

    if ( !exist<ITrEvalTable>( CaloIdLocation::HcalE ) )
      return Warning("No HcalE relation table at     '"+ CaloIdLocation::HcalE+"'" );
    m_HcalETable = get<ITrEvalTable>(  CaloIdLocation::HcalE  );    

    if ( !exist<ITrEvalTable>( CaloIdLocation::HcalPIDe ) )
      return Warning("No DLLeHcal  relation table at     '"+ CaloIdLocation::HcalPIDe +"'" );
    m_dlleHcalTable  = get<ITrEvalTable>(  CaloIdLocation::HcalPIDe  );  
  
    if ( !exist<ITrEvalTable>( CaloIdLocation::HcalPIDmu ) )
    return Warning("No DLLmuEcal relation table at     '"+ CaloIdLocation::HcalPIDe+"'" );
    m_dllmuHcalTable = get<ITrEvalTable>( CaloIdLocation::HcalPIDmu );
    
    debug() << "HCAL PID SUCCESFULLY LOADED" << endreq;
    
  }else{

    debug() << "HCAL PID HAS BEEN DISABLED" << endreq;

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
