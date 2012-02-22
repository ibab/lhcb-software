// $Id: $
// Include files

// local
#include "PackParticlesAndVertices.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackParticlesAndVertices
//
// 2012-01-23 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackParticlesAndVertices )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PackParticlesAndVertices::PackParticlesAndVertices( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputStream",        m_inputStream   = "/Event"  );
  declareProperty( "AlwaysCreateOutput", m_alwaysOutput  = false     );
  declareProperty( "ForceReading",       m_forceReading  = false     );
  declareProperty( "DeleteInput",        m_deleteInput   = false     );
  declareProperty( "ListRemaining",      m_listRemaining = false     );
  declareProperty( "VetoedContainers",   m_vetoedConts  );
}

//=============================================================================
// Destructor
//=============================================================================
PackParticlesAndVertices::~PackParticlesAndVertices() {}

//=========================================================================
//  Initializer: Set the class ids for the various types to be packed
//=========================================================================
StatusCode PackParticlesAndVertices::initialize( ) 
{
  const StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;  

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_clIdParticles   = 0x60000 + LHCb::CLID_Particle;
  m_clIdVertices    = 0x60000 + LHCb::CLID_Vertex;
  m_clIdRecVertices = 0x60000 + LHCb::CLID_RecVertex;
  m_clIdPart2Vert   = 0xEA9168DC;   // One of Vanya's relation, Particle to Vertex
  m_clIdPart2Ints   = 880; // Defined in the .cpp of this class!!!!

  return sc;
}

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
      if ( m_deleteInput ) toBeDeleted.push_back( parts );
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
      if ( m_deleteInput ) toBeDeleted.push_back( verts );
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
      if ( m_deleteInput ) toBeDeleted.push_back( rverts );
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
      debug() << "=== Process Relation containers :" << endmsg;
    for ( std::vector<std::string>::const_iterator itS = names.begin();
          names.end() != itS; ++itS ) 
    {
      RELATION* rels = get<RELATION>( *itS );
      if ( m_deleteInput ) toBeDeleted.push_back( rels );
      if ( rels->relations().empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) ) 
        debug () << format( "%4d relations in ", rels->relations().size() ) << *itS << endmsg;
      packARelationContainer( rels, *prels );
    }
    if ( msgLevel( MSG::DEBUG ) ) 
      debug() << "Stored " << prels->relations().size() << " packed relation" << endmsg;
  }

  names.clear();
  selectContainers( root, names, m_clIdPart2Ints );
  if ( !names.empty() )
  {
    LHCb::PackedParticle2Ints* pPartIds = new LHCb::PackedParticle2Ints();
    put( pPartIds, m_inputStream + LHCb::PackedParticle2IntsLocation::InStream );
    if ( msgLevel( MSG::DEBUG ) ) 
      debug() << "=== Process Particle2LHCbIDs containers :" << endmsg;
    for ( std::vector<std::string>::const_iterator itS = names.begin(); 
          names.end() != itS; ++itS ) 
    {
      DaVinci::Map::Particle2LHCbIDs* partIds = get<DaVinci::Map::Particle2LHCbIDs>( *itS );
      if ( m_deleteInput ) toBeDeleted.push_back( partIds );
      if ( partIds->empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) ) 
        debug() << format( "%4d particles2LHCbIDs in ", partIds->size() ) << *itS << endmsg;
      packAParticleLHCbIDContainer( partIds, *pPartIds );
    }
    if ( msgLevel( MSG::DEBUG ) ) 
      debug() << "Stored " << pPartIds->relations().size() << " packed Particle2Ints" << endmsg;
  }

  //== Remove the converted containers if requested  
  if ( m_deleteInput ) 
  {
    for ( std::vector<DataObject*>::iterator itO = toBeDeleted.begin();
          toBeDeleted.end() != itO; ++itO ) 
    {
      evtSvc()->unregisterObject( *itO );
      delete *itO;
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
        if ( tmp->clID() == classID )
        {
          if ( msgLevel(MSG::DEBUG) ) 
            debug() << "Found container " << id << endmsg;
          
          std::vector<std::string>::const_iterator iV = std::find( m_vetoedConts.begin(),
                                                                   m_vetoedConts.end(),
                                                                   id );
          if ( iV == m_vetoedConts.end() )
          {
            if ( msgLevel(MSG::DEBUG) ) 
              debug() << " --> Selected ... " << id << endmsg;
            names.push_back( id );
          }
          else
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " --> VETO'ed ... " << id << endmsg;
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
void PackParticlesAndVertices::packAParticleContainer ( const LHCb::Particles* parts, 
                                                        LHCb::PackedParticles& pparts ) 
{
  static const LHCb::ParticlePacker pPacker;

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
  }

  if ( !m_deleteInput ) parts->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a container of vertices in the PackedVertices object
//=========================================================================
void PackParticlesAndVertices::packAVertexContainer ( const LHCb::Vertices* verts, 
                                                      LHCb::PackedVertices& pverts )
{
  static const LHCb::VertexPacker vPacker;

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
  }

  if ( !m_deleteInput ) verts->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a container of relations in the Pack object
//=========================================================================
void PackParticlesAndVertices::packARelationContainer ( const RELATION* rels, 
                                                        LHCb::PackedRelations& prels )
{
  // Make a new packed data object and save
  prels.relations().push_back( LHCb::PackedRelation() );
  LHCb::PackedRelation& prel = prels.relations().back();
  // reference to original container and key
  prel.container = m_pack.reference64( &prels, rels, 0 );
  prel.start     = prels.sources().size();
  RELATION::Range all = rels->relations();
  for ( RELATION::Range::iterator itR = all.begin(); all.end() != itR; ++itR )
  {
    prels.sources().push_back( m_pack.reference64( &prels, (*itR).from()->parent(), (*itR).from()->key() ) );
    prels.dests().push_back  ( m_pack.reference64( &prels, (*itR).to()->parent(),   (*itR).to()->key()   ) );
  }
  prel.end = prels.sources().size();
  if ( !m_deleteInput ) rels->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a container of RecVertex
//=========================================================================
void PackParticlesAndVertices::packARecVertexContainer( const LHCb::RecVertices* rVerts, 
                                                        LHCb::PackedRecVertices& pRVerts ) 
{
  static const LHCb::RecVertexPacker rvPacker;

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

//=========================================================================
//  Pack a container of vertices in the PackedVertices object
//=========================================================================
void 
PackParticlesAndVertices::packAParticleLHCbIDContainer ( const DaVinci::Map::Particle2LHCbIDs* partIds, 
                                                         LHCb::PackedParticle2Ints& pPartIds ) 
{
  
  for ( unsigned int kk = 0; partIds->size() > kk; ++kk )
  {
    // Make a new packed data object and save
    pPartIds.relations().push_back( LHCb::PackedParticle2Int() );
    LHCb::PackedParticle2Int & pPartId = pPartIds.relations().back();
    pPartId.key = m_pack.reference64( &pPartIds, partIds, kk );
    const LHCb::Particle* part = partIds->key_at(kk);
    const std::vector<LHCb::LHCbID> & ids = partIds->value_at(kk);
    // reference to original container and key
    pPartId.container = m_pack.reference64( &pPartIds, part->parent(), part->key() );
    pPartId.start     = pPartIds.ints().size();
    for ( std::vector<LHCb::LHCbID>::const_iterator itId = ids.begin(); 
          ids.end() != itId; ++itId ) 
    {
      pPartIds.ints().push_back( (*itId).lhcbID() );
    }
    pPartId.end       = pPartIds.ints().size();
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  if ( !m_deleteInput ) partIds->registry()->setAddress( 0 );
}

//=============================================================================
