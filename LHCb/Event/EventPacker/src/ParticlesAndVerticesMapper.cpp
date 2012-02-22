
// Include files
#include <iterator>
// From Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IJobOptionsSvc.h"
// local
#include "ParticlesAndVerticesMapper.h"

// ----------------------------------------------------------------------------
// Implementation file for class: ParticlesAndVerticesMapper
//
// 17/01/2012: Marco Clemencic
// ----------------------------------------------------------------------------

#define ON_VERBOSE if (UNLIKELY(msgLevel(MSG::VERBOSE)))
#define ON_DEBUG   if (UNLIKELY(msgLevel(MSG::DEBUG)))
#define LOG_VERBOSE ON_VERBOSE verbose()
#define LOG_DEBUG   ON_DEBUG debug()

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
ParticlesAndVerticesMapper::ParticlesAndVerticesMapper(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent)
  : base_class(type, name, parent)
{
  declareProperty( "UnpackerType",
                   m_unpackerType = "UnpackParticlesAndVertices" );
  declareProperty( "UnpackerOutputLevel", m_unpackersOutputLevel = -1 );
}

// ============================================================================
// Destructor
// ============================================================================
ParticlesAndVerticesMapper::~ParticlesAndVerticesMapper() {}

// ============================================================================
// Finalize
// ============================================================================
StatusCode ParticlesAndVerticesMapper::finalize()
{
  m_jos.reset(); // release JobOptionsSvc
  return GaudiTool::finalize();
}

// ============================================================================
// Initialize
// ============================================================================
StatusCode ParticlesAndVerticesMapper::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // No need to check because it's implicit in the base class initialization
  m_jos = serviceLocator()->service("JobOptionsSvc");

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
    const std::string algName = streamName(path) + "_Converter";

    // Add the configuration of algorithm instance to the JobOptionsSvc
    m_jos->addPropertyToCatalogue( algName,
                                   StringProperty("InputStream",streamRoot(path)) );
    if ( m_unpackersOutputLevel > 0 )
    {
      std::stringstream lvl; 
      lvl << m_unpackersOutputLevel;
      m_jos->addPropertyToCatalogue( algName, 
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

  const std::string retS = ( it != m_nodeTypeMap.end() ? it->second : "" );

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

    int key(0),linkID(0);

    // Load the packed Particles
    if ( exist<LHCb::PackedParticles>(streamR+LHCb::PackedParticleLocation::InStream) )
    {
      LHCb::PackedParticles* pparts =
        get<LHCb::PackedParticles>(streamR+LHCb::PackedParticleLocation::InStream);
      for ( std::vector<LHCb::PackedParticle>::iterator itP = pparts->data().begin();
            pparts->data().end() != itP; ++itP )
      {
        m_pack.indexAndKey64( (*itP).key, linkID, key );
        addPath( pparts->linkMgr()->link(linkID)->path() );
      }
    }

    // Load the vertices
    if ( exist<LHCb::PackedVertices>(streamR+LHCb::PackedVertexLocation::InStream) )
    {
      LHCb::PackedVertices* pverts =
        get<LHCb::PackedVertices>(streamR+LHCb::PackedVertexLocation::InStream);
      for ( std::vector<LHCb::PackedVertex>::iterator itV = pverts->data().begin();
            pverts->data().end() != itV; ++itV )
      {
        m_pack.indexAndKey64( (*itV).key, linkID, key );
        addPath( pverts->linkMgr()->link(linkID)->path() );
      }
    }

    // Load the Rec vertices
    if ( exist<LHCb::PackedRecVertices>(streamR+LHCb::PackedRecVertexLocation::InStream) )
    {
      LHCb::PackedRecVertices* pRecVerts =
        get<LHCb::PackedRecVertices>(streamR+LHCb::PackedRecVertexLocation::InStream);
      for ( std::vector<LHCb::PackedRecVertex>::iterator itV = pRecVerts->vertices().begin();
            pRecVerts->vertices().end() != itV; ++itV )
      {
        const int linkID = (*itV).container;
        addPath( pRecVerts->linkMgr()->link(linkID)->path() );
      }
    }

    // relations
    if ( exist<LHCb::PackedRelations>(streamR+LHCb::PackedRelationsLocation::InStream) )
    {
      LHCb::PackedRelations* prels =
        get<LHCb::PackedRelations>(streamR+LHCb::PackedRelationsLocation::InStream);
      for ( std::vector<LHCb::PackedRelation>::iterator itR = prels->relations().begin();
            prels->relations().end() != itR; ++itR )
      {
        m_pack.indexAndKey64( (*itR).container, linkID, key );
        addPath( prels->linkMgr()->link(linkID)->path() );
      }
    }

    // Particle2LHCbID
    if ( exist<LHCb::PackedParticle2Ints>(streamR+LHCb::PackedParticle2IntsLocation::InStream) )
    {
      LHCb::PackedParticle2Ints* pPartIds =
        get<LHCb::PackedParticle2Ints>(streamR+LHCb::PackedParticle2IntsLocation::InStream);
      for ( std::vector<LHCb::PackedParticle2Int>::iterator itL = pPartIds->relations().begin();
            pPartIds->relations().end() != itL; ++itL )
      {
        m_pack.indexAndKey64( (*itL).key, linkID, key );
        addPath( pPartIds->linkMgr()->link(linkID)->path() );
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

std::string
ParticlesAndVerticesMapper::streamName( const std::string & path ) const
{
  std::string tmp = path;
  if ( path.substr(0,7) == "/Event/" ) { tmp = tmp.substr(7); }
  const std::string::size_type slash = tmp.find_first_of( "/" );
  return tmp.substr(0,slash);
}

// ============================================================================

DECLARE_TOOL_FACTORY(ParticlesAndVerticesMapper)

// ============================================================================
