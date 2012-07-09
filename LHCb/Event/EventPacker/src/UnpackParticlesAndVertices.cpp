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
  int nbPart = 0;
  int prevLink = -1;

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

  //=================================================================
  //== Process the relations
  //=================================================================
  int nbRelContainer = 0;
  int nbRel = 0;
  RELATION* rels = NULL;
  LHCb::PackedRelations* prels =
    getIfExists<LHCb::PackedRelations>( m_inputStream + LHCb::PackedRelationsLocation::InStream );
  if ( NULL != prels )
  {
    LHCb::PackedRelations* prels =
      get<LHCb::PackedRelations>( m_inputStream + LHCb::PackedRelationsLocation::InStream );
    for ( std::vector<LHCb::PackedRelation>::iterator itR = prels->relations().begin();
          prels->relations().end() != itR; ++itR )
    {
      const LHCb::PackedRelation& prel = *itR;
      int indx = prel.container >> 32;
      const std::string & containerName = prels->linkMgr()->link( indx )->path() + m_postFix;
      rels = new RELATION();
      put( rels, containerName );
      ++nbRelContainer;
      LHCb::Particles* srcContainer = NULL;
      int prevSrcLink = -1;
      DataObject* dstContainer = NULL;
      int prevDstLink = -1;
      for ( int kk = prel.start;  prel.end > kk; ++kk )
      {
        int srcLink(0), srcKey(0);
        m_pack.indexAndKey64( prels->sources()[kk], srcLink, srcKey );
        if ( srcLink != prevSrcLink )
        {
          prevSrcLink = srcLink;
          const std::string & srcName = prels->linkMgr()->link( srcLink )->path();
          srcContainer = get<LHCb::Particles>( srcName );
        }
        LHCb::Particle* from = srcContainer->object( srcKey );
        int dstLink(0), dstKey(0);
        m_pack.indexAndKey64( prels->dests()[kk], dstLink, dstKey );
        if ( dstLink != prevDstLink )
        {
          prevDstLink = dstLink;
          const std::string & dstName = prels->linkMgr()->link( dstLink )->path();
          dstContainer = get<DataObject>( dstName );
        }
        const LHCb::RecVertices * _to = dynamic_cast<LHCb::RecVertices*>(dstContainer);
        LHCb::VertexBase* to = ( _to ? _to->object(dstKey) : NULL );
        if ( !to ) info() << "Unknown objec: Container type " << (dstContainer->clID()>>16)
                          << "+" << (dstContainer->clID()&0xFFFF)
                          << " key " << dstKey << endmsg;
        rels->relate( from, to );
        ++nbRel;
      }
    }
  }

  //=================================================================
  //== Process the Particle2LHCbID
  //=================================================================
  int nbPartIdContainer = 0;
  int nbPartId = 0;
  prevLink = -1;
  DaVinci::Map::Particle2LHCbIDs* partIds = NULL;
  LHCb::Particles* partContainer = NULL;
  int prevPartLink = -1;
  LHCb::PackedParticle2Ints* pPartIds =
    getIfExists<LHCb::PackedParticle2Ints>( m_inputStream +
                                            LHCb::PackedParticle2IntsLocation::InStream );
  if ( NULL != pPartIds )
  {
    for ( std::vector<LHCb::PackedParticle2Int>::iterator itL = pPartIds->relations().begin();
          pPartIds->relations().end() != itL; ++itL )
    {
      const LHCb::PackedParticle2Int& pPartId = *itL;
      const int linkID = pPartId.key >> 32;
      if ( linkID != prevLink )
      {
        prevLink = linkID;
        const std::string & containerName = pPartIds->linkMgr()->link( linkID )->path() + m_postFix;
        partIds = new DaVinci::Map::Particle2LHCbIDs();
        put( partIds, containerName );
        ++nbPartIdContainer;
      }
      ++nbPartId;
      int partLink(0), partKey(0);
      m_pack.indexAndKey64( pPartId.container, partLink, partKey );
      if ( partLink != prevPartLink )
      {
        prevPartLink = partLink;
        const std::string & partName = pPartIds->linkMgr()->link( partLink )->path();
        partContainer = get<LHCb::Particles>( partName );
      }
      LHCb::Particle* part = partContainer->object( partKey );
      std::vector<LHCb::LHCbID> temp;
      for ( int kk = pPartId.start; pPartId.end != kk; ++kk )
      {
        temp.push_back( LHCb::LHCbID( pPartIds->ints()[kk] ) );
      }
      partIds->insert( part, temp );
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "== Retrieved " << nbPart << " Particles in " << nbContainer << " containers, "
            << nbVert << " vertices in " << nbVertContainer << " containers, "
            << nbRecVert << " RecVerticess in " << nbRecVertContainer << " containers,"
            << nbRel << " relations in " << nbRelContainer << " containers, "
            << nbPartId << " Part-LHCbID in " << nbPartIdContainer << " containers, "
            <<endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackParticlesAndVertices )

//=============================================================================
