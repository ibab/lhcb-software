
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleMaker.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleMaker
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleMaker.cpp,v 1.2 2009-09-03 11:09:22 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "ChargedProtoParticleMaker.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleMaker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleMaker::ChargedProtoParticleMaker( const std::string& name,
                                                      ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
{

  // context specific locations
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_tracksPath = LHCb::TrackLocation::HltForward;
    m_protoPath  = LHCb::ProtoParticleLocation::HltCharged;
    m_trSelType  = "TrackSelector";
  }
  else
  {
    m_tracksPath = LHCb::TrackLocation::Default;
    m_protoPath  = LHCb::ProtoParticleLocation::Charged;
    m_trSelType  = "DelegatingTrackSelector";
  }

  // track selector type
  declareProperty( "TrackSelectorType", m_trSelType );

  // Input data
  declareProperty( "InputTrackLocation", m_tracksPath );

  // output data
  declareProperty( "OutputProtoParticleLocation", m_protoPath );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleMaker::~ChargedProtoParticleMaker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoParticleMaker::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  // get an instance of the track selector
  m_trSel = tool<ITrackSelector>( m_trSelType, "TrackSelector", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleMaker::execute()
{
  // Load the Track objects (manditory - should be there for each event)
  if ( !exist<LHCb::Tracks>(m_tracksPath) )
  {
    setFilterPassed(false);
    return Warning( "No Tracks at '"+m_tracksPath+"'", StatusCode::SUCCESS );
  }
  const LHCb::Tracks * tracks = get<LHCb::Tracks>( m_tracksPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << tracks->size()
            << " Tracks from " << m_tracksPath << endmsg;
  }

  // ProtoParticle container
  LHCb::ProtoParticles * protos = NULL;
  if ( exist<LHCb::ProtoParticles>(m_protoPath) )
  {
    // Get existing container, clear, and reuse
    Warning( "Existing ProtoParticle container at " + m_protoPath +
             " found -> Will replace", StatusCode::SUCCESS ).ignore();
    protos = get<LHCb::ProtoParticles>(m_protoPath);
    protos->clear();
  }
  else
  {
    // make new container and give to Gaudi
    protos = new LHCb::ProtoParticles();
    put ( protos, m_protoPath );
  }

  // Loop over tracks
  for ( LHCb::Tracks::const_iterator iTrack = tracks->begin();
        iTrack != tracks->end(); ++iTrack )
  {

    // Select tracks
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Trying Track " << (*iTrack)->key() << endmsg;
    if ( !m_trSel->accept(**iTrack) ) continue;
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Track selected " << (*iTrack)->key() << endmsg;
      verbose() << " -> Track type " << (*iTrack)->type() << endmsg;
      verbose() << " -> Track flag " << (*iTrack)->flag() << endmsg;
      verbose() << " -> Track charge " << (*iTrack)->charge() << endmsg;
    }

    // Make a proto-particle
    LHCb::ProtoParticle* proto = new LHCb::ProtoParticle();

    // Insert into container, with same key as Track
    protos->insert( proto, (*iTrack)->key() );

    // Set track reference
    proto->setTrack( *iTrack );

    // Add minimal track info
    proto->addInfo( LHCb::ProtoParticle::TrackChi2PerDof, (*iTrack)->chi2PerDoF() );
    proto->addInfo( LHCb::ProtoParticle::TrackNumDof,     (*iTrack)->nDoF()       );
    proto->addInfo( LHCb::ProtoParticle::TrackHistory,    (*iTrack)->history()    );
    proto->addInfo( LHCb::ProtoParticle::TrackType,       (*iTrack)->type()       );

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Created ProtoParticle : " << *proto << endmsg;
    }

  }

  // return
  return StatusCode::SUCCESS;
}

//=============================================================================
