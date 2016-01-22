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
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_pack ( this )
{
  declareProperty( "InputStream",        m_inputStream   = "/Event" );
  declareProperty( "AlwaysCreateOutput", m_alwaysOutput  = false    );
  declareProperty( "DeleteInput",        m_deleteInput   = false    );
  declareProperty( "EnableCheck",        m_enableCheck   = false    );
  declareProperty( "VetoedContainers",   m_vetoedConts  );
  //setProperty( "OutputLevel", 1 );
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
  const auto * root = getIfExists<DataObject*>( m_inputStream );
  if ( !root ) return StatusCode::SUCCESS;

  // Class IDs for handled data
  const CLID clIdParticles      = 0x60000 + LHCb::CLID_Particle;
  const CLID clIdVertices       = 0x60000 + LHCb::CLID_Vertex;
  const CLID clIdRecVertices    = 0x60000 + LHCb::CLID_RecVertex;
  const CLID clIdFlavourTags    = 0x60000 + LHCb::CLID_FlavourTag;
  const CLID clIdTracks         = 0x60000 + LHCb::CLID_Track;
  const CLID clIdProtoParticles = 0x60000 + LHCb::CLID_ProtoParticle;
  const CLID clIdMuonPIDs       = 0x60000 + LHCb::CLID_MuonPID;
  const CLID clIdRichPIDs       = 0x60000 + LHCb::CLID_RichPID;
  const CLID clIdPart2Vert      = 0xEA9168DC; // Particle to Vertex relation
  const CLID clIdPart2MCPart    = 0x7B880798; // Particle to MCParticle relations
  const CLID clIdProto2MCPart   = 0x6540787E; // ProtoParticle to MCParticle weighted relations
  const CLID clIdPart2Int       = 0xF94852E4; // Particle to int relations
  const CLID clIdPart2RelInfo   = 0x90F0684D; // particle to related info map

  // list of objects to remove at the end
  std::vector<DataObject*> toBeDeleted;

  //==============================================================================
  // Traverse the TES to build the map of ClassIDs to TES locations
  //==============================================================================
  ClassIDTESMap tesMap;
  buildTESMap( root, tesMap );

  //==============================================================================
  // Find Particles
  //==============================================================================
  {
    const auto & names = tesMap[clIdParticles];
    if ( !names.empty() )
    {
      LHCb::PackedParticles* pparts = new LHCb::PackedParticles();
      put( pparts, m_inputStream + LHCb::PackedParticleLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process Particle containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        LHCb::Particles* parts = get<LHCb::Particles>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( parts );
        if ( parts->empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug() << format( "%4d particles in ", parts->size() ) << name << endmsg;
        packAParticleContainer( parts, *pparts );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << pparts->data().size() << " packed particles" << endmsg;
    }
  }

  //==============================================================================
  // Find Vertices
  //==============================================================================
  {
    const auto & names = tesMap[clIdVertices];
    if ( !names.empty() )
    {
      LHCb::PackedVertices* pverts = new LHCb::PackedVertices();
      put( pverts, m_inputStream + LHCb::PackedVertexLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process Vertex containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        LHCb::Vertices* verts = get<LHCb::Vertices>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( verts );
        if ( verts->empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug () << format( "%4d vertices in ", verts->size() ) << name << endmsg;
        packAVertexContainer( verts, *pverts );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << pverts->data().size() << " packed vertices" << endmsg;
    }
  }

  //==============================================================================
  // Find Flavour Tags
  //==============================================================================
  {
    const auto & names = tesMap[clIdFlavourTags];
    if ( !names.empty() )
    {
      LHCb::PackedFlavourTags * pfts = new LHCb::PackedFlavourTags();
      put( pfts, m_inputStream + LHCb::PackedFlavourTagLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process FlavourTag containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        LHCb::FlavourTags * fts = get<LHCb::FlavourTags>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( fts );
        if ( fts->empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug() << format( "%4d FlavourTags in ", fts->size() ) << name << endmsg;
        packAFTContainer( fts, *pfts );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << pfts->data().size() << " packed FlavourTags"
                << endmsg;
    }
  }

  //==============================================================================
  // Find Rec Vertices
  //==============================================================================
  {
    const auto & names = tesMap[clIdRecVertices];
    if ( !names.empty() )
    {
      LHCb::PackedRecVertices* prverts = new LHCb::PackedRecVertices();
      prverts->setPackingVersion( LHCb::PackedRecVertices::defaultPackingVersion() );
      put( prverts, m_inputStream + LHCb::PackedRecVertexLocation::InStream );
      prverts->setVersion( 2 ); // CRJ - Increment version for new RecVertex with weights
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process RecVertices containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        LHCb::RecVertices* rverts = get<LHCb::RecVertices>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( rverts );
        if ( rverts->empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug () << format( "%4d RecVertices in ", rverts->size() ) 
                   << name << endmsg;
        packARecVertexContainer( rverts, *prverts );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << prverts->vertices().size() << " packed RecVertices" 
                << endmsg;
    }
  }

  //==============================================================================
  // Find Particle2 Vert Relations
  //==============================================================================
  {
    const auto & names = tesMap[clIdPart2Vert];
    if ( !names.empty() )
    {
      LHCb::PackedRelations* prels = new LHCb::PackedRelations();
      put( prels, m_inputStream + LHCb::PackedRelationsLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process Particle2Vertex Relation containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        P2VRELATION* rels = get<P2VRELATION>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( rels );
        if ( rels->relations().empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug () << format( "%4d relations in ", rels->relations().size() ) 
                   << name << endmsg;
        packAP2PRelationContainer( rels, *prels );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << prels->relations().size()
                << " packed Particle2Vertex relations" << endmsg;
    }
  }

  //==============================================================================
  // Find Particle to Ints
  //==============================================================================
  {
    const auto & names = tesMap[clIdPart2Int];
    if ( !names.empty() )
    {
      LHCb::PackedRelations * pPartIds = new LHCb::PackedRelations();
      put( pPartIds, m_inputStream + LHCb::PackedRelationsLocation::P2Int );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process Particle2Int Relation containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        Part2IntRelations * partIds = get<Part2IntRelations>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( partIds );
        if ( partIds->relations().empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug() << format( "%4d Particle2Ints in ", partIds->relations().size() )
                  << name << endmsg;
        packAP2IntRelationContainer( partIds, *pPartIds );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << pPartIds->relations().size() 
                << " packed Particle2Ints" << endmsg;
    }
  }

  //==============================================================================
  // Find Particle to Related Info
  //==============================================================================
  {
    const auto & names = tesMap[clIdPart2RelInfo];
    if ( !names.empty() )
    {
      LHCb::PackedRelatedInfoRelations * pPartIds = new LHCb::PackedRelatedInfoRelations();
      put( pPartIds, m_inputStream + LHCb::PackedRelatedInfoLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Found " << names.size() << " RelatedInfo containers : "
                << names << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      pPartIds->containers().reserve( names.size() );
      for ( const auto& name : names )
      {
        Part2InfoRelations * partIds = get<Part2InfoRelations>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( partIds );
        if ( msgLevel( MSG::DEBUG ) )
          debug() << " -> Processing " << name
                  << " with " << partIds->relations().size() << " relations" << endmsg;
        if ( partIds->relations().empty() ) continue;
        packAP2RelatedInfoRelationContainer( partIds, *pPartIds, name );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << pPartIds->relations().size()
                << " packed Particle2RelatedInfo in "
                << pPartIds->containers().size() << " containers."
                << " Total info pairs = " << pPartIds->info().size()
                << endmsg;
    }
  }

  //==============================================================================
  // Find ProtoParticles
  //==============================================================================
  {
    const auto & names = tesMap[clIdProtoParticles];
    if ( !names.empty() )
    {
      LHCb::PackedProtoParticles * pprotos = new LHCb::PackedProtoParticles();
      pprotos->setVersion( 2 ); // CRJ : Why set this ?
      pprotos->setPackingVersion( LHCb::PackedProtoParticles::defaultPackingVersion() );
      put( pprotos, m_inputStream + LHCb::PackedProtoParticleLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process ProtoParticle containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        LHCb::ProtoParticles* protos = get<LHCb::ProtoParticles>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( protos );
        if ( protos->empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug() << format( "%4d protoparticles in ", protos->size() ) << name << endmsg;
        packAProtoParticleContainer( protos, *pprotos );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << pprotos->protos().size() << " packed ProtoParticles" 
                << endmsg;
    }
  }

  //==============================================================================
  // Find MuonPIDs
  //==============================================================================
  {
    const auto & names = tesMap[clIdMuonPIDs];
    if ( !names.empty() )
    {
      LHCb::PackedMuonPIDs * ppids = new LHCb::PackedMuonPIDs();
      ppids->setPackingVersion( LHCb::PackedMuonPIDs::defaultPackingVersion() );
      put( ppids, m_inputStream + LHCb::PackedMuonPIDLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process MuonPID containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        LHCb::MuonPIDs * pids = get<LHCb::MuonPIDs>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( pids );
        if ( pids->empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug() << format( "%4d MuonPIDs in ", pids->size() ) << name << endmsg;
        packAMuonPIDContainer( pids, *ppids );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << ppids->data().size() << " packed MuonPIDs" << endmsg;
    }
  }

  //==============================================================================
  // Find RichPIDs
  //==============================================================================
  {
    const auto & names = tesMap[clIdRichPIDs];
    if ( !names.empty() )
    {
      LHCb::PackedRichPIDs * ppids = new LHCb::PackedRichPIDs();
      ppids->setPackingVersion( LHCb::PackedRichPIDs::defaultPackingVersion() );
      put( ppids, m_inputStream + LHCb::PackedRichPIDLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process RichPID containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        LHCb::RichPIDs * pids = get<LHCb::RichPIDs>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( pids );
        if ( pids->empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug() << format( "%4d RichPIDs in ", pids->size() ) << name << endmsg;
        packARichPIDContainer( pids, *ppids );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << ppids->data().size() << " packed RichPIDs" << endmsg;
    }
  }

  //==============================================================================
  // Find Tracks
  //==============================================================================
  {
    const auto & names = tesMap[clIdTracks];
    if ( !names.empty() )
    {
      LHCb::PackedTracks * ptracks = new LHCb::PackedTracks();
      ptracks->setVersion( 4 );
      put( ptracks, m_inputStream + LHCb::PackedTrackLocation::InStream );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process Track containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        LHCb::Tracks * tracks = get<LHCb::Tracks>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( tracks );
        if ( tracks->empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug() << format( "%4d tracks in ", tracks->size() ) << name << endmsg;
        packATrackContainer( tracks, *ptracks );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << ptracks->tracks().size() << " packed Tracks" 
                << endmsg;
    }
  }

  // MC Information next

  //==============================================================================
  // Find Particle -> MC relations
  //==============================================================================
  {
    const auto & names = tesMap[clIdPart2MCPart];
    if ( !names.empty() )
    {
      LHCb::PackedRelations* prels = new LHCb::PackedRelations();
      put( prels, m_inputStream + LHCb::PackedRelationsLocation::P2MCP );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process Particle2MCParticle Relation containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        P2MCPRELATION * rels = get<P2MCPRELATION>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( rels );
        if ( rels->relations().empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug () << format( "%4d relations in ", rels->relations().size() ) << name 
                   << endmsg;
        packAP2PRelationContainer( rels, *prels );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << prels->relations().size()
                << " packed Particle2MCParticle relations" << endmsg;
    }
  }

  //==============================================================================
  // Find Proto -> MC relations
  //==============================================================================
  {
    const auto & names = tesMap[clIdProto2MCPart];
    if ( !names.empty() )
    {
      LHCb::PackedWeightedRelations* prels = new LHCb::PackedWeightedRelations();
      put( prels, m_inputStream + LHCb::PackedWeightedRelationsLocation::PP2MCP );
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "=== Process ProtoParticle2MCParticle Relation containers :" << endmsg;
      toBeDeleted.reserve( names.size() + toBeDeleted.size() );
      for ( const auto& name : names )
      {
        Proto2MCPRelation * rels = get<Proto2MCPRelation>( name );
        if ( m_deleteInput ) toBeDeleted.push_back( rels );
        if ( rels->relations().empty() ) continue;
        if ( msgLevel( MSG::DEBUG ) )
          debug () << format( "%4d relations in ", rels->relations().size() ) << name 
                   << endmsg;
        packAP2PRelationContainer( rels, *prels );
      }
      if ( msgLevel( MSG::DEBUG ) )
        debug() << "Stored " << prels->relations().size()
                << " packed ProtoParticle2MCParticle relations" << endmsg;
    }
  }

  //==============================================================================
  // Remove the converted containers if requested
  //==============================================================================

  if ( m_deleteInput )
  {
    StatusCode sc = StatusCode::SUCCESS;
    for ( auto *& it : toBeDeleted )
    {
      const StatusCode ssc = evtSvc()->unregisterObject( it );
      if ( ssc.isSuccess() ) { delete it; it = nullptr; }
      sc = sc && ssc;
    }
    if ( sc.isFailure() ) 
    { return Error( "Failed to delete input data as requested", sc ); }
  }

  //==============================================================================

  if ( msgLevel( MSG::DEBUG ) ) { debug() << "Finished..." << endmsg; }
  return StatusCode::SUCCESS;
}

//=========================================================================
// Build the ClassID to TES location(s) map
//=========================================================================
void PackParticlesAndVertices::buildTESMap( const DataObject* obj,
                                            ClassIDTESMap& tesmap ) const
{
  // Find any data leaves for this location
  SmartIF<IDataManagerSvc> mgr( eventSvc() );
  Leaves leaves;
  StatusCode sc = mgr->objectLeaves( obj, leaves );
  if ( sc.isSuccess() )
  {
    // loop over the found leaves
    for ( const auto& leaf : leaves )
    {
      // TES location identifier
      const std::string & id = leaf->identifier();
      // load the object
      DataObject* tmp(nullptr);
      sc = eventSvc()->findObject( id, tmp );
      // if found process it
      if ( sc.isSuccess() && nullptr != tmp )
      {
        // If not a data node, save in the map
        if ( tmp->clID() != CLID_DataObject )
        {
          if ( msgLevel(MSG::DEBUG) )
            debug() << "Found '" << id << "' ClassID=" << tmp->clID()
                    << " Type='" << System::typeinfoName( typeid(*tmp) )
                    << endmsg;
          // Save in the map if not veto'ed
          if ( !isVetoed(id) ) { tesmap[tmp->clID()].push_back( id ); }
        }
        // recurse down the tree
        buildTESMap( tmp, tesmap );
      }
    }
  }
}

//=========================================================================
// Pack a container of FlavourTags in the PackedParticles object
//=========================================================================
void
PackParticlesAndVertices::
packAFTContainer ( const LHCb::FlavourTags* fts,
                   LHCb::PackedFlavourTags& pfts ) const
{
  const LHCb::FlavourTagPacker ftPacker(this);

  // checks
  auto * unpacked = ( m_enableCheck ? new LHCb::FlavourTags() : nullptr );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsFTTest" ); }

  // reserve size
  pfts.data().reserve( pfts.data().size() + fts->size() );

  // loop over FTs
  for ( const auto * ft : *fts )
  {
    // Make a new packed data object and save
    pfts.data().emplace_back( LHCb::PackedFlavourTag() );
    LHCb::PackedFlavourTag& pft = pfts.data().back();

    // reference to original container and key
    pft.key = m_pack.reference64( &pfts, ft->parent(), ft->key() );

    // pack the physics info
    ftPacker.pack( *ft, pft, pfts );

    // checks ?
    if ( unpacked )
    {
      int key(0), linkID(0);
      m_pack.indexAndKey64( pft.key, linkID, key );
      LHCb::FlavourTag * testObj = new LHCb::FlavourTag();
      unpacked->insert( testObj, key );
      ftPacker.unpack( pft, *testObj, pfts, *unpacked );
      ftPacker.check( *ft, *testObj ).ignore();
    }

  }

  // clean up test data
  if ( unpacked )
  {
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) { delete unpacked; }
    else { Exception( "Failed to delete test data after unpacking check" ); }
  }

  if ( !m_deleteInput ) fts->registry()->setAddress( 0 );
}

//=========================================================================
// Pack a container of particles in the PackedParticles object
//=========================================================================
void
PackParticlesAndVertices::
packAParticleContainer ( const LHCb::Particles* parts,
                         LHCb::PackedParticles& pparts ) const
{
  const LHCb::ParticlePacker pPacker(this);

  // checks
  auto * unpacked = ( m_enableCheck ? new LHCb::Particles() : nullptr );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsParticleTest" ); }

  // reserve size
  pparts.data().reserve( pparts.data().size() + parts->size() );

  // loop
  for ( const auto * part : *parts )
  {
    // Make a new packed data object and save
    pparts.data().emplace_back( LHCb::PackedParticle() );
    LHCb::PackedParticle& ppart = pparts.data().back();

    // reference to original container and key
    ppart.key = m_pack.reference64( &pparts, part->parent(), part->key() );

    // pack the physics info
    pPacker.pack( *part, ppart, pparts );

    // checks ?
    if ( unpacked )
    {
      int key(0), linkID(0);
      m_pack.indexAndKey64( ppart.key, linkID, key );
      LHCb::Particle* testObj = new LHCb::Particle();
      unpacked->insert( testObj, key );
      pPacker.unpack( ppart, *testObj, pparts, *unpacked );
      pPacker.check( *part, *testObj ).ignore();
    }

  }

  // clean up test data
  if ( unpacked )
  {
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) { delete unpacked; }
    else { Exception( "Failed to delete test data after unpacking check" ); }
  }

  if ( !m_deleteInput ) parts->registry()->setAddress( 0 );
}

//=========================================================================
// Pack a container of MuonPIDs
//=========================================================================
void
PackParticlesAndVertices::
packAMuonPIDContainer ( const LHCb::MuonPIDs* pids,
                        LHCb::PackedMuonPIDs& ppids ) const
{
  const LHCb::MuonPIDPacker pPacker(this);

  // checks
  auto * unpacked = ( m_enableCheck ? new LHCb::MuonPIDs() : nullptr );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsMuonPIDTest" ); }

  // reserve size
  ppids.data().reserve( ppids.data().size() + pids->size() );

  // loop
  for ( const auto * pid : *pids )
  {
    // Make a new packed data object and save
    ppids.data().emplace_back( LHCb::PackedMuonPID() );
    LHCb::PackedMuonPID& ppid = ppids.data().back();

    // reference to original container and key
    ppid.key = m_pack.reference64( &ppids, pid->parent(), pid->key() );

    // pack the physics info
    pPacker.pack( *pid, ppid, ppids );

    // checks ?
    if ( unpacked )
    {
      int key(0), linkID(0);
      m_pack.indexAndKey32( ppid.key, linkID, key );
      LHCb::MuonPID* testObj = new LHCb::MuonPID();
      unpacked->insert( testObj, key );
      pPacker.unpack( ppid, *testObj, ppids, *unpacked );
      pPacker.check( *pid, *testObj ).ignore();
    }

  }

  // clean up test data
  if ( unpacked )
  {
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) { delete unpacked; }
    else { Exception( "Failed to delete test data after unpacking check" ); }
  }

  if ( !m_deleteInput ) pids->registry()->setAddress( 0 );

}

//=========================================================================
// Pack a container of RichPIDs
//=========================================================================
void
PackParticlesAndVertices::
packARichPIDContainer ( const LHCb::RichPIDs* pids,
                        LHCb::PackedRichPIDs& ppids ) const
{
  const LHCb::RichPIDPacker pPacker(this);

  // checks
  auto * unpacked = ( m_enableCheck ? new LHCb::RichPIDs() : nullptr );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsRichPIDTest" ); }

  // reserve size
  ppids.data().reserve( ppids.data().size() + pids->size() );

  // loop
  for ( const auto * pid : *pids )
  {
    // Make a new packed data object and save
    ppids.data().emplace_back( LHCb::PackedRichPID() );
    LHCb::PackedRichPID& ppid = ppids.data().back();

    // reference to original container and key
    ppid.key = m_pack.reference64( &ppids, pid->parent(), pid->key() );

    // pack the physics info
    pPacker.pack( *pid, ppid, ppids );

    // checks ?
    if ( unpacked )
    {
      int key(0), linkID(0);
      m_pack.indexAndKey32( ppid.key, linkID, key );
      LHCb::RichPID* testObj = new LHCb::RichPID();
      unpacked->insert( testObj, key );
      pPacker.unpack( ppid, *testObj, ppids, *unpacked );
      pPacker.check( *pid, *testObj ).ignore();
    }

  }

  // clean up test data
  if ( unpacked )
  {
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) { delete unpacked; }
    else { Exception( "Failed to delete test data after unpacking check" ); }
  }

  if ( !m_deleteInput ) pids->registry()->setAddress( 0 );

}

//=========================================================================
// Pack a container of protoparticles
//=========================================================================
void
PackParticlesAndVertices::
packAProtoParticleContainer( const LHCb::ProtoParticles* protos,
                             LHCb::PackedProtoParticles& pprotos ) const
{
  const LHCb::ProtoParticlePacker pPacker(this);

  // checks
  auto * unpacked = ( m_enableCheck ? new LHCb::ProtoParticles() : nullptr );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsProtoParticleTest" ); }

  // reserve size
  pprotos.protos().reserve( pprotos.protos().size() + protos->size() );

  // loop
  for ( const auto * proto : *protos )
  {
    // Make a new packed data object and save
    pprotos.protos().emplace_back( LHCb::PackedProtoParticle() );
    LHCb::PackedProtoParticle& pproto = pprotos.protos().back();

    // reference to original container and key
    pproto.key = m_pack.reference64( &pprotos, proto->parent(), proto->key() );

    // pack the physics info
    pPacker.pack( *proto, pproto, pprotos );

    // checks ?
    if ( unpacked )
    {
      int key(0), linkID(0);
      m_pack.indexAndKey32( pproto.key, linkID, key );
      LHCb::ProtoParticle* testObj = new LHCb::ProtoParticle();
      unpacked->insert( testObj, key );
      pPacker.unpack( pproto, *testObj, pprotos, *unpacked );
      pPacker.check( *proto, *testObj ).ignore();
    }

  }

  // clean up test data
  if ( unpacked )
  {
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) { delete unpacked; }
    else { Exception( "Failed to delete test data after unpacking check" ); }
  }

  if ( !m_deleteInput ) protos->registry()->setAddress( 0 );
}

//=========================================================================
// Pack a container of tracks
//=========================================================================
void
PackParticlesAndVertices::
packATrackContainer( const LHCb::Tracks* tracks,
                     LHCb::PackedTracks& ptracks ) const
{
  const LHCb::TrackPacker tPacker(this);

  // checks
  auto * unpacked = ( m_enableCheck ? new LHCb::Tracks() : nullptr );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsTrackTest" ); }

  // reserve size
  ptracks.tracks().reserve( ptracks.tracks().size() + tracks->size() );

  // loop
  for ( const auto * track : *tracks )
  {
    // Make a new packed data object and save
    ptracks.tracks().emplace_back( LHCb::PackedTrack() );
    LHCb::PackedTrack& ptrack = ptracks.tracks().back();

    // reference to original container and key
    ptrack.key = m_pack.reference64( &ptracks, track->parent(), track->key() );

    // pack the physics info
    tPacker.pack( *track, ptrack, ptracks );

    // checks ?
    if ( unpacked )
    {
      int key(0), linkID(0);
      m_pack.indexAndKey32( ptrack.key, linkID, key );
      LHCb::Track* testObj = new LHCb::Track();
      unpacked->insert( testObj, key );
      tPacker.unpack( ptrack, *testObj, ptracks, *unpacked );
      tPacker.check( *track, *testObj ).ignore();
    }

  }

  // clean up test data
  if ( unpacked )
  {
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) { delete unpacked; }
    else { Exception( "Failed to delete test data after unpacking check" ); }
  }

  if ( !m_deleteInput ) tracks->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a container of vertices in the PackedVertices object
//=========================================================================
void 
PackParticlesAndVertices::
packAVertexContainer ( const LHCb::Vertices* verts,
                       LHCb::PackedVertices& pverts ) const
{
  const LHCb::VertexPacker vPacker(this);

  // checks
  auto * unpacked = ( m_enableCheck ? new LHCb::Vertices() : nullptr );
  if ( unpacked ) { put( unpacked, "/Event/Transient/PsAndVsVertexTest" ); }

  // reserve size
  pverts.data().reserve( pverts.data().size() + verts->size() );

  // loop
  for ( const auto * vert : *verts )
  {
    // Make a new packed data object and save
    pverts.data().emplace_back( LHCb::PackedVertex() );
    LHCb::PackedVertex& pvert = pverts.data().back();

    // reference to original container and key
    pvert.key = m_pack.reference64( &pverts, vert->parent(), vert->key() );

    // fill remaining physics info
    vPacker.pack( *vert, pvert, pverts );

    // checks ?
    if ( unpacked )
    {
      int key(0), linkID(0);
      m_pack.indexAndKey64( pvert.key, linkID, key );
      LHCb::Vertex* testObj = new LHCb::Vertex();
      unpacked->insert( testObj, key );
      vPacker.unpack( pvert, *testObj, pverts, *unpacked );
      vPacker.check( *vert, *testObj ).ignore();
    }
  }

  // clean up test data
  if ( unpacked )
  {
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) { delete unpacked; }
    else { Exception("Failed to delete test data after unpacking check" ); }
  }

  if ( !m_deleteInput ) verts->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a container of RecVertex
//=========================================================================
void 
PackParticlesAndVertices::
packARecVertexContainer( const LHCb::RecVertices* rVerts,
                         LHCb::PackedRecVertices& pRVerts ) const
{
  const LHCb::RecVertexPacker rvPacker(this);

  // reserve size
  pRVerts.vertices().reserve( pRVerts.vertices().size() + rVerts->size() );

  // loop
  for ( const auto * rVert : *rVerts )
  {
    // Make a new packed data object and save
    pRVerts.vertices().emplace_back( LHCb::PackedRecVertex() );
    LHCb::PackedRecVertex& pRVert = pRVerts.vertices().back();

    // reference to original container and key
    pRVert.key       = rVert->key();
    pRVert.container = (int)m_pack.linkID( &pRVerts, rVert->parent() );

    // Physics info
    rvPacker.pack( *rVert, pRVert, *rVerts, pRVerts );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  if ( !m_deleteInput ) { rVerts->registry()->setAddress( 0 ); }
}

//=========================================================================
//  Pack a container of Related info
//=========================================================================

void
PackParticlesAndVertices::
packAP2RelatedInfoRelationContainer( const PackParticlesAndVertices::Part2InfoRelations * rels,
                                     LHCb::PackedRelatedInfoRelations& prels,
                                     const std::string & location ) const
{
  // Make a entry in the containers vector, for this TES location
  prels.containers().emplace_back( LHCb::PackedRelatedInfoMap() );
  LHCb::PackedRelatedInfoMap & pcont = prels.containers().back();

  // reference to original container and key
  pcont.reference = m_pack.reference64( &prels, rels, 0 );

  // First entry in the relations vector
  pcont.first = prels.relations().size();

  // Loop over the relations and fill
  prels.relations().reserve( prels.relations().size() + rels->relations().size() );

  // Use the packer to pack this location ...
  const LHCb::RelatedInfoRelationsPacker rPacker(this);
  rPacker.pack( *rels, prels );

  // last entry in the relations vector
  pcont.last = prels.relations().size();

  // checks
  if ( m_enableCheck )
  {
    // Make a temporary object
    auto * unpacked = new Part2InfoRelations();
    put( unpacked, "/Event/Transient/Part2RelatedInfoRelations" );

    // unpack
    rPacker.unpack( prels, *unpacked, location );

    // check
    rPacker.check( *rels, *unpacked );

    // remove temporary data
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) { delete unpacked; }
    else { Exception("Failed to delete test data after unpacking check" ); }
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  if ( !m_deleteInput ) { rels->registry()->setAddress( 0 ); }
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackParticlesAndVertices )

//=============================================================================
