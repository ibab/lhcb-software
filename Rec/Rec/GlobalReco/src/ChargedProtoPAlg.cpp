
//-----------------------------------------------------------------------------
/** @file ChargedProtoPAlg.cpp
*
* Implementation file for algorithm ChargedProtoPAlg
*
* CVS Log :-
* $Id: ChargedProtoPAlg.cpp,v 1.26 2006-03-30 20:46:47 jonrob Exp $
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
    m_trSel        ( NULL )
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
  m_trSel = tool<ITrackSelector>( "TrackSelector",
				  "TrackSelector",
                                  this );
				    
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoPAlg::execute()
{

  // Create the ProtoParticle container
  m_protos = new ProtoParticles();
  // give to Gaudi
  put ( protos(), m_protoPath );

  // Load the Track objects (manditory - should be there for each event)
  if ( !exist<Tracks>(m_tracksPath) )
  {
    return Warning( "No Tracks at '"+m_tracksPath+"'", StatusCode::SUCCESS );
  }
  const Tracks * tracks = get<Tracks>( m_tracksPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << tracks->size() 
	    << " Tracks from " << m_tracksPath << endreq;
  }

  // Load the RichPIDs (manditory - should be there for each event)
  if ( getRichData().isFailure() ) 
    return Warning( "No RichPIDs at '"+m_richPath+"'", StatusCode::SUCCESS );

  // Load the MuonPIDs (manditory - should be there for each event)
  if ( getMuonData().isFailure() )
    return Warning( "No MuonPIDs at '"+m_muonPath+"'", StatusCode::SUCCESS );

  // Loop over tracks
  for ( Tracks::const_iterator iTrack = tracks->begin(); 
	iTrack != tracks->end(); ++iTrack )
  {
    // Select tracks 
    // ( work needed in the tool to add all the selection cuts we need )
    if ( !m_trSel->accept(**iTrack) ) continue;

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

    bool hasPIDInfo ( false );

    // Add RICH info
    if ( addRich(proto) ) { hasPIDInfo = true; }

    // Add MUON info
    if ( addMuon(proto) ) { hasPIDInfo = true; }

    // Add CALO info (To do)

    // Add Velo dE/dx info (To do)

    // has any PID info been added ?
    if ( !hasPIDInfo ) { proto->addInfo(ProtoParticle::NoPID,1); }

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Add RICH info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addRich( ProtoParticle * proto )
{
  // Does this track have a RICH PID result ?
  TrackToRichPID::const_iterator iR = m_richMap.find( proto->track() );
  if ( m_richMap.end() == iR ) return false;

  // RichPID for this track is found, so save data

  const RichPID * richPID = (*iR).second;
  
  // reference to RichPID object
  proto->setRichPID( richPID );

  // Store the PID info
  proto->addInfo( ProtoParticle::RichElDLL, richPID->particleDeltaLL(Rich::Electron) );
  proto->addInfo( ProtoParticle::RichMuDLL, richPID->particleDeltaLL(Rich::Muon) );
  proto->addInfo( ProtoParticle::RichPiDLL, richPID->particleDeltaLL(Rich::Pion) );
  proto->addInfo( ProtoParticle::RichKaDLL, richPID->particleDeltaLL(Rich::Kaon) );
  proto->addInfo( ProtoParticle::RichPrDLL, richPID->particleDeltaLL(Rich::Proton) );
  // Store History
  proto->addInfo( ProtoParticle::RichPIDStatus, richPID->pidResultCode() );

  return true;
}

//=============================================================================
// Add MUON info to the protoparticle
//=============================================================================
bool ChargedProtoPAlg::addMuon( LHCb::ProtoParticle * proto )
{
  // Does this track have a RICH PID result ?
  TrackToMuonPID::const_iterator iM = m_muonMap.find( proto->track() );
  if ( m_muonMap.end() == iM ) return false;

  // MuonPID for this track is found, so save data

  const MuonPID * muonPID = (*iM).second;

  // reference to MuonPID object
  proto->setMuonPID( muonPID );

  // Store the PID info
  proto->addInfo(ProtoParticle::MuonMuLL,      muonPID->MuonLLMu() );
  proto->addInfo(ProtoParticle::MuonBkgLL,     muonPID->MuonLLBg() );
  proto->addInfo(ProtoParticle::MuonNShared,   muonPID->nShared()  );
  // Store History
  proto->addInfo(ProtoParticle::MuonPIDStatus, muonPID->Status()   );

  return true;
}

//=============================================================================
// Loads the RICH data
//=============================================================================
StatusCode ChargedProtoPAlg::getRichData()
{
  // Do we have any RichPID results
  if ( !exist<RichPIDs>(m_richPath) ) return StatusCode::FAILURE;
  // yes, so load them
  const RichPIDs * richpids = get<RichPIDs>( m_richPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << richpids->size() 
	    << " RichPIDs from " << m_richPath << endreq;
  }
  // refresh the reverse mapping
  m_richMap.clear();
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
  // Do we have any MuonPID results
  if ( !exist<MuonPIDs>(m_muonPath) ) return StatusCode::FAILURE;
   // yes, so load them
  const MuonPIDs * muonpids = get<MuonPIDs>( m_muonPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << muonpids->size() 
	    << " MuonPIDs from " << m_muonPath << endreq;
  }
   // refresh the reverse mapping
  m_muonMap.clear();
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

  return GaudiAlgorithm::finalize();
}

//=============================================================================
