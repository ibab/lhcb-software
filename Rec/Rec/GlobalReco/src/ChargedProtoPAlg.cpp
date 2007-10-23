//-----------------------------------------------------------------------------
/** @file ChargedProtoPAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoPAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoPAlg.cpp,v 1.62 2007-10-23 10:41:53 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Chrono.h"
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
    m_tracksPath   ( TrackLocation::Default ),
    m_richPath     ( RichPIDLocation::Default ),
    m_muonPath     ( MuonPIDLocation::Default ),
    m_protos       ( NULL ),
    m_trSel        ( NULL ),
    m_velodEdx     ( NULL ),
    m_electron     ( NULL ),
    m_clusTrTable ( NULL ),
    m_elecTrTable ( NULL ),
    m_bremTrTable ( NULL ),
    m_dlleEcalTable ( NULL ),
    m_dllePrsTable  ( NULL ),
    m_dlleHcalTable ( NULL ),
    m_dlleBremTable ( NULL ),
    m_dllmuHcalTable ( NULL ),
    m_dllmuEcalTable ( NULL ),
    m_InSpdTable  ( NULL ),
    m_InPrsTable  ( NULL ),
    m_InEcalTable ( NULL ),
    m_InHcalTable ( NULL ),
    m_InBremTable ( NULL ),
    m_SpdETable ( NULL ),
    m_PrsETable ( NULL ),
    m_HcalETable ( NULL ),
    m_EcalETable ( NULL ),
    m_ClusChi2Table ( NULL ),
    m_BremChi2Table ( NULL ),
    m_EcalChi2Table ( NULL ),
    m_nEvts        ( 0    ),
    m_timer(NULL),
    m_timerIndex(0)
{
  // context specific locations
  if      ( context() == "Offline" )
  {
    m_richPath   = LHCb::RichPIDLocation::Offline; 
    m_muonPath   = MuonPIDLocation::Default;
  }
  else if ( context() == "HLT" || context() == "Hlt" )
  {
    m_richPath   = LHCb::RichPIDLocation::HLT;
    m_tracksPath = LHCb::TrackLocation::HltForward;
    m_muonPath   = "Hlt/Muon/MuonPID";
  }

  // Input data
  declareProperty( "InputTrackLocation", m_tracksPath );
  declareProperty( "InputRichPIDLocation", m_richPath );
  declareProperty( "InputMuonPIDLocation", m_muonPath );

  // activate sub-det PID
  declareProperty("UseRichPID",     m_richPID   =   true  );
  declareProperty("UseMuonPID",     m_muonPID   =   true  );
  declareProperty("UseVeloPID",     m_veloPID   =   true  );
  declareProperty("UseCaloPrsPID",  m_PrsPID    =   true  );
  declareProperty("UseCaloSpdPID",  m_SpdPID    =   true  );
  declareProperty("UseCaloEcalPID", m_EcalPID   =   true  );
  declareProperty("UseCaloHcalPID", m_HcalPID   =   true  );
  declareProperty("UseCaloBremPID", m_BremPID   =   true  );

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

  // Velo dE/dx tool
  if ( m_veloPID) 
  {
    m_velodEdx = tool<ITrackVelodEdxCharge>( "TrackVelodEdxCharge", "VeloCharge", this );
  }
  
  // CaloElectron tool
  if (m_EcalPID)
  {
    m_electron = tool<ICaloElectron>("CaloElectron","CaloElectron",this);
  }
  
  // disabled CALO warnings
  if (!m_EcalPID) Warning( "ECAL PID has been disabled", StatusCode::SUCCESS );
  if (!m_PrsPID)  Warning( "PRS  PID has been disabled", StatusCode::SUCCESS );
  if (!m_HcalPID) Warning( "HCAL PID has been disabled", StatusCode::SUCCESS );
  if (!m_BremPID) Warning( "BREM PID has been disabled", StatusCode::SUCCESS );
  if (!m_SpdPID)  Warning( "SPD  PID has been disabled", StatusCode::SUCCESS );
  // disable RICH warnings
  if (!m_richPID)  Warning( "RICH PID has been disabled", StatusCode::SUCCESS );
  // disable MUON warnings
  if (!m_muonPID)  Warning( "MUON PID has been disabled", StatusCode::SUCCESS );
  // disable VELO warnings
  if (!m_veloPID)  Warning( "VELO PID has been disabled", StatusCode::SUCCESS );

  // timing in debug mode only
  if (msgLevel(MSG::DEBUG)) 
  {
    m_timer      = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerIndex = m_timer->addTimer(name()+"::execute") ;
  }
  
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoPAlg::execute()
{
  if ( msgLevel(MSG::DEBUG) ) m_timer->start(m_timerIndex) ;
  
  // Load the Track objects (manditory - should be there for each event)
  if ( !exist<Tracks>(m_tracksPath) )
  {
    setFilterPassed(false);
    return Warning( "No Tracks at '"+m_tracksPath+"'", StatusCode::SUCCESS );
  }
  const Tracks * tracks = get<Tracks>( m_tracksPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << tracks->size()
            << " Tracks from " << m_tracksPath << endreq;
  }

  // Load the RichPIDs
  const bool richSc = (m_richPID && getRichData());

  // Load the MuonPIDs
  const bool muonSc = (m_muonPID && getMuonData());

  // Load the Calo info
  const bool ecalSc = (m_EcalPID && getEcalData());
  const bool bremSc = (m_BremPID && getBremData());
  const bool spdSc  = (m_SpdPID  && getSpdData());
  const bool prsSc  = (m_PrsPID  && getPrsData());
  const bool hcalSc = (m_HcalPID && getHcalData());

  // ProtoParticle container
  if ( exist<ProtoParticles>(m_protoPath) )
  {
    // get existing container, clear, and reuse
    Warning( "Existing ProtoParticle container at " + m_protoPath + 
             " found -> Will replace", StatusCode::SUCCESS );
    m_protos = get<ProtoParticles>(m_protoPath);
    m_protos->clear();
  }
  else
  {
    // make new container and give to gaudi
    m_protos = new ProtoParticles();
    put ( m_protos, m_protoPath );
  }

  // Loop over tracks
  for ( Tracks::const_iterator iTrack = tracks->begin();
        iTrack != tracks->end(); ++iTrack )
  {

    // track tally
    TrackTally & tally = m_nTracks[ (*iTrack)->type() ];
    ++tally.totTracks;

    // Select tracks
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Trying Track " << (*iTrack)->key() << endreq;
    if ( !m_trSel->accept(**iTrack) ) continue;
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Track selected " << (*iTrack)->key() << endreq;
      verbose() << " -> Track type " << (*iTrack)->type() << endreq;
      verbose() << " -> Track flag " << (*iTrack)->flag() << endreq;
      verbose() << " -> Track charge " << (*iTrack)->charge() << endreq;
    }

    // Count selectedtracks
    ++tally.selTracks;

    // Make a proto-particle
    ProtoParticle* proto = new ProtoParticle();
    // Insert into container, with same key as Track
    protos()->insert( proto, (*iTrack)->key() );

    // Set track reference
    proto->setTrack( *iTrack );

    // Add minimal track info
    proto->addInfo( ProtoParticle::TrackChi2PerDof, (*iTrack)->chi2PerDoF() );
    proto->addInfo( ProtoParticle::TrackNumDof,     (*iTrack)->nDoF()       );
    proto->addInfo( ProtoParticle::TrackHistory,    (*iTrack)->history()    );
    proto->addInfo( ProtoParticle::TrackType,       (*iTrack)->type()       );

    // flag signifying if any PID info has been added for this track
    bool hasRICHInfo(false), hasMUONInfo(false);
    bool hasECALInfo(false), hasBREMInfo(false), hasSPDInfo(false), hasPRSInfo(false), hasHCALInfo(false);

    // Add RICH info
    if ( richSc && addRich(proto) )
    {
      hasRICHInfo = true;
      ++tally.richTracks;
    }

    // Add MUON info
    if ( muonSc && addMuon(proto) )
    {
      hasMUONInfo = true;
      ++tally.muonTracks;
    }

    // Add ECAL info
    if ( ecalSc && addEcal(proto) )
    {
      hasECALInfo = true;
      ++tally.ecalTracks;
    }

    // Add Brem info
    if ( bremSc && addBrem(proto) )
    {
      hasBREMInfo = true;
      ++tally.bremTracks;
    }

    // Add Spd info
    if ( spdSc && addSpd(proto) )
    {
      hasSPDInfo = true;
      ++tally.spdTracks;
    }

    // Add Prs info
    if ( prsSc && addPrs(proto) )
    {
      hasPRSInfo = true;
      ++tally.prsTracks;
    }

    // Add Hcal info
    if ( hcalSc && addHcal(proto) )
    {
      hasHCALInfo = true;
      ++tally.hcalTracks;
    }

    // Add Velo dE/dx info
    if ( m_veloPID && addVelodEdx(proto) )
    {
      ++tally.velodEdxTracks;
    }

    // has any PID info been added ?
    if ( !( hasRICHInfo || hasMUONInfo || 
            hasECALInfo || hasBREMInfo || hasSPDInfo || hasPRSInfo || hasHCALInfo ) )
    {
      // NO PID was added, so add a flag confirming this to the proto
      proto->addInfo(ProtoParticle::NoPID,1);
    }

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Created ProtoParticle : " << *proto << endreq;
    }
    if ( msgLevel(MSG::DEBUG) ){
      debug() << " Created protoparticle with MuonID " 
              << proto->info( LHCb::ProtoParticle::MuonPIDStatus, -1.) 
              << " RICH ID " << proto->info( LHCb::ProtoParticle::RichPIDStatus, -1.) << endmsg ;
    }
    
  }

  // update tallies
  ++m_nEvts;

  if ( msgLevel(MSG::DEBUG) )
  {
    m_timer->stop(m_timerIndex) ;
    debug() << "  USER TIME: " << m_timer->lastTime(m_timerIndex)
             << " Tracks " << tracks->size() << endmsg ;
  }
  
  // return
  return StatusCode::SUCCESS;
}

//=============================================================================
// Add RICH info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addRich( ProtoParticle * proto ) const
{
  // Does this track have a RICH PID result ?
  TrackToRichPID::const_iterator iR = m_richMap.find( proto->track() );
  if ( m_richMap.end() == iR ) return false;

  const RichPID * richPID = (*iR).second;

  // RichPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Found RichPID data : DLLs = " << richPID->particleLLValues() << endreq;
  }

  // reference to RichPID object
  proto->setRichPID( richPID );

  // Store the raw RICH PID info
  proto->addInfo( ProtoParticle::RichDLLe,  richPID->particleDeltaLL(Rich::Electron) );
  proto->addInfo( ProtoParticle::RichDLLmu, richPID->particleDeltaLL(Rich::Muon) );
  proto->addInfo( ProtoParticle::RichDLLpi, richPID->particleDeltaLL(Rich::Pion) );
  proto->addInfo( ProtoParticle::RichDLLk,  richPID->particleDeltaLL(Rich::Kaon) );
  proto->addInfo( ProtoParticle::RichDLLp,  richPID->particleDeltaLL(Rich::Proton) );

  // Store History
  proto->addInfo( ProtoParticle::RichPIDStatus, richPID->pidResultCode() );

  return true;
}

//=============================================================================
// Add MUON info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addMuon( LHCb::ProtoParticle * proto ) const
{
  // Does this track have a MUON PID result ?
  TrackToMuonPID::const_iterator iM = m_muonMap.find( proto->track() );
  if ( m_muonMap.end() == iM ) return false;

  // MuonPID object is found
  const MuonPID * muonPID = (*iM).second;

  // MuonPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Found MuonPID data : MuLL=" <<  muonPID->MuonLLMu()
              << " BkLL=" <<  muonPID->MuonLLBg()
              << " nSharedHits=" << muonPID->nShared()
              << " isMuon=" << muonPID->IsMuon()
              << endreq;
  }

  // store acceptance flag for those in acceptance (lack of flag signifies
  // track was outside acceptance)
  if ( muonPID->InAcceptance() )
  {
    proto->addInfo( ProtoParticle::InAccMuon, true );
  }

  // check IsMuon flag - Reject non-muons
  if ( !muonPID->IsMuon() ) return false;

  // reference to MuonPID object
  proto->setMuonPID( muonPID );

  // Store the PID info
  proto->addInfo( ProtoParticle::MuonMuLL,      muonPID->MuonLLMu() );
  proto->addInfo( ProtoParticle::MuonBkgLL,     muonPID->MuonLLBg() );
  proto->addInfo( ProtoParticle::MuonNShared,   muonPID->nShared()  );

  // Store History
  proto->addInfo( ProtoParticle::MuonPIDStatus, muonPID->Status()   );

  return true;
}

//=============================================================================
// Add Calo Ecal info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addEcal( LHCb::ProtoParticle * proto ) const
{
  LHCb::Calo2Track::ITrAccTable::Range                 aRange;
  LHCb::Calo2Track::ITrHypoTable2D::Range              hRange;
  LHCb::Calo2Track::IClusTrTable2D::InverseType::Range cRange;
  LHCb::Calo2Track::ITrEvalTable::Range                vRange;

  bool hasEcalPID = false;
  if (m_EcalPID)
  {
    aRange = m_InEcalTable -> relations ( proto->track() ) ;
    if ( !aRange.empty() )
    {
      hasEcalPID = aRange.front().to();

      if( hasEcalPID )
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> The track is in Ecal acceptance"  << endreq;
        proto->addInfo(ProtoParticle::InAccEcal, true );

        // Get the highest weight associated electron CaloHypo (3D matching)
        hRange = m_elecTrTable ->relations ( proto->track() ) ;
        if ( !hRange.empty() )
        {
          proto->addToCalo ( hRange.front().to() );
          // CaloElectron->caloTrajectory must be after addToCalo
          if ( m_electron->set(proto) )
          {
            proto->addInfo( ProtoParticle::CaloTrajectoryL, 
                            m_electron->caloTrajectoryL(CaloPlane::ShowerMax,"hypo") );
          }
          proto->addInfo(ProtoParticle::CaloChargedSpd, CaloSpd( hRange.front().to() ));
          proto->addInfo(ProtoParticle::CaloChargedPrs, CaloPrs( hRange.front().to() ));
          proto->addInfo(ProtoParticle::CaloChargedEcal, CaloEcal( hRange.front().to() ));
          proto->addInfo(ProtoParticle::CaloElectronMatch , hRange.front().weight() );
        }

        // Get the highest weight associated CaloCluster (2D matching)
        cRange = m_clusTrTable -> inverse() ->relations ( proto->track() ) ;
        if ( !cRange.empty() ) { proto->addInfo(ProtoParticle::CaloTrMatch , cRange.front().weight() ); }

        // Get EcalE (intermediate) estimator
        vRange = m_EcalETable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::CaloEcalE ,  vRange.front().to() ); }

        // Get EcalChi2 (intermediate) estimator
        vRange = m_EcalChi2Table -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::CaloEcalChi2,  vRange.front().to() ); }

        // Get ClusChi2 (intermediate) estimator
        vRange = m_ClusChi2Table -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::CaloClusChi2,  vRange.front().to() ); }

        // Get Ecal DLL(e)
        vRange = m_dlleEcalTable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::EcalPIDe , vRange.front().to() ); }

        // Get Ecal DLL(mu)
        vRange = m_dllmuEcalTable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::EcalPIDmu , vRange.front().to() ); }

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Ecal PID : "
                    << " Chi2-3D    =" <<  proto->info(ProtoParticle::CaloElectronMatch, -999.)
                    << " Chi2-2D    =" <<  proto->info(ProtoParticle::CaloTrMatch, -999.)
                    << " EcalE      =" <<  proto->info(ProtoParticle::CaloEcalE , -999.)
                    << " ClusChi2   =" <<  proto->info(ProtoParticle::CaloClusChi2, -999.)
                    << " EcalChi2   =" <<  proto->info(ProtoParticle::CaloEcalChi2, -999.)
                    << " Dlle (Ecal) =" <<  proto->info(ProtoParticle::EcalPIDe, -999.)
                    << " Dllmu (Ecal) =" <<  proto->info(ProtoParticle::EcalPIDmu, -999.)
                    << " Spd Digits " <<  proto->info(ProtoParticle::CaloChargedSpd, 0.)
                    << " Prs Digits " <<  proto->info(ProtoParticle::CaloChargedPrs, 0.)
                    << " Spd Digits " <<  proto->info(ProtoParticle::CaloChargedSpd, 0.)
                    << " Ecal Cluster " <<  proto->info(ProtoParticle::CaloChargedEcal, 0.)
                    << " TrajectoryL " <<  proto->info(ProtoParticle::CaloTrajectoryL, 0.)
                    << endreq;

      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Ecal acceptance"  << endreq;
      }
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Ecal acceptance table "  << endreq;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> Ecal PID has been disabled"  << endreq;
  }

  return hasEcalPID;
}

//=============================================================================
// Add Calo Brem info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addBrem( LHCb::ProtoParticle * proto ) const
{
  LHCb::Calo2Track::ITrAccTable::Range                 aRange;
  LHCb::Calo2Track::ITrHypoTable2D::Range              hRange;
  LHCb::Calo2Track::ITrEvalTable::Range                vRange;

  bool hasBremPID = false;
  if (m_BremPID)
  {
    aRange = m_InBremTable -> relations ( proto->track() ) ;

    if ( !aRange.empty() )
    {
      hasBremPID = aRange.front().to();
      if ( hasBremPID )
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> The Brem. extrapolated line is in Ecal acceptance"  << endreq;
        proto->addInfo(ProtoParticle::InAccBrem , true );

        // Get the highest weight associated brem. CaloHypo (3D matching)
        hRange = m_bremTrTable ->relations ( proto->track() ) ;
        if ( !hRange.empty() )
        {
          proto->addToCalo ( hRange.front().to() );
          proto->addInfo(ProtoParticle::CaloNeutralSpd, CaloSpd( hRange.front().to() ));
          proto->addInfo(ProtoParticle::CaloNeutralPrs, CaloPrs( hRange.front().to() ));
          proto->addInfo(ProtoParticle::CaloNeutralEcal, CaloEcal( hRange.front().to() ));
          proto->addInfo(ProtoParticle::CaloBremMatch , hRange.front().weight() );
        }

        // Get the BremChi2 (intermediate) estimator
        vRange = m_BremChi2Table -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::CaloBremChi2,  vRange.front().to() ); }

        // Get the Brem DLL(e)
        vRange = m_dlleBremTable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::BremPIDe , vRange.front().to() ); }

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> BremStrahlung PID : "
                    << " Chi2-Brem  =" <<  proto->info(ProtoParticle::CaloBremMatch, -999.)
                    << " BremChi2   =" <<  proto->info(ProtoParticle::CaloBremChi2, -999.)
                    << " Dlle (Brem) =" <<  proto->info(ProtoParticle::BremPIDe, -999.)
                    << " Spd Digits " <<  proto->info(ProtoParticle::CaloNeutralSpd, 0.)
                    << " Prs Digits " <<  proto->info(ProtoParticle::CaloNeutralPrs, 0.)
                    << " Ecal Cluster " <<  proto->info(ProtoParticle::CaloNeutralEcal, 0.)
                    << endreq;
      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> The Brem. extrapolated line is NOT in Ecal acceptance"  << endreq;
      }
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " ->  No entry for that track in the Brem acceptance table"  << endreq;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Brem PID has been disabled"  << endreq;
  }

  return hasBremPID;
}

//=============================================================================
// Add Calo Hcal info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addHcal( LHCb::ProtoParticle * proto ) const
{
  LHCb::Calo2Track::ITrAccTable::Range                 aRange;
  LHCb::Calo2Track::ITrEvalTable::Range                vRange;

  bool hasHcalPID = false;
  if (m_HcalPID)
  {
   aRange = m_InHcalTable -> relations ( proto->track() ) ;
    if ( !aRange.empty() ) 
    {
      hasHcalPID = aRange.front().to();
      if( hasHcalPID )
      {
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Hcal acceptance"  << endreq;
        proto->addInfo(ProtoParticle::InAccHcal, true );

        // Get the HcalE (intermediate) estimator
        vRange = m_HcalETable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::CaloHcalE,  vRange.front().to() ); }
        // Get the Hcal DLL(e)
        vRange = m_dlleHcalTable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::HcalPIDe , vRange.front().to() ); }

        // Get the Hcal DLL(mu)
        vRange = m_dllmuHcalTable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::HcalPIDmu , vRange.front().to() ); }

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Hcal PID  : "
                    << " HcalE      =" <<  proto->info(ProtoParticle::CaloHcalE, -999.)
                    << " Dlle (Hcal) =" <<  proto->info(ProtoParticle::HcalPIDe, -999.)
                    << " Dllmu (Hcal) =" <<  proto->info(ProtoParticle::HcalPIDmu, -999.)
                    << endreq;

      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Hcal acceptance"  << endreq;
      }
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Hcal acceptance table"  << endreq;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> Hcal PID has been disabled"  << endreq;
  }

  return hasHcalPID;
}

//=============================================================================
// Add Calo Prs info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addPrs( LHCb::ProtoParticle * proto ) const
{
  LHCb::Calo2Track::ITrAccTable::Range                 aRange;
  LHCb::Calo2Track::ITrEvalTable::Range                vRange;

  bool hasPrsPID = false;
  if (m_PrsPID)
  {
    aRange = m_InPrsTable -> relations ( proto->track() ) ;
    if( !aRange.empty() ) 
    {
      hasPrsPID = aRange.front().to();
      if( hasPrsPID )
      {
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Prs acceptance"  << endreq;
        proto->addInfo(ProtoParticle::InAccPrs , true );

        // Get the PrsE (intermediate) estimator
        vRange = m_PrsETable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::CaloPrsE,  vRange.front().to() ); }

        // Get the Prs DLL(e)
        vRange = m_dllePrsTable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::PrsPIDe , vRange.front().to() ); }

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Prs PID : "
                    << " PrsE       =" <<  proto->info(ProtoParticle::CaloPrsE, -999.)
                    << " Dlle (Prs)  =" <<  proto->info(ProtoParticle::PrsPIDe , -999.)
                    << endreq;

      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is NOT in Prs acceptance"  << endreq;
      }
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )verbose() << " -> No entry for that track in the Prs acceptance table"  << endreq;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )verbose() << " -> Prs PID has been disabled"  << endreq;
  }

  return hasPrsPID;
}

//=============================================================================
// Add Calo Spd info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addSpd( LHCb::ProtoParticle * proto ) const
{
  LHCb::Calo2Track::ITrAccTable::Range                 aRange;
  LHCb::Calo2Track::ITrEvalTable::Range                vRange;

  bool hasSpdPID = false;
  if (m_SpdPID)
  {
    aRange = m_InSpdTable -> relations ( proto->track() ) ;
    if ( !aRange.empty() ) 
    {
      hasSpdPID = aRange.front().to();
      if( hasSpdPID )
      {
        if ( msgLevel(MSG::VERBOSE) )verbose() << " -> The track is in Spd acceptance"  << endreq;
        proto->addInfo(ProtoParticle::InAccSpd , true );

        // Get the PrsE (intermediate) estimator
        vRange = m_SpdETable -> relations ( proto->track() ) ;
        if ( !vRange.empty() ) { proto->addInfo(ProtoParticle::CaloSpdE,  vRange.front().to() ); }

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Spd PID : "
                    << " SpdE       =" <<  proto->info(ProtoParticle::CaloSpdE, -999.)
                    << endreq;

      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> The track is NOT in Spd acceptance"  << endreq;
      }
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> No entry for that track in the Spd acceptance table"  << endreq;
    }
  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Spd PID has been disabled"  << endreq;
  }

  return hasSpdPID;
}

//=============================================================================
// Add VELO dE/dx info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addVelodEdx( LHCb::ProtoParticle * proto ) const
{
  // get velo charge
  double veloNtks(0);
  bool OK(false);
  const StatusCode sc = m_velodEdx->nTracks( proto->track(), veloNtks );
  if ( sc.isSuccess() )
  {
    proto->addInfo ( ProtoParticle::VeloCharge, veloNtks );
    OK = true;
  }
  // return status
  return OK;
}

//=============================================================================
// Loads the RICH data
//=============================================================================
bool ChargedProtoPAlg::getRichData()
{
  // empty the map
  m_richMap.clear();

  // Do we have any RichPID results
  if ( !exist<RichPIDs>(m_richPath) )
  {
    Warning( "No RichPIDs at '"+m_richPath+"'", StatusCode::SUCCESS );
    return false;
  }

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

  return true;
}

//=============================================================================
// Loads the Muon data
//=============================================================================
bool ChargedProtoPAlg::getMuonData()
{
  // empty the map
  m_muonMap.clear();

  // Do we have any MuonPID results
  if ( !exist<MuonPIDs>(m_muonPath) )
  {
    Warning( "No MuonPIDs at '"+m_muonPath+"'", StatusCode::SUCCESS );
    return false;
  }

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

  return true;
}

//=============================================================================
// Loads the Calo Ecal data
//=============================================================================
bool ChargedProtoPAlg::getEcalData()
{
  using namespace LHCb::Calo2Track;

  const bool sc1 = loadCaloTable(m_InEcalTable,CaloIdLocation::InEcal);
  const bool sc2 = loadCaloTable(m_elecTrTable,CaloIdLocation::ElectronMatch);
  const bool sc3 = loadCaloTable(m_clusTrTable,CaloIdLocation::ClusterMatch);
  const bool sc4 = loadCaloTable(m_EcalChi2Table,CaloIdLocation::EcalChi2);
  const bool sc5 = loadCaloTable(m_EcalETable,CaloIdLocation::EcalE);
  const bool sc6 = loadCaloTable(m_ClusChi2Table,CaloIdLocation::ClusChi2);
  const bool sc7 = loadCaloTable(m_dlleEcalTable,CaloIdLocation::EcalPIDe);
  const bool sc8 = loadCaloTable(m_dllmuEcalTable,CaloIdLocation::EcalPIDmu);
  const bool sc  = sc1 && sc2 && sc3 && sc4 && sc5 && sc6 && sc7 && sc8;
  if ( sc ) debug() << "Ecal PID SUCCESSFULLY LOADED" << endreq;

  return sc;
}

//=============================================================================
// Loads the Calo Brem data
//=============================================================================
bool ChargedProtoPAlg::getBremData()
{
  using namespace LHCb::Calo2Track;

  const bool sc1 = loadCaloTable(m_InBremTable,CaloIdLocation::InBrem);
  const bool sc2 = loadCaloTable(m_bremTrTable,CaloIdLocation::BremMatch);
  const bool sc3 = loadCaloTable(m_BremChi2Table,CaloIdLocation::BremChi2);
  const bool sc4 = loadCaloTable(m_dlleBremTable,CaloIdLocation::BremPIDe);
  const bool sc  = sc1 && sc2 && sc3 && sc4;

  if ( sc ) debug() << "BREM PID SUCCESSFULLY LOADED" << endreq;

  return sc;
}

//=============================================================================
// Loads the Calo Spd data
//=============================================================================
bool ChargedProtoPAlg::getSpdData()
{
  using namespace LHCb::Calo2Track;

  const bool sc1 = loadCaloTable(m_InSpdTable,CaloIdLocation::InSpd);
  const bool sc2 = loadCaloTable(m_SpdETable,CaloIdLocation::SpdE);
  const bool sc  = sc1 && sc2;

  if ( sc ) debug() << "SPD PID SUCCESSFULLY LOADED" << endreq;

  return sc;
}

//=============================================================================
// Loads the Calo Prs data
//=============================================================================
bool ChargedProtoPAlg::getPrsData()
{
  using namespace LHCb::Calo2Track;

  const bool sc1 = loadCaloTable(m_InPrsTable,CaloIdLocation::InPrs);
  const bool sc2 = loadCaloTable(m_PrsETable,CaloIdLocation::PrsE);
  const bool sc3 = loadCaloTable(m_dllePrsTable,CaloIdLocation::PrsPIDe);
  const bool sc  = sc1 && sc2 && sc3;

  if ( sc ) debug() << "PRS PID SUCCESSFULLY LOADED" << endreq;

  return sc;
}

//=============================================================================
// Loads the Calo Hcal data
//=============================================================================
bool ChargedProtoPAlg::getHcalData()
{
  using namespace LHCb::Calo2Track;

  const bool sc1 = loadCaloTable(m_InHcalTable,CaloIdLocation::InHcal);
  const bool sc2 = loadCaloTable(m_HcalETable,CaloIdLocation::HcalE);
  const bool sc3 = loadCaloTable(m_dlleHcalTable,CaloIdLocation::HcalPIDe);
  const bool sc4 = loadCaloTable(m_dllmuHcalTable,CaloIdLocation::HcalPIDmu);
  const bool sc  = sc1 && sc2 && sc3 && sc4;

  if ( sc ) debug() << "HCAL PID SUCCESSFULLY LOADED" << endreq;

  return sc;
}

double ChargedProtoPAlg::CaloSpd  ( const LHCb::CaloHypo*  hypo  )  const
{
  if ( NULL == hypo ) return 0;
  LHCb::CaloHypo::Digits digits = hypo->digits();
  LHCb::CaloDataFunctor::IsFromCalo< LHCb::CaloDigit* > isSpd( DeCalorimeterLocation::Spd );
  LHCb::CaloHypo::Digits::iterator it = std::stable_partition ( digits.begin(),digits.end(),isSpd );
  return ( it == digits.begin() ) ? 0. : +1.;
}

double ChargedProtoPAlg::CaloPrs  ( const LHCb::CaloHypo*  hypo  )  const
{
  if ( NULL == hypo) return 0;
  LHCb::CaloHypo::Digits digits = hypo->digits();
  LHCb::CaloDataFunctor::IsFromCalo< LHCb::CaloDigit* > isPrs( DeCalorimeterLocation::Prs );
  LHCb::CaloHypo::Digits::iterator it = std::stable_partition ( digits.begin(),digits.end(),isPrs );
  double CaloPrs = 0. ;
  for(LHCb::CaloHypo::Digits::iterator id = digits.begin(); id != it ; ++id){
    if(0 != *id)CaloPrs += (*id)->e();
  }
  return CaloPrs  ;
}

double ChargedProtoPAlg::CaloEcal  ( const LHCb::CaloHypo*  hypo  )  const
{
  if ( NULL == hypo) return 0;
  SmartRefVector<LHCb::CaloCluster> clusters = hypo->clusters();
  if ( clusters.empty() ) return 0.;
  SmartRefVector<LHCb::CaloCluster>::iterator icluster = clusters.begin();
  LHCb::CaloCluster* cluster = *icluster;
  if ( NULL == cluster) return 0;
  return cluster->e();
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

    const double protoECAL = 100 * ( tally.selTracks>0 ? (double)tally.ecalTracks/(double)tally.selTracks : 0 );
    const double protoBREM = 100 * ( tally.selTracks>0 ? (double)tally.bremTracks/(double)tally.selTracks : 0 );
    const double protoSPD  = 100 * ( tally.selTracks>0 ? (double)tally.spdTracks/(double)tally.selTracks : 0 );
    const double protoPRS  = 100 * ( tally.selTracks>0 ? (double)tally.prsTracks/(double)tally.selTracks : 0 );
    const double protoHCAL = 100 * ( tally.selTracks>0 ? (double)tally.hcalTracks/(double)tally.selTracks : 0 );

    const double protoMUON = 100 * ( tally.selTracks>0 ? (double)tally.muonTracks/(double)tally.selTracks : 0 );

    const double protoVELO = 100 * ( tally.selTracks>0 ? (double)tally.velodEdxTracks/(double)tally.selTracks : 0 );

    info() << " -> Created. = " << protos << " ProtoParticles/event : Track Sel. Eff. "
           << tkSel << "%" << endreq;
    info() << "  -> " << protoRICH << "% with RichPID information" << endreq;
    info() << "  -> " << protoMUON << "% with MuonPID information" << endreq;
    info() << "  -> " << protoECAL << "% with CaloPID ECAL information" << endreq;
    info() << "  -> " << protoBREM << "% with CaloPID BREM information" << endreq;
    info() << "  -> " << protoSPD  << "% with CaloPID SPD information" << endreq;
    info() << "  -> " << protoPRS  << "% with CaloPID PRS information" << endreq;
    info() << "  -> " << protoHCAL << "% with CaloPID HCAL information" << endreq;
    info() << "  -> " << protoVELO << "% with Velo dE/dx charge information" << endreq;
  }

  // execute base class finalise and return
  return GaudiAlgorithm::finalize();
}

//=============================================================================
