
// local
#include "ParticlesAndVerticesBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticlesAndVerticesBase
//
// 2012-01-23 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticlesAndVerticesBase::ParticlesAndVerticesBase( const std::string& name,
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
  m_clIdPart2Vert   = 0xEA9168DC;   // One of Vanya's relation, Particle to Vertex
  m_clIdPart2Ints   = 880; // Defined in the .cpp of this class!!!!
}

//=============================================================================
// Destructor
//=============================================================================
ParticlesAndVerticesBase::~ParticlesAndVerticesBase() {}

//=========================================================================
// Select iteratively the containers ending with 'Prefix'
//=========================================================================
void ParticlesAndVerticesBase::selectContainers ( DataObject* obj,
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
void
ParticlesAndVerticesBase::packAParticleContainer ( const LHCb::Particles* parts,
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
void ParticlesAndVerticesBase::packAVertexContainer ( const LHCb::Vertices* verts,
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
//  Pack a container of relations in the Pack object
//=========================================================================
void ParticlesAndVerticesBase::packARelationContainer ( const RELATION* rels,
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
void ParticlesAndVerticesBase::packARecVertexContainer( const LHCb::RecVertices* rVerts,
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

//=========================================================================
//  Pack a container of vertices in the PackedVertices object
//=========================================================================
void
ParticlesAndVerticesBase::
packAParticleLHCbIDContainer ( const DaVinci::Map::Particle2LHCbIDs* partIds,
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
