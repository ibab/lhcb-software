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
StatusCode PackParticlesAndVertices::initialize( ) {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

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
StatusCode PackParticlesAndVertices::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  std::vector<std::string>::const_iterator itS;

  DataObject* root = get<DataObject*>( m_inputStream );
  std::vector<std::string> names;
  std::vector<DataObject*> toBeDeleted;
  if ( m_forceReading ) selectContainers( root, names, 0, true );  // fake class ID to get no container, but load.

  if ( m_listRemaining ) 
  {
    info() << "Initial objects are:" << endmsg;
    selectContainers( root, names, 0xFFFFFFFF );
  }

  names.clear();
  selectContainers( root, names, m_clIdParticles );
  if ( !names.empty() )
  {
    LHCb::PackedParticles* pparts = new LHCb::PackedParticles();
    put( pparts, m_inputStream + LHCb::PackedParticleLocation::InStream );  
    if ( msgLevel( MSG::DEBUG ) ) debug() << "=== Process Particle containers :" << endmsg;
    for ( itS = names.begin(); names.end() != itS; ++itS ) 
    {
      LHCb::Particles* parts = get<LHCb::Particles>( *itS );
      if ( m_deleteInput ) toBeDeleted.push_back( parts );
      if ( parts->empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) ) debug() << format( "%4d particles in ", parts->size() ) << *itS << endmsg;
      packAParticleContainer( parts, *pparts );
    }
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Stored " << pparts->data().size() << " packed tracks" << endmsg;
  }

  names.clear();
  selectContainers( root, names, m_clIdVertices );
  if ( !names.empty() )
  {
    LHCb::PackedVertices* pverts = new LHCb::PackedVertices();
    put( pverts, m_inputStream + LHCb::PackedVertexLocation::InStream );
    if ( msgLevel( MSG::DEBUG ) ) debug() << "=== Process Vertex containers :" << endmsg;
    for ( itS = names.begin(); names.end() != itS; ++itS ) 
    {
      LHCb::Vertices* verts = get<LHCb::Vertices>( *itS );
      if ( m_deleteInput ) toBeDeleted.push_back( verts );
      if ( verts->empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) ) debug () << format( "%4d vertices in ", verts->size() ) << *itS << endmsg;
      packAVertexContainer( verts, *pverts );
    }
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Stored " << pverts->data().size() << " packed vertices" << endmsg;
  }

  names.clear();
  selectContainers( root, names, m_clIdRecVertices );
  if ( !names.empty() )
  {
    LHCb::PackedRecVertices* prverts = new LHCb::PackedRecVertices();
    put( prverts, m_inputStream + LHCb::PackedRecVertexLocation::InStream );
    if ( msgLevel( MSG::DEBUG ) ) debug() << "=== Process RecVertices containers :" << endmsg;
    for ( itS = names.begin(); names.end() != itS; ++itS ) 
    {
      LHCb::RecVertices* rverts = get<LHCb::RecVertices>( *itS );
      if ( m_deleteInput ) toBeDeleted.push_back( rverts );
      if ( rverts->empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) ) debug () << format( "%4d RecVertices in ", rverts->size() ) << *itS << endmsg;
      packARecVertexContainer( rverts, *prverts );
    }
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Stored " << prverts->vertices().size() << " packed RecVertices" << endmsg;
  }

  names.clear();
  selectContainers( root, names, m_clIdPart2Vert );
  if ( !names.empty() )
  {
    LHCb::PackedRelations* prels = new LHCb::PackedRelations();
    put( prels, m_inputStream + LHCb::PackedRelationsLocation::InStream );
    if ( msgLevel( MSG::DEBUG ) ) debug() << "=== Process Relation containers :" << endmsg;
    for ( itS = names.begin(); names.end() != itS; ++itS ) 
    {
      RELATION* rels = get<RELATION>( *itS );
      if ( m_deleteInput ) toBeDeleted.push_back( rels );
      if ( rels->relations().empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) ) debug () << format( "%4d relations in ", rels->relations().size() ) << *itS << endmsg;
      packARelationContainer( rels, *prels );
    }
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Stored " << prels->relations().size() << " packed relation" << endmsg;
  }

  names.clear();
  selectContainers( root, names, m_clIdPart2Ints );
  if ( !names.empty() )
  {
    LHCb::PackedParticle2Ints* pPartIds = new LHCb::PackedParticle2Ints();
    put( pPartIds, m_inputStream + LHCb::PackedParticle2IntsLocation::InStream );
    if ( msgLevel( MSG::DEBUG ) ) debug() << "=== Process Particle2LHCbIDs containers :" << endmsg;
    for ( itS = names.begin(); names.end() != itS; ++itS ) 
    {
      DaVinci::Map::Particle2LHCbIDs* partIds = get<DaVinci::Map::Particle2LHCbIDs>( *itS );
      if ( m_deleteInput ) toBeDeleted.push_back( partIds );
      if ( partIds->empty() ) continue;
      if ( msgLevel( MSG::DEBUG ) ) debug () << format( "%4d particles2LHCbIDs in ", partIds->size() ) << *itS << endmsg;
      packAParticleLHCbIDContainer( partIds, *pPartIds );
    }
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Stored " << pPartIds->relations().size() << " packed Particle2Ints" << endmsg;
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
                                                  unsigned int classID,
                                                  bool forceRead )
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
void PackParticlesAndVertices::packAParticleContainer ( LHCb::Particles* parts, 
                                                        LHCb::PackedParticles& pparts ) {
  for ( LHCb::Particles::const_iterator iD = parts->begin();
        iD != parts->end(); ++iD ) {
    const LHCb::Particle& part = **iD;

    // Make a new packed data object and save
    pparts.data().push_back( LHCb::PackedParticle() );
    LHCb::PackedParticle& ppart = pparts.data().back();

    // reference to original container and key
    ppart.key = m_pack.referenceLong( &pparts, part.parent(), part.key() );

    // Particle ID
    ppart.particleID = part.particleID().pid();

    // Mass and error
    ppart.measMass    = m_pack.mass( part.measuredMass()    );
    ppart.measMassErr = m_pack.mass( part.measuredMassErr() );

    // Lorentz vector
    const double px = part.momentum().px();
    const double py = part.momentum().py();
    const double pz = part.momentum().pz();
    ppart.lv_px = m_pack.slope( fabs(pz) > 0 ? px/pz : 0.0 );
    ppart.lv_py = m_pack.slope( fabs(pz) > 0 ? py/pz : 0.0 );
    ppart.lv_pz = m_pack.energy( pz );
    ppart.lv_mass = (float) part.momentum().M() ;

    // reference point
    ppart.refx = m_pack.position( part.referencePoint().x() );
    ppart.refy = m_pack.position( part.referencePoint().y() );
    ppart.refz = m_pack.position( part.referencePoint().z() );

    // Mom Cov
    const double merr00 = std::sqrt( part.momCovMatrix()(0,0) );
    const double merr11 = std::sqrt( part.momCovMatrix()(1,1) );
    const double merr22 = std::sqrt( part.momCovMatrix()(2,2) );
    const double merr33 = std::sqrt( part.momCovMatrix()(3,3) );
    ppart.momCov00 = m_pack.slope( merr00/px );
    ppart.momCov11 = m_pack.slope( merr11/py );
    ppart.momCov22 = m_pack.energy( merr22 );
    ppart.momCov33 = m_pack.energy( merr33 );
    ppart.momCov10 = m_pack.fraction( part.momCovMatrix()(1,0) / (merr11*merr00) );
    ppart.momCov20 = m_pack.fraction( part.momCovMatrix()(2,0) / (merr22*merr00) );
    ppart.momCov21 = m_pack.fraction( part.momCovMatrix()(2,1) / (merr22*merr11) );
    ppart.momCov30 = m_pack.fraction( part.momCovMatrix()(3,0) / (merr33*merr00) );
    ppart.momCov31 = m_pack.fraction( part.momCovMatrix()(3,1) / (merr33*merr11) );
    ppart.momCov32 = m_pack.fraction( part.momCovMatrix()(3,2) / (merr33*merr22) );

    // Pos Cov
    const double perr00 = std::sqrt( part.posCovMatrix()(0,0) );
    const double perr11 = std::sqrt( part.posCovMatrix()(1,1) );
    const double perr22 = std::sqrt( part.posCovMatrix()(2,2) );
    ppart.posCov00 = m_pack.position( perr00 );
    ppart.posCov11 = m_pack.position( perr11 );
    ppart.posCov22 = m_pack.position( perr22 );
    ppart.posCov10 = m_pack.fraction( part.posCovMatrix()(1,0) / (perr11*perr00) );
    ppart.posCov20 = m_pack.fraction( part.posCovMatrix()(2,0) / (perr22*perr00) );
    ppart.posCov21 = m_pack.fraction( part.posCovMatrix()(2,1) / (perr22*perr11) );

    // PosMom Cov
    ppart.pmCov00 = m_pack.fltPacked( part.posMomCovMatrix()(0,0) );
    ppart.pmCov01 = m_pack.fltPacked( part.posMomCovMatrix()(0,1) );
    ppart.pmCov02 = m_pack.fltPacked( part.posMomCovMatrix()(0,2) );
    ppart.pmCov10 = m_pack.fltPacked( part.posMomCovMatrix()(1,0) );
    ppart.pmCov11 = m_pack.fltPacked( part.posMomCovMatrix()(1,1) );
    ppart.pmCov12 = m_pack.fltPacked( part.posMomCovMatrix()(1,2) );
    ppart.pmCov20 = m_pack.fltPacked( part.posMomCovMatrix()(2,0) );
    ppart.pmCov21 = m_pack.fltPacked( part.posMomCovMatrix()(2,1) );
    ppart.pmCov22 = m_pack.fltPacked( part.posMomCovMatrix()(2,2) );
    ppart.pmCov30 = m_pack.fltPacked( part.posMomCovMatrix()(3,0) );
    ppart.pmCov31 = m_pack.fltPacked( part.posMomCovMatrix()(3,1) );
    ppart.pmCov32 = m_pack.fltPacked( part.posMomCovMatrix()(3,2) );

    // extra info
    ppart.firstExtra = pparts.extra().size();
    for ( LHCb::Particle::ExtraInfo::const_iterator iE = part.extraInfo().begin();
          iE != part.extraInfo().end(); ++iE ) {
      pparts.extra().push_back( std::pair<int,int>( iE->first, m_pack.fltPacked(iE->second)) );
    }
    ppart.lastExtra = pparts.extra().size();

    // end vertex
    if ( part.endVertex() ) {
      ppart.vertex = m_pack.referenceLong( &pparts,
                                           part.endVertex()->parent(),
                                           part.endVertex()->key() );
    }

    // protoparticle
    if ( part.proto() ) {
      ppart.proto = m_pack.referenceLong( &pparts,
                                          part.proto()->parent(),
                                          part.proto()->key() );
    }

    // daughters
    ppart.firstDaughter = pparts.daughters().size();
    for ( SmartRefVector<LHCb::Particle>::const_iterator iD1 = part.daughters().begin();
          iD1 != part.daughters().end(); ++iD1 ) {
      const LHCb::Particle * mcP = *iD1;
      if ( mcP ) {
        pparts.daughters().push_back( m_pack.referenceLong( &pparts,
                                                            mcP->parent(),
                                                            mcP->key() ) );
      }
    }
    ppart.lastDaughter = pparts.daughters().size();
  }
  if ( !m_deleteInput ) parts->registry()->setAddress( 0 );
}
//=========================================================================
//  Pack a container of vertices in the PackedVertices object
//=========================================================================
void PackParticlesAndVertices::packAVertexContainer ( LHCb::Vertices* verts, 
                                                      LHCb::PackedVertices& pverts ) {
  for ( LHCb::Vertices::const_iterator iD = verts->begin();
        iD != verts->end(); ++iD ) {
    const LHCb::Vertex& vert = **iD;

    // Make a new packed data object and save
    pverts.data().push_back( LHCb::PackedVertex() );
    LHCb::PackedVertex& pvert = pverts.data().back();
    
    // reference to original container and key
    pvert.key = m_pack.referenceLong( &pverts, vert.parent(), vert.key() );
    pvert.technique = static_cast<int>(vert.technique());

    // outgoing particles
    pvert.firstOutgoingPart = pverts.outgoingParticles().size();
    for ( SmartRefVector<LHCb::Particle>::const_iterator iP = vert.outgoingParticles().begin();
          iP != vert.outgoingParticles().end(); ++iP ) {
      const LHCb::Particle * mcP = *iP;
      if ( mcP ) {
        pverts.outgoingParticles().push_back( m_pack.referenceLong( &pverts,
                                                                    mcP->parent(),
                                                                    mcP->key() ) );
      }
    }
    pvert.lastOutgoingPart = pverts.outgoingParticles().size();    
  }
  if ( !m_deleteInput ) verts->registry()->setAddress( 0 );
}
//=========================================================================
//  Pack a container of vertices in the PackedVertices object
//=========================================================================
void PackParticlesAndVertices::packARelationContainer ( RELATION* rels, 
                                                        LHCb::PackedRelations& prels ) {
  // Make a new packed data object and save
  prels.relations().push_back( LHCb::PackedRelation() );
  LHCb::PackedRelation& prel = prels.relations().back();
  // reference to original container and key
  prel.container = m_pack.referenceLong( &prels, rels, 0 );
  prel.start     = prels.sources().size();
  RELATION::Range all = rels->relations();
  for ( RELATION::Range::iterator itR = all.begin(); all.end() != itR; ++itR ) {
    int src = m_pack.referenceLong( &prels, (*itR).from()->parent(), (*itR).from()->key() );
    prels.sources().push_back( src );
    int dst = m_pack.referenceLong( &prels, (*itR).to()->parent(), (*itR).to()->key() );
    prels.dests().push_back( dst );
  }
  prel.end       = prels.sources().size();
  if ( !m_deleteInput ) rels->registry()->setAddress( 0 );
}

//=========================================================================
//  Pack a container of RecVertex
//=========================================================================
void PackParticlesAndVertices::packARecVertexContainer( LHCb::RecVertices* rVerts, 
                                                        LHCb::PackedRecVertices& pRVerts ) {

  for ( LHCb::RecVertices::const_iterator iD = rVerts->begin();
        iD != rVerts->end(); ++iD ) {
    const LHCb::RecVertex& rVert = **iD;
    // Make a new packed data object and save
    pRVerts.vertices().push_back( LHCb::PackedRecVertex() );
    LHCb::PackedRecVertex& pRVert = pRVerts.vertices().back();

    // reference to original container and key
    pRVert.key        = m_pack.referenceLong( &pRVerts, rVert.parent(), rVert.key() );
    pRVert.technique  = rVert.technique();
    pRVert.chi2       = m_pack.fltPacked( rVert.chi2() );
    pRVert.nDoF       = rVert.nDoF();
    pRVert.x          = m_pack.position( rVert.position().x() );
    pRVert.y          = m_pack.position( rVert.position().y() );
    pRVert.z          = m_pack.position( rVert.position().z() );

    // convariance Matrix
    double err0 = std::sqrt( rVert.covMatrix()(0,0) );
    double err1 = std::sqrt( rVert.covMatrix()(1,1) );
    double err2 = std::sqrt( rVert.covMatrix()(2,2) );
    
    pRVert.cov00 = m_pack.position( err0 );
    pRVert.cov11 = m_pack.position( err1 );
    pRVert.cov22 = m_pack.position( err2 );
    pRVert.cov10 = m_pack.fraction( rVert.covMatrix()(1,0)/err1/err0 );
    pRVert.cov20 = m_pack.fraction( rVert.covMatrix()(2,0)/err2/err0 );
    pRVert.cov21 = m_pack.fraction( rVert.covMatrix()(2,1)/err2/err1 );

    //== Store the Tracks
    pRVert.firstTrack = pRVerts.sizeRefs();
    for ( SmartRefVector<LHCb::Track>::const_iterator itT = rVert.tracks().begin();
          rVert.tracks().end() != itT; ++itT ) {
      SmartRef<LHCb::Track> refTr = (*itT);
      LinkManager::Link* myLink =  rVerts->linkMgr()->link( refTr.hintID() );
      if ( NULL != myLink ) {
        const std::string & containerName = myLink->path();
        int myRef = m_pack.reference( &pRVerts, containerName, refTr.linkID() );
        pRVerts.addRef( myRef );
      }
    }
    pRVert.lastTrack = pRVerts.sizeRefs();

    //== Handles the ExtraInfo
    pRVert.firstInfo = pRVerts.sizeExtra();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = rVert.extraInfo().begin(); 
          rVert.extraInfo().end() != itE; ++itE ) {
      pRVerts.addExtra( (*itE).first, m_pack.fltPacked( (*itE).second ) );
    }
    pRVert.lastInfo = pRVerts.sizeExtra();
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  if ( !m_deleteInput ) rVerts->registry()->setAddress( 0 );  
}

//=========================================================================
//  Pack a container of vertices in the PackedVertices object
//=========================================================================
void PackParticlesAndVertices::packAParticleLHCbIDContainer ( DaVinci::Map::Particle2LHCbIDs* partIds, 
                                                              LHCb::PackedParticle2Ints& pPartIds ) {
  
  for ( unsigned int kk = 0; partIds->size() > kk; ++kk ) {
    // Make a new packed data object and save
    pPartIds.relations().push_back( LHCb::PackedParticle2Int() );
    LHCb::PackedParticle2Int& pPartId = pPartIds.relations().back();
    pPartId.key = m_pack.referenceLong( &pPartIds, partIds, kk );
    const LHCb::Particle* part = partIds->key_at(kk);
    const std::vector<LHCb::LHCbID> & ids = partIds->value_at(kk);
    // reference to original container and key
    pPartId.container = m_pack.referenceLong( &pPartIds, part->parent(), part->key() );
    pPartId.start     = pPartIds.ints().size();
    for ( std::vector<LHCb::LHCbID>::const_iterator itId = ids.begin(); ids.end() != itId; ++itId ) {
      pPartIds.ints().push_back( (*itId).lhcbID() );
    }
    pPartId.end       = pPartIds.ints().size();
  }
  if ( !m_deleteInput ) partIds->registry()->setAddress( 0 );
}

//=============================================================================
