// Include files
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
  : GaudiAlgorithm ( name, pSvcLocator ),
    m_pack( this )
{
  declareProperty( "InputStream", m_inputStream = "/Event/" );
  declareProperty( "PostFix",     m_postFix     = "" );
  //setProperty( "OutputLevel", 1 );
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
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "==> Execute"  << endmsg;

  //=================================================================
  //== Process the Tracks
  //=================================================================
  {
    int prevLink = -1;
    unsigned int nbPartContainer(0), nbPart(0);
    auto * ptracks =
      getIfExists<LHCb::PackedTracks>( m_inputStream + LHCb::PackedTrackLocation::InStream );
    if ( ptracks )
    {
      LHCb::Tracks * tracks = nullptr;
      for ( const LHCb::PackedTrack& ptrack : ptracks->tracks() )
      {
        int key(0),linkID(0);
        m_pack.indexAndKey64( ptrack.key, linkID, key );
        if ( linkID != prevLink )
        {
          prevLink = linkID;
          const std::string & containerName = ptracks->linkMgr()->link(linkID)->path() + m_postFix;
          // Check to see if container already exists. If it does, unpacking has already been run this
          // event so quit
          if ( exist<LHCb::Tracks>(containerName) )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " -> " << containerName << " exists" << endmsg;
            return StatusCode::SUCCESS;
          }
          tracks = new LHCb::Tracks();
          put( tracks, containerName );
          ++nbPartContainer;
        }

        // Make new object and insert into the output container
        LHCb::Track * track = new LHCb::Track();
        tracks->insert( track, key );
        ++nbPart;

        // Unpack the physics info
        m_trackPacker.unpack( ptrack, *track, *ptracks, *tracks );

      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbPart << " Tracks in " << nbPartContainer << " containers" << endmsg;
    }
    counter("# Unpacked Tracks") += nbPart;
  }

  //=================================================================
  //== Process the MuonPIDs
  //=================================================================
  {
    int prevLink = -1;
    unsigned int nbPartContainer(0), nbPart(0);
    auto * ppids =
      getIfExists<LHCb::PackedMuonPIDs>( m_inputStream + LHCb::PackedMuonPIDLocation::InStream );
    if ( ppids )
    {
      LHCb::MuonPIDs * pids = nullptr;
      for ( const LHCb::PackedMuonPID& ppid : ppids->data() )
      {
        int key(0),linkID(0);
        m_pack.indexAndKey64( ppid.key, linkID, key );
        if ( linkID != prevLink )
        {
          prevLink = linkID;
          const std::string & containerName = ppids->linkMgr()->link(linkID)->path() + m_postFix;
          // Check to see if container already exists. If it does, unpacking has already been run this
          // event so quit
          if ( exist<LHCb::MuonPIDs>(containerName) )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " -> " << containerName << " exists" << endmsg;
            return StatusCode::SUCCESS;
          }
          pids = new LHCb::MuonPIDs();
          put( pids, containerName );
          ++nbPartContainer;
        }

        // Make new object and insert into the output container
        LHCb::MuonPID * pid = new LHCb::MuonPID();
        pids->insert( pid, key );
        ++nbPart;

        // Unpack the physics info
        m_muonPacker.unpack( ppid, *pid, *ppids, *pids );

      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbPart << " MuonPIDs in " << nbPartContainer << " containers" << endmsg;
    }
    counter("# Unpacked MuonPIDs") += nbPart;
  }

  //=================================================================
  //== Process the RichPIDs
  //=================================================================
  {
    int prevLink = -1;
    unsigned int nbPartContainer(0), nbPart(0);
    auto * ppids =
      getIfExists<LHCb::PackedRichPIDs>( m_inputStream + LHCb::PackedRichPIDLocation::InStream );
    if ( ppids )
    {
      LHCb::RichPIDs * pids = nullptr;
      for ( const LHCb::PackedRichPID& ppid : ppids->data() )
      {
        int key(0),linkID(0);
        m_pack.indexAndKey64( ppid.key, linkID, key );
        if ( linkID != prevLink )
        {
          prevLink = linkID;
          const std::string & containerName = ppids->linkMgr()->link(linkID)->path() + m_postFix;
          // Check to see if container already exists. If it does, unpacking has already been run this
          // event so quit
          if ( exist<LHCb::RichPIDs>(containerName) )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " -> " << containerName << " exists" << endmsg;
            return StatusCode::SUCCESS;
          }
          pids = new LHCb::RichPIDs();
          put( pids, containerName );
          ++nbPartContainer;
        }

        // Make new object and insert into the output container
        LHCb::RichPID * pid = new LHCb::RichPID();
        pids->insert( pid, key );
        ++nbPart;

        // Unpack the physics info
        m_richPacker.unpack( ppid, *pid, *ppids, *pids );

      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbPart << " RichPIDs in " << nbPartContainer << " containers" << endmsg;
    }
    counter("# Unpacked RichPIDs") += nbPart;
  }

  //=================================================================
  //== Process the ProtoParticles
  //=================================================================
  {
    int prevLink = -1;
    unsigned int nbPartContainer(0), nbPart(0);
    auto * pprotos =
      getIfExists<LHCb::PackedProtoParticles>( m_inputStream + LHCb::PackedProtoParticleLocation::InStream );
    if ( pprotos )
    {
      LHCb::ProtoParticles * protos = nullptr;
      for ( const LHCb::PackedProtoParticle& pproto : pprotos->protos() )
      {
        int key(0),linkID(0);
        m_pack.indexAndKey64( pproto.key, linkID, key );
        if ( linkID != prevLink )
        {
          prevLink = linkID;
          const std::string & containerName = pprotos->linkMgr()->link( linkID )->path() + m_postFix;
          // Check to see if container already exists. If it does, unpacking has already been run this
          // event so quit
          if ( exist<LHCb::ProtoParticles>(containerName) )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " -> " << containerName << " exists" << endmsg;
            return StatusCode::SUCCESS;
          }
          protos = new LHCb::ProtoParticles();
          put( protos, containerName );
          ++nbPartContainer;
        }

        // Make new object and insert into the output container
        LHCb::ProtoParticle * proto = new LHCb::ProtoParticle();
        protos->insert( proto, key );
        ++nbPart;

        // Unpack the physics info
        m_protoPacker.unpack( pproto, *proto, *pprotos, *protos );

      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbPart << " ProtoParticles in " << nbPartContainer << " containers" 
              << endmsg;
    }
    counter("# Unpacked ProtoParticles") += nbPart;
  }

  //=================================================================
  //== Process the Particles
  //=================================================================
  {
    int prevLink = -1;
    unsigned int nbPartContainer(0), nbPart(0);
    auto* pparts =
      getIfExists<LHCb::PackedParticles>( m_inputStream + LHCb::PackedParticleLocation::InStream );
    if ( pparts )
    {
      LHCb::Particles* parts = nullptr;
      for ( const LHCb::PackedParticle& ppart : pparts->data() )
      {
        int key(0),linkID(0);
        m_pack.indexAndKey64( ppart.key, linkID, key );
        if ( linkID != prevLink )
        {
          prevLink = linkID;
          const std::string & containerName = pparts->linkMgr()->link(linkID)->path() + m_postFix;
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
          ++nbPartContainer;
        }

        // Make new object and insert into the output container
        LHCb::Particle* part = new LHCb::Particle();
        parts->insert( part, key );
        ++nbPart;

        // Unpack the physics info
        m_partPacker.unpack( ppart, *part, *pparts, *parts );

      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbPart << " Particles in " << nbPartContainer << " containers" << endmsg;
    }
    counter("# Unpacked Particles") += nbPart;
  }

  //=================================================================
  //== Process the vertices
  //=================================================================
  {
    unsigned int nbVertContainer(0), nbVert(0);
    int prevLink = -1;
    auto* pverts =
      getIfExists<LHCb::PackedVertices>( m_inputStream + LHCb::PackedVertexLocation::InStream );
    if ( pverts )
    {
      LHCb::Vertices* verts = nullptr;
      for ( const LHCb::PackedVertex& pvert : pverts->data() )
      {
        int key(0),linkID(0);
        m_pack.indexAndKey64( pvert.key, linkID, key );
        if ( linkID != prevLink )
        {
          prevLink = linkID;
          const std::string & containerName = pverts->linkMgr()->link( linkID )->path() + m_postFix;
          // Check to see if container already exists. If it does, unpacking has already been run this
          // event so quit
          if ( exist<LHCb::Vertices>(containerName) )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " -> " << containerName << " exists" << endmsg;
            return StatusCode::SUCCESS;
          }
          verts = new LHCb::Vertices();
          put( verts, containerName );
          ++nbVertContainer;
        }
        //== Construct with verticle ID and key.
        LHCb::Vertex* vert = new LHCb::Vertex();
        verts->insert( vert, key );
        ++nbVert;

        // unpack the physics info
        m_vertPacker.unpack( pvert, *vert, *pverts, *verts );
      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbVert << " vertices in " << nbVertContainer << " containers"
              << endmsg;
    }
    counter("# Unpacked Vertices") += nbVert;
  }

  //=================================================================
  //== Process the Flavour Tags
  //=================================================================
  {
    unsigned int nbFTContainer(0), nbFT(0);
    int prevLink = -1;
    auto* pfts =
      getIfExists<LHCb::PackedFlavourTags>( m_inputStream + LHCb::PackedFlavourTagLocation::InStream );
    if ( pfts )
    {
      LHCb::FlavourTags * fts = nullptr;
      for ( const LHCb::PackedFlavourTag& pft : pfts->data() )
      {
        int key(0),linkID(0);
        m_pack.indexAndKey64( pft.key, linkID, key );
        if ( linkID != prevLink )
        {
          prevLink = linkID;
          const std::string & containerName = pfts->linkMgr()->link(linkID)->path() + m_postFix;
          // Check to see if container alrady exists. If it does, unpacking has already been run this
          // event so quit
          if ( exist<LHCb::FlavourTags>(containerName) )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " -> " << containerName << " exists" << endmsg;
            return StatusCode::SUCCESS;
          }
          fts = new LHCb::FlavourTags();
          put( fts, containerName );
          ++nbFTContainer;
        }

        // Make new object and insert into the output container
        LHCb::FlavourTag * ft = new LHCb::FlavourTag();
        fts->insert( ft, key );
        ++nbFT;

        // Unpack the physics info
        m_ftPacker.unpack( pft, *ft, *pfts, *fts );

      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbFT << " FlavourTags in " << nbFTContainer << " containers" << endmsg;
    }
    counter("# Unpacked FlavourTags") += nbFT;
  }

  //=================================================================
  //== Process the RecVertices
  //=================================================================
  {
    unsigned int nbRecVertContainer(0), nbRecVert(0);
    int prevLink = -1;
    auto* pRecVerts =
      getIfExists<LHCb::PackedRecVertices>( m_inputStream + LHCb::PackedRecVertexLocation::InStream );
    if ( pRecVerts )
    {
      LHCb::RecVertices* recVerts = nullptr;
      for ( const LHCb::PackedRecVertex& pRecVert : pRecVerts->vertices() )
      {
        const int key    = pRecVert.key;
        const int linkID = pRecVert.container;
        if ( linkID != prevLink )
        {
          prevLink = linkID;
          const std::string & containerName = pRecVerts->linkMgr()->link(linkID)->path() + m_postFix;
          // Check to see if container already exists. If it does, unpacking has already been run this
          // event so quit
          if ( exist<LHCb::RecVertices>(containerName) )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " -> " << containerName << " exists" << endmsg;
            return StatusCode::SUCCESS;
          }
          recVerts = new LHCb::RecVertices();
          put( recVerts, containerName );
          ++nbRecVertContainer;
        }

        //== Construct with RecVertex ID and key.
        LHCb::RecVertex* recVert = new LHCb::RecVertex( key );
        recVerts->add( recVert );
        ++nbRecVert;

        // Physics Info
        m_rvPacker.unpack( pRecVert, *recVert, *pRecVerts, *recVerts );
      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbRecVert << " RecVertices in " << nbRecVertContainer << " containers"
              << endmsg;
    }
    counter("# Unpacked RecVertices") += nbRecVert;
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
  //== Process the PP2MCP relations
  //=================================================================
  unpackP2PWeightedRelations<LHCb::ProtoParticle,LHCb::MCParticle,LHCb::ProtoParticles,const LHCb::MCParticles,double>(m_inputStream+LHCb::PackedWeightedRelationsLocation::PP2MCP);

  //=================================================================
  //== Process the P2Int relations
  //=================================================================
  unpackP2IntRelations<LHCb::Particle,int,LHCb::Particles>(m_inputStream+LHCb::PackedRelationsLocation::P2Int);

  //=================================================================
  //== Process the Related info relations
  //=================================================================
  {
    typedef LHCb::Particle                FROM;
    typedef LHCb::RelatedInfoMap            TO;
    typedef LHCb::Relation1D<FROM,TO> RELATION;

    // Count data objects recreated
    unsigned int nbRelContainer(0), nbRel(0);

    // Location of the packed data
    const std::string location = m_inputStream + LHCb::PackedRelatedInfoLocation::InStream;

    // do we have any packed data
    LHCb::PackedRelatedInfoRelations * prels = getIfExists<LHCb::PackedRelatedInfoRelations>(location);
    if ( nullptr != prels )
    {
      // Loop over the different TES containers that where saved
      for ( const auto& cont : prels->containers() )
      {
        // Reconstruct container name for this entry
        const int indx = cont.reference >> 32;
        const std::string & containerName = prels->linkMgr()->link(indx)->path() + m_postFix;

        // Check to see if container already exists. If it does, unpacking has already been run this
        // event so quit
        if ( exist<RELATION>(containerName) )
        {
          if ( msgLevel(MSG::DEBUG) )
            debug() << " -> " << containerName << " exists" << endmsg;
          return StatusCode::SUCCESS;
        }
        
        // Create a new unpacked object at the TES location and save
        RELATION * rels = new RELATION();
        put( rels, containerName );

        // Loop over the relations saved at this container location
        for ( unsigned int kk = cont.first; cont.last > kk; ++kk )
        {
          // The relation information
          const auto& rel = prels->relations()[kk];

          // unpack this one entry
          m_rInfoPacker.unpack( rel, *prels, *rels );

          // Count
          ++nbRel;
        }

        // Count containers
        ++nbRelContainer;

      }

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Retrieved " << nbRel << " RelatedInfo relations in " << nbRelContainer << " containers"
              << " from " << location
              << endmsg;
    }

  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "... Execute finished." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackParticlesAndVertices )

//=============================================================================
