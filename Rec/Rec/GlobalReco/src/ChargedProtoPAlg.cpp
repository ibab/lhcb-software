
//-----------------------------------------------------------------------------
/** @file ChargedProtoPAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoPAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoPAlg.cpp,v 1.33 2006-06-18 14:11:21 jonrob Exp $
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
    m_nEvts        ( 0 ),
    m_nTracks      ( 0 ),
    m_nTracksRich  ( 0 ),
    m_nTracksMuon  ( 0 )
{
  // Input data
  declareProperty( "InputTrackLocation",
                   m_tracksPath = TrackLocation::Default );
  declareProperty( "InputRichPIDLocation",
                   m_richPath = RichPIDLocation::Default );
  declareProperty( "InputMuonPIDLocation",
                   m_muonPath = MuonPIDLocation::Default );

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

  // Create the ProtoParticle container
  m_protos = new ProtoParticles();
  // give to Gaudi
  put ( protos(), m_protoPath );

  // tallies
  unsigned long nTracks(0), nTracksRich(0), nTracksMuon(0);

  // Loop over tracks
  for ( Tracks::const_iterator iTrack = tracks->begin();
        iTrack != tracks->end(); ++iTrack )
  {
    // Select tracks
    if ( !m_trSel->accept(**iTrack) ) continue;

    verbose() << "Trying Track " << (*iTrack)->key() << endreq;

    // Count tracks
    ++nTracks;

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
    bool hasRICHInfo(false), hasMUONInfo(false);

    // Combined DLL data object for this proto
    CombinedLL combLL(0);

    // Add RICH info
    if ( addRich(proto,combLL) ) { hasRICHInfo = true; ++nTracksRich; }

    // Add MUON info
    if ( addMuon(proto,combLL) ) { hasMUONInfo = true; ++nTracksMuon; }

    // Add CALO info (To do)

    // Add Velo dE/dx info (To do)

    // has any PID info been added ?
    if ( hasRICHInfo || hasMUONInfo )
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
  m_nTracks     += nTracks;
  m_nTracksRich += nTracksRich;
  m_nTracksMuon += nTracksMuon;

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << nTracks << " ProtoParticles : " << nTracksRich
            << " with RICH info " << nTracksMuon << " with MUON info" << endreq;
  }

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
//  Finalize
//=============================================================================
StatusCode ChargedProtoPAlg::finalize()
{
  // summary printout

  const double protoPerEvent 
    =       ( m_nEvts>0   ? (double)m_nTracks/(double)m_nEvts       : 0 );
  const double protoRICH     
    = 100 * ( m_nTracks>0 ? (double)m_nTracksRich/(double)m_nTracks : 0 );
  const double protoMUON     
    = 100 * ( m_nTracks>0 ? (double)m_nTracksMuon/(double)m_nTracks : 0 );

  // print out summary info
  info() << "==================================================" << endreq;
  info() << "        ProtoParticle Summary : " << m_nEvts << " events" << endreq;
  info() << "--------------------------------------------------" << endreq;

  info() << "    Created " << protoPerEvent << " ProtoParticles / event" << endreq;
  info() << "      -> " << protoRICH << "% with RichPID information" << endreq;
  info() << "      -> " << protoMUON << "% with MuonPID information" << endreq;

  info() << "--------------------------------------------------" << endreq;

  // execute base class finalise and return
  return GaudiAlgorithm::finalize();
}

//=============================================================================
