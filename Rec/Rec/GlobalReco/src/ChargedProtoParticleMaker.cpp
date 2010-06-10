
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleMaker.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleMaker
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleMaker.cpp,v 1.6 2010-04-02 10:50:09 jonrob Exp $
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
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_trSel        ( NULL )
{

  // context specific locations
  m_tracksPath.clear();
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_trSelType  = "TrackSelector";
  }
  else
  {
    m_tracksPath.push_back(LHCb::TrackLocation::Default);
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

  // Loop over tracks container
  setFilterPassed(false);
  for ( std::vector<std::string>::const_iterator c = m_tracksPath.begin() ;
        m_tracksPath.end() != c ; ++c )
  {
    // track location
    const std::string& loc = *c;

    // Load the Track objects (manditory - should be there for each event)
    if ( !exist<LHCb::Tracks>(loc) )
    {
      Warning( "No Tracks at '"+loc+"'", StatusCode::SUCCESS ).ignore();
      continue;
    }

    setFilterPassed(true);
    const LHCb::Tracks * tracks = get<LHCb::Tracks>( loc  );
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Successfully loaded " << tracks->size() << " Tracks from " << loc << endmsg;  
    
    int count = 0;
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
      proto->addInfo( LHCb::ProtoParticle::TrackP,          (*iTrack)->p()          );
      proto->addInfo( LHCb::ProtoParticle::TrackPt,         (*iTrack)->pt()         );
      
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << " -> Created ProtoParticle : " << *proto << endmsg;
      }
      count++;
    }
    counter(loc +" ==> " + m_protoPath )+= count;
  }
  // return
  return StatusCode::SUCCESS;
}

//=============================================================================
