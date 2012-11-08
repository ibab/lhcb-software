// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/RecVertex.h"
// local
#include "UnpackParticlesAndVertices.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackParticlesAndVertices
//
// 2012-01-23 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackParticlesAndVertices::
UnpackParticlesAndVertices( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputStream", m_inputStream = "/Event/" );
  declareProperty( "PostFix",     m_postFix     = "" );
}

//=============================================================================
// Destructor
//=============================================================================
UnpackParticlesAndVertices::~UnpackParticlesAndVertices() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackParticlesAndVertices::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute"  << endmsg;

  int nbContainer = 0;
  int nbPart      = 0;
  int prevLink    = -1;

  //=================================================================
  //== Process the Particles
  //=================================================================
  LHCb::Particles* parts = NULL;
  const LHCb::ParticlePacker pPacker(*dynamic_cast<GaudiAlgorithm*>(this));
  LHCb::PackedParticles* pparts =
    getIfExists<LHCb::PackedParticles>( m_inputStream + LHCb::PackedParticleLocation::InStream );
  if ( NULL != pparts )
  {
    for ( std::vector<LHCb::PackedParticle>::iterator itP = pparts->data().begin();
          pparts->data().end() != itP; ++itP )
    {
      const LHCb::PackedParticle& ppart = *itP;
      int key(0),linkID(0);
      m_pack.indexAndKey64( ppart.key, linkID, key );
      if ( linkID != prevLink )
      {
        prevLink = linkID;
        const std::string & containerName = pparts->linkMgr()->link( linkID )->path() + m_postFix;
        // Check to see if container alrady exists. If it does, unpacking has already been run this
        // event so quit
        if ( exist<LHCb::Particles>(containerName) )
        {
          if ( msgLevel(MSG::DEBUG) )
            debug() << " -> " << containerName << " exists" << endmsg;
          return StatusCode::SUCCESS;
        }
        parts = new LHCb::Particles();
        put( parts, containerName );
        ++nbContainer;
      }

      // Make new object and insert into the output container
      LHCb::Particle* part = new LHCb::Particle();
      parts->insert( part, key );
      ++nbPart;

      // Unpack the physics info
      pPacker.unpack( ppart, *part, *pparts, *parts );

    }
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Retrieved " << nbPart << " Particles in " << nbContainer << " containers" << endmsg;
  }

  //=================================================================
  //== Process the vertices
  //=================================================================
  int nbVertContainer = 0;
  int nbVert = 0;
  prevLink = -1;
  LHCb::Vertices* verts = NULL;
  const LHCb::VertexPacker vPacker(*dynamic_cast<GaudiAlgorithm*>(this));
  LHCb::PackedVertices* pverts =
    getIfExists<LHCb::PackedVertices>( m_inputStream + LHCb::PackedVertexLocation::InStream );
  if ( NULL != pverts )
  {
    for ( std::vector<LHCb::PackedVertex>::iterator itV = pverts->data().begin();
          pverts->data().end() != itV; ++itV )
    {
      const LHCb::PackedVertex& pvert = *itV;
      int key(0),linkID(0);
      m_pack.indexAndKey64( pvert.key, linkID, key );
      if ( linkID != prevLink )
      {
        prevLink = linkID;
        const std::string & containerName = pverts->linkMgr()->link( linkID )->path() + m_postFix;
        verts = new LHCb::Vertices();
        put( verts, containerName );
        ++nbVertContainer;
      }
      //== Construct with verticle ID and key.
      LHCb::Vertex* vert = new LHCb::Vertex();
      verts->insert( vert, key );
      ++nbVert;

      // unpack the physics info
      vPacker.unpack( pvert, *vert, *pverts, *verts );
    }
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Retrieved " << nbVert << " vertices in " << nbVertContainer << " containers"
            << endmsg;
  }

  //=================================================================
  //== Process the RecVertices
  //=================================================================
  int nbRecVertContainer = 0;
  int nbRecVert = 0;
  prevLink = -1;
  LHCb::RecVertices* recVerts = NULL;
  const LHCb::RecVertexPacker rvPacker(*dynamic_cast<GaudiAlgorithm*>(this));
  LHCb::PackedRecVertices* pRecVerts =
    getIfExists<LHCb::PackedRecVertices>( m_inputStream + LHCb::PackedRecVertexLocation::InStream );
  if ( NULL != pRecVerts )
  {
    for ( std::vector<LHCb::PackedRecVertex>::iterator itV = pRecVerts->vertices().begin();
          pRecVerts->vertices().end() != itV; ++itV )
    {
      const LHCb::PackedRecVertex& pRecVert = *itV;
      const int key    = pRecVert.key;
      const int linkID = pRecVert.container;
      if ( linkID != prevLink )
      {
        prevLink = linkID;
        const std::string & containerName = pRecVerts->linkMgr()->link(linkID)->path() + m_postFix;
        recVerts = new LHCb::RecVertices();
        put( recVerts, containerName );
        ++nbRecVertContainer;
      }

      //== Construct with RecVertex ID and key.
      LHCb::RecVertex* recVert = new LHCb::RecVertex( key );
      recVerts->add( recVert );
      ++nbRecVert;

      // Physics Info
      rvPacker.unpack( pRecVert, *recVert, *pRecVerts, *recVerts );
    }
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Retrieved " << nbRecVert << " RecVerticess in " << nbRecVertContainer << " containers"
            << endmsg;
  }

  //=================================================================
  //== Process the P2V relations
  //=================================================================
  unpackP2PRelations<LHCb::Particle,LHCb::VertexBase,LHCb::Particles,const LHCb::RecVertices>(m_inputStream+LHCb::PackedRelationsLocation::InStream);

  //=================================================================
  //== Process the P2MCP relations
  //=================================================================
  unpackP2PRelations<LHCb::Particle,LHCb::MCParticle,LHCb::Particles,const LHCb::MCParticles>(m_inputStream+LHCb::PackedRelationsLocation::P2MCP);

  //=================================================================
  //== Process the P2Int relations
  //=================================================================
  unpackP2IntRelations<LHCb::Particle,int,LHCb::Particles>(m_inputStream+LHCb::PackedRelationsLocation::P2Int);

  return StatusCode::SUCCESS;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackParticlesAndVertices )

//=============================================================================
