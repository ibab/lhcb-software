// Include files

// local
#include "PackParticlesAndVertices.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackParticlesAndVertices
//
// 2012-01-23 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackParticlesAndVertices::PackParticlesAndVertices( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputStream",        m_inputStream   = "/Event" );
  declareProperty( "AlwaysCreateOutput", m_alwaysOutput  = false    );
  declareProperty( "ForceReading",       m_forceReading  = false    );
  declareProperty( "DeleteInput",        m_deleteInput   = false    );
  declareProperty( "ListRemaining",      m_listRemaining = false    );
  declareProperty( "EnableCheck",        m_enableCheck   = false    );
  declareProperty( "VetoedContainers",   m_vetoedConts  );

  m_clIdParticles   = 0x60000 + LHCb::CLID_Particle;
  m_clIdVertices    = 0x60000 + LHCb::CLID_Vertex;
  m_clIdRecVertices = 0x60000 + LHCb::CLID_RecVertex;
  m_clIdPart2Vert   = 0xEA9168DC; // Particle to Vertex relation
  m_clIdPart2MCPart = 0x7B880798; // Particle to MCParticle relations
  m_clIdPart2Int    = 0xF94852E4; // Particle to int relations

  //setProperty( "OutputLevel", 2 );
}

//=============================================================================
// Destructor
//=============================================================================
PackParticlesAndVertices::~PackParticlesAndVertices() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackParticlesAndVertices::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Only continue if this stream exists for this event
  if ( !exist<DataObject*>(m_inputStream) ) return StatusCode::SUCCESS;
  DataObject* root = get<DataObject*>( m_inputStream );

  // List of data objects TES locations
  std::vector<std::string> names;

  if ( m_forceReading )
  {
    // fake class ID to get no container, but load.
    selectContainers( root, names, 0, true );
  }

  if ( m_listRemaining )
  {
    info() << "Initial objects are:" << endmsg;
    selectContainers( root, names, 0xFFFFFFFF );
  }

  // list of objects to remove at the end
  std::vector<DataObject*> toBeDeleted;

  names.clear();
  selectContainers( root, names, m_clIdParticles );
  if ( !names.empty() )
  {
    LHCb::PackedParticles* pparts = new LHCb::PackedParticles();
    put( pparts, m_inputStream + LHCb::PackedParticleLocation::InStream );
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "=== Process Particle containers :" << endmsg;
    for ( std::vector<std::string>::const_iterator itS = names.begin();
          names.end() != itS; ++itS )
    {
      LHCb::Particles* parts = get<LHCb::Particles>( *itS );
      if ( UNLIKELY(m_deleteInput) ) toBeDeleted.push_back( parts );
      if ( parts->empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) )
        debug() << format( "%4d particles in ", parts->size() ) << *itS << endmsg;
      packAParticleContainer( parts, *pparts );
    }
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "Stored " << pparts->data().size() << " packed tracks" << endmsg;
  }

  names.clear();
  selectContainers( root, names, m_clIdVertices );
  if ( !names.empty() )
  {
    LHCb::PackedVertices* pverts = new LHCb::PackedVertices();
    put( pverts, m_inputStream + LHCb::PackedVertexLocation::InStream );
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "=== Process Vertex containers :" << endmsg;
    for ( std::vector<std::string>::const_iterator itS = names.begin();
          names.end() != itS; ++itS )
    {
      LHCb::Vertices* verts = get<LHCb::Vertices>( *itS );
      if ( UNLIKELY(m_deleteInput) ) toBeDeleted.push_back( verts );
      if ( verts->empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) )
        debug () << format( "%4d vertices in ", verts->size() ) << *itS << endmsg;
      packAVertexContainer( verts, *pverts );
    }
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "Stored " << pverts->data().size() << " packed vertices" << endmsg;
  }

  names.clear();
  selectContainers( root, names, m_clIdRecVertices );
  if ( !names.empty() )
  {
    LHCb::PackedRecVertices* prverts = new LHCb::PackedRecVertices();
    put( prverts, m_inputStream + LHCb::PackedRecVertexLocation::InStream );
    prverts->setVersion( 2 ); // CRJ - Increment version for new RecVertex with weights
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "=== Process RecVertices containers :" << endmsg;
    for ( std::vector<std::string>::const_iterator itS = names.begin();
          names.end() != itS; ++itS )
    {
      LHCb::RecVertices* rverts = get<LHCb::RecVertices>( *itS );
      if ( UNLIKELY(m_deleteInput) ) toBeDeleted.push_back( rverts );
      if ( rverts->empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) )
        debug () << format( "%4d RecVertices in ", rverts->size() ) << *itS << endmsg;
      packARecVertexContainer( rverts, *prverts );
    }
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "Stored " << prverts->vertices().size() << " packed RecVertices" << endmsg;
  }

  names.clear();
  selectContainers( root, names, m_clIdPart2Vert );
  if ( !names.empty() )
  {
    LHCb::PackedRelations* prels = new LHCb::PackedRelations();
    put( prels, m_inputStream + LHCb::PackedRelationsLocation::InStream );
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "=== Process Particle2Vertex Relation containers :" << endmsg;
    for ( std::vector<std::string>::const_iterator itS = names.begin();
          names.end() != itS; ++itS )
    {
      P2VRELATION* rels = get<P2VRELATION>( *itS );
      if ( UNLIKELY(m_deleteInput) ) toBeDeleted.push_back( rels );
      if ( rels->relations().empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) )
        debug () << format( "%4d relations in ", rels->relations().size() ) << *itS << endmsg;
      packAP2PRelationContainer( rels, *prels );
    }
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "Stored " << prels->relations().size() 
              << " packed Particle2Vertex relations" << endmsg;
  }

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "Looking for Particle2Int relations " << m_clIdPart2Int << endmsg;
  names.clear();
  selectContainers( root, names, m_clIdPart2Int );
  if ( !names.empty() )
  {
    LHCb::PackedRelations * pPartIds = new LHCb::PackedRelations();
    put( pPartIds, m_inputStream + LHCb::PackedRelationsLocation::P2Int );
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "=== Process Particle2Int Relation containers :" << endmsg;
    for ( std::vector<std::string>::const_iterator itS = names.begin();
          names.end() != itS; ++itS )
    {
      Part2IntRelations * partIds = get<Part2IntRelations>( *itS );
      if ( UNLIKELY(m_deleteInput) ) toBeDeleted.push_back( partIds );
      if ( partIds->relations().empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) )
        debug() << format( "%4d Particle2Ints in ", partIds->relations().size() ) 
                << *itS << endmsg;
      packAP2IntRelationContainer( partIds, *pPartIds );
    }
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "Stored " << pPartIds->relations().size() << " packed Particle2Ints" << endmsg;
  }

  // MC Information next

  names.clear();
  selectContainers( root, names, m_clIdPart2MCPart );
  if ( !names.empty() )
  {
    LHCb::PackedRelations* prels = new LHCb::PackedRelations();
    put( prels, m_inputStream + LHCb::PackedRelationsLocation::P2MCP );
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "=== Process Particle2MCParticle Relation containers :" << endmsg;
    for ( std::vector<std::string>::const_iterator itS = names.begin();
          names.end() != itS; ++itS )
    {
      P2MCPRELATION * rels = get<P2MCPRELATION>( *itS );
      if ( UNLIKELY(m_deleteInput) ) toBeDeleted.push_back( rels );
      if ( rels->relations().empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) )
        debug () << format( "%4d relations in ", rels->relations().size() ) << *itS << endmsg;
      packAP2PRelationContainer( rels, *prels );
    }
    if ( msgLevel( MSG::DEBUG ) )
      debug() << "Stored " << prels->relations().size() 
              << " packed Particle2MCParticle relations" << endmsg;
  }

  //== Remove the converted containers if requested
  if ( UNLIKELY(m_deleteInput) )
  {
    for ( std::vector<DataObject*>::iterator itO = toBeDeleted.begin();
          toBeDeleted.end() != itO; ++itO )
    {
      const StatusCode sc = evtSvc()->unregisterObject( *itO );
      if ( sc.isSuccess() )
      {
        delete *itO;
      }
      else
      {
        Error("Failed to delete input data as requested", sc ).ignore();
      }
    }
  }

  if ( m_listRemaining )
  {
    info() << "Deleted " << toBeDeleted.size() << " containers. Remaining objects are:" << endmsg;
    selectContainers( root, names, 0xFFFFFFFF );
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Select iteratively the containers ending with 'Prefix'
//=========================================================================
void PackParticlesAndVertices::selectContainers ( DataObject* obj,
                                                  std::vector<std::string>& names,
                                                  const unsigned int classID,
                                                  const bool forceRead )
{
  SmartIF<IDataManagerSvc> mgr( eventSvc() );
  typedef std::vector<IRegistry*> Leaves;
  Leaves leaves;
  StatusCode sc = mgr->objectLeaves( obj, leaves );
  if ( sc )
  {
    for ( Leaves::const_iterator iL = leaves.begin(); leaves.end() != iL; ++iL )
    {
      const std::string& id = (*iL)->identifier();
      DataObject* tmp(NULL);
      if ( forceRead )
      {
        sc = eventSvc()->retrieveObject( id, tmp );
      }
      else
      {
        sc = eventSvc()->findObject( id, tmp );
      }
      if ( sc && NULL != tmp )
      {
        if ( 0xFFFFFFFF == classID )
        {
          if ( tmp->clID() != CLID_DataObject )
          {
            info() << format( "Class %8.8x (%5d) name ", tmp->clID(), tmp->clID()&0xFFFF )
                   << id << endmsg;
          }
        }
        if ( msgLevel(MSG::DEBUG) ) 
          debug() << "Found container '" << id << "' ClassID=" << tmp->clID() << endmsg;
        if ( tmp->clID() == classID )
        {
          if ( msgLevel(MSG::DEBUG) ) 
            debug() << " -> Matches target class ID " << classID << endmsg;

          // Is this location veto'ed ?
          std::vector<std::string>::const_iterator iV = std::find( m_vetoedConts.begin(),
                                                                   m_vetoedConts.end(),
                                                                   id );
          if ( iV == m_vetoedConts.end() )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << "  --> Selected ... " << id << endmsg;
            names.push_back( id );
          }
          else
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << "  --> VETO'ed ... " << id << endmsg;
          }
        }
        selectContainers( tmp, names, classID, forceRead );
      }
    }
  }
}

//=========================================================================
//  Pack a container of particles in the PackedParticles object
//=========================================================================
void
PackParticlesAndVertices::packAParticleContainer ( const LHCb::Particles* parts,
                                                   LHCb::PackedParticles& pparts )
{
  const LHCb::ParticlePacker pPacker(*this);

  // checks
  LHCb::Particles* unpacked = ( m_enableCheck ? new LHCb::Particles() : NULL );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsParticleTest" ); }

  for ( LHCb::Particles::const_iterator iD = parts->begin();
        iD != parts->end(); ++iD )
  {
    const LHCb::Particle& part = **iD;

    // Make a new packed data object and save
    pparts.data().push_back( LHCb::PackedParticle() );
    LHCb::PackedParticle& ppart = pparts.data().back();

    // reference to original container and key
    ppart.key = m_pack.reference64( &pparts, part.parent(), part.key() );

    // pack the physics info
    pPacker.pack( part, ppart, pparts );

    // checks ?
    if ( unpacked )
    {
      int key(0),linkID(0);
      m_pack.indexAndKey64( ppart.key, linkID, key );
      LHCb::Particle* testObj = new LHCb::Particle();
      unpacked->insert( testObj, key );
      pPacker.unpack( ppart, *testObj, pparts, *unpacked );
      pPacker.check( part, *testObj ).ignore();
    }

  }

  // clean up test data
  if ( unpacked )
  {
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() )
    {
      delete unpacked;
    }
    else
    {
      Exception( "Failed to delete test data after unpacking check" );
    }
  }

  if ( !m_deleteInput ) parts->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a container of vertices in the PackedVertices object
//=========================================================================
void PackParticlesAndVertices::packAVertexContainer ( const LHCb::Vertices* verts,
                                                      LHCb::PackedVertices& pverts )
{
  const LHCb::VertexPacker vPacker(*this);

  // checks
  LHCb::Vertices* unpacked = ( m_enableCheck ? new LHCb::Vertices() : NULL );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsVertexTest" ); }

  for ( LHCb::Vertices::const_iterator iD = verts->begin();
        iD != verts->end(); ++iD )
  {
    const LHCb::Vertex& vert = **iD;

    // Make a new packed data object and save
    pverts.data().push_back( LHCb::PackedVertex() );
    LHCb::PackedVertex& pvert = pverts.data().back();

    // reference to original container and key
    pvert.key = m_pack.reference64( &pverts, vert.parent(), vert.key() );

    // fill remaining physics info
    vPacker.pack( vert, pvert, pverts );

    // checks ?
    if ( unpacked )
    {
      int key(0),linkID(0);
      m_pack.indexAndKey64( pvert.key, linkID, key );
      LHCb::Vertex* testObj = new LHCb::Vertex();
      unpacked->insert( testObj, key );
      vPacker.unpack( pvert, *testObj, pverts, *unpacked );
      vPacker.check( vert, *testObj ).ignore();
    }
  }

  // clean up test data
  if ( unpacked )
  {
    StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if( sc.isSuccess() )
    {
      delete unpacked;
    }
    else
    {
      Error("Failed to delete test data after unpacking check", sc ).ignore();
    }
  }

  if ( !m_deleteInput ) verts->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a container of RecVertex
//=========================================================================
void PackParticlesAndVertices::packARecVertexContainer( const LHCb::RecVertices* rVerts,
                                                        LHCb::PackedRecVertices& pRVerts )
{
  const LHCb::RecVertexPacker rvPacker(*dynamic_cast<GaudiAlgorithm*>(this));

  for ( LHCb::RecVertices::const_iterator iD = rVerts->begin();
        iD != rVerts->end(); ++iD )
  {
    const LHCb::RecVertex& rVert = **iD;

    // Make a new packed data object and save
    pRVerts.vertices().push_back( LHCb::PackedRecVertex() );
    LHCb::PackedRecVertex& pRVert = pRVerts.vertices().back();

    // reference to original container and key
    pRVert.key       = rVert.key();
    pRVert.container = (int)m_pack.linkID( &pRVerts, rVert.parent() );

    // Physics info
    rvPacker.pack( rVert, pRVert, *rVerts, pRVerts );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  if ( !m_deleteInput ) rVerts->registry()->setAddress( 0 );
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackParticlesAndVertices )

//=============================================================================
