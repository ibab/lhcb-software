
// Include files
#include <iterator>
// local
#include "ParticlesAndVerticesMapper.h"

// ----------------------------------------------------------------------------

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
ParticlesAndVerticesMapper::
ParticlesAndVerticesMapper( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : base_class( type, name, parent )
{
  declareProperty( "UnpackerType",
                   m_unpackerType = "UnpackParticlesAndVertices" );
  declareProperty( "UnpackerOutputLevel", m_unpackersOutputLevel = -1 );
  //setProperty( "OutputLevel", 1 );
}

// ============================================================================
// Destructor
// ============================================================================
ParticlesAndVerticesMapper::~ParticlesAndVerticesMapper() {}

// ============================================================================
// Initialize
// ============================================================================
StatusCode ParticlesAndVerticesMapper::initialize()
{
  const StatusCode sc = MapperToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Incident service
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return sc;
}

// ============================================================================

void ParticlesAndVerticesMapper::handle ( const Incident& )
{
  // Only one incident type registered, so do not bother to check type
  m_streamsDone.clear();
}

// ============================================================================

Gaudi::Utils::TypeNameString
ParticlesAndVerticesMapper::algorithmForPath( const std::string & path )
{
  LOG_VERBOSE << "ParticlesAndVerticesMapper::algorithmForPath '"
              << path << "'" << endmsg;

  updateNodeTypeMap(path);

  // Is this path in the list of output locations this packer can create
  if ( pathIsHandled(path) )
  {
    // Choose a unique name for the algorithm instance
    const std::string algName = streamName(path) + "_PsAndVsUnpack";

    // Add the configuration of algorithm instance to the JobOptionsSvc
    joSvc()->addPropertyToCatalogue( algName,
                                     StringProperty("InputStream",streamRoot(path)) );
    if ( m_unpackersOutputLevel > 0 )
    {
      std::stringstream lvl;
      lvl << m_unpackersOutputLevel;
      joSvc()->addPropertyToCatalogue( algName,
                                       StringProperty("OutputLevel",lvl.str()));
    }

    // Return the algorithm type/name.
    LOG_VERBOSE << " -> Use algorithm type '" << m_unpackerType << "'"
                << " name '" << algName << "'"
                << endmsg;
    return Gaudi::Utils::TypeNameString(algName,m_unpackerType);
  }

  return "";
}

// ============================================================================

std::string
ParticlesAndVerticesMapper::nodeTypeForPath( const std::string & path )
{
  updateNodeTypeMap(path);

  NodeTypeMap::const_iterator it = m_nodeTypeMap.find( fixPath(path) );

  const std::string& retS = ( it != m_nodeTypeMap.end() ? it->second : "" );

  LOG_VERBOSE << "ParticlesAndVerticesMapper::nodeTypeForPath '"
              << path << "' NodeType '" << retS << "'" << endmsg;

  return retS;
}

// ============================================================================

void ParticlesAndVerticesMapper::updateNodeTypeMap( const std::string & path )
{
  // The stream TES root
  const std::string streamR = streamRoot(path);

  if ( !m_streamsDone[streamR] )
  {
    m_streamsDone[streamR] = true;
    LOG_VERBOSE << "ParticlesAndVerticesMapper::updateNodeTypeMap '" << path << "'" << endmsg;

    int key(0), linkID(0);

    // Load the packed Tracks
    {
      const LHCb::PackedTracks * ptracks =
        getIfExists<LHCb::PackedTracks>(evtSvc(),streamR+LHCb::PackedTrackLocation::InStream);
      if ( NULL != ptracks )
      {
        for ( const auto& P : ptracks->tracks() )
        {
          m_pack.indexAndKey64( P.key, linkID, key );
          addPath( ptracks->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Load the packed MuonPIDs
    {
      const LHCb::PackedMuonPIDs * ppids =
        getIfExists<LHCb::PackedMuonPIDs>(evtSvc(),streamR+LHCb::PackedMuonPIDLocation::InStream);
      if ( NULL != ppids )
      {
        for ( const auto& P : ppids->data() )
        {
          m_pack.indexAndKey64( P.key, linkID, key );
          addPath( ppids->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Load the packed RichPIDs
    {
      const LHCb::PackedRichPIDs * ppids =
        getIfExists<LHCb::PackedRichPIDs>(evtSvc(),streamR+LHCb::PackedRichPIDLocation::InStream);
      if ( NULL != ppids )
      {
        for ( const auto& P : ppids->data() )
        {
          m_pack.indexAndKey64( P.key, linkID, key );
          addPath( ppids->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Load the packed ProtoParticles
    {
      const LHCb::PackedProtoParticles * pprotos =
        getIfExists<LHCb::PackedProtoParticles>(evtSvc(),streamR+LHCb::PackedProtoParticleLocation::InStream);
      if ( NULL != pprotos )
      {
        for ( const auto& P : pprotos->protos() )
        {
          m_pack.indexAndKey64( P.key, linkID, key );
          addPath( pprotos->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Load the packed Particles
    {
      const LHCb::PackedParticles* pparts =
        getIfExists<LHCb::PackedParticles>(evtSvc(),streamR+LHCb::PackedParticleLocation::InStream);
      if ( NULL != pparts )
      {
        for ( const auto& P : pparts->data() )
        {
          m_pack.indexAndKey64( P.key, linkID, key );
          addPath( pparts->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Load the vertices
    {
      const LHCb::PackedVertices* pverts =
        getIfExists<LHCb::PackedVertices>(evtSvc(),streamR+LHCb::PackedVertexLocation::InStream);
      if ( NULL != pverts )
      {
        for ( const auto& V : pverts->data() )
        {
          m_pack.indexAndKey64( V.key, linkID, key );
          addPath( pverts->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Load the packed FlavourTags
    {
      const LHCb::PackedFlavourTags* pfts =
        getIfExists<LHCb::PackedFlavourTags>(evtSvc(),streamR+LHCb::PackedFlavourTagLocation::InStream);
      if ( NULL != pfts )
      {
        for ( const auto& FT : pfts->data() )
        {
          m_pack.indexAndKey64( FT.key, linkID, key );
          addPath( pfts->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Load the Rec vertices
    {
      const LHCb::PackedRecVertices* pRecVerts =
        getIfExists<LHCb::PackedRecVertices>(evtSvc(),streamR+LHCb::PackedRecVertexLocation::InStream);
      if ( NULL != pRecVerts )
      {
        for ( const auto& V : pRecVerts->vertices() )
        {
          addPath( pRecVerts->linkMgr()->link(V.container)->path() );
        }
      }
    }

    // Particle 2 Vertex relations
    {
      const LHCb::PackedRelations* prels =
        getIfExists<LHCb::PackedRelations>(evtSvc(),streamR+LHCb::PackedRelationsLocation::InStream);
      if ( NULL != prels )
      {
        for ( const auto& R : prels->relations() )
        {
          m_pack.indexAndKey64( R.container, linkID, key );
          addPath( prels->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Particle 2 MCParticle relations
    {
      const LHCb::PackedRelations* prelsMC =
        getIfExists<LHCb::PackedRelations>(evtSvc(),streamR+LHCb::PackedRelationsLocation::P2MCP);
      if ( NULL != prelsMC )
      {
        for ( const auto& R : prelsMC->relations() )
        {
          m_pack.indexAndKey64( R.container, linkID, key );
          addPath( prelsMC->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // ProtoParticle 2 MCParticle weighted relations
    {
      const LHCb::PackedWeightedRelations* pprelsMC =
        getIfExists<LHCb::PackedWeightedRelations>(evtSvc(),streamR+LHCb::PackedWeightedRelationsLocation::PP2MCP);
      if ( NULL != pprelsMC )
      {
        for ( const auto& R : pprelsMC->relations() )
        {
          m_pack.indexAndKey64( R.container, linkID, key );
          addPath( pprelsMC->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Particle 2 Int relations
    {
      const LHCb::PackedRelations* prelsInt =
        getIfExists<LHCb::PackedRelations>(evtSvc(),streamR+LHCb::PackedRelationsLocation::P2Int);
      if ( NULL != prelsInt )
      {
        for ( const auto& R : prelsInt->relations() )
        {
          m_pack.indexAndKey64( R.container, linkID, key );
          addPath( prelsInt->linkMgr()->link(linkID)->path() );
        }
      }
    }

    // Load the packed RelatedInfo relations
    {
      const LHCb::PackedRelatedInfoRelations* pRelInfos =
        getIfExists<LHCb::PackedRelatedInfoRelations>(evtSvc(),streamR+LHCb::PackedRelatedInfoLocation::InStream);
      if ( NULL != pRelInfos )
      {
        for ( const auto& cont : pRelInfos->containers() )
        {
          m_pack.indexAndKey64( cont.reference, linkID, key );
          addPath( pRelInfos->linkMgr()->link(linkID)->path() );
        }
      }
    }

  }

}

// ============================================================================

void ParticlesAndVerticesMapper::addPath( const std::string & path )
{
  // Make sure paths start with /Event/
  const std::string npath = fixPath(path);

  // if not already there, add.
  if ( m_nodeTypeMap.find(npath) == m_nodeTypeMap.end() )
  {
    LOG_VERBOSE << " -> Path " << npath << endmsg;

    // Main path
    m_nodeTypeMap[npath] = "";

    // Data Node paths ...
    std::string tmp = npath;
    std::string::size_type slash = tmp.find_last_of("/");
    while ( !tmp.empty() && slash != std::string::npos )
    {
      tmp = tmp.substr(0,slash);
      if ( !tmp.empty() )
      {
        LOG_VERBOSE << "  -> Node " << tmp << endmsg;
        m_nodeTypeMap[tmp] = "DataObject";
      }
      slash = tmp.find_last_of("/");
    }
  }

}

// ============================================================================

DECLARE_TOOL_FACTORY(ParticlesAndVerticesMapper)

// ============================================================================
