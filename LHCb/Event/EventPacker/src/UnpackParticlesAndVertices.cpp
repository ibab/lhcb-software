// $Id: $
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

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackParticlesAndVertices )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  UnpackParticlesAndVertices::UnpackParticlesAndVertices( const std::string& name,
                                                          ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputStream", m_inputStream = "/Event" );
  declareProperty( "PostFix",     m_postFix     = "" );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackParticlesAndVertices::~UnpackParticlesAndVertices() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackParticlesAndVertices::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  int nbContainer = 0;
  int nbPart = 0;

  int prevLink = -1;
  LHCb::Particles* parts = NULL;
  if ( exist<LHCb::PackedParticles>(m_inputStream+LHCb::PackedParticleLocation::InStream) )
  {
    LHCb::PackedParticles* pparts =
      get<LHCb::PackedParticles>( m_inputStream + LHCb::PackedParticleLocation::InStream );
    for ( std::vector<LHCb::PackedParticle>::iterator itP = pparts->data().begin();
          pparts->data().end() != itP; ++itP )
    {
      const LHCb::PackedParticle& ppart = *itP;
      int key    = ppart.key & 0x0000FFFF;
      int linkID = ppart.key >> 16;
      if ( linkID != prevLink ) {
        prevLink = linkID;
        std::string containerName = pparts->linkMgr()->link( linkID )->path() + m_postFix;
        parts = new LHCb::Particles();
        put( parts, containerName );
        nbContainer++;
      }
      //== Construct with particle ID and key.
      LHCb::Particle* part = new LHCb::Particle( LHCb::ParticleID(ppart.particleID), key );
      parts->add( part );
      nbPart++;

      // Mass and error
      part->setMeasuredMass   ( m_pack.mass(ppart.measMass) );
      part->setMeasuredMassErr( m_pack.mass(ppart.measMassErr) );

      // Lorentz momentum vector
      const double pz   = m_pack.energy( ppart.lv_pz );
      const double px   = m_pack.slope( ppart.lv_px ) * pz;
      const double py   = m_pack.slope( ppart.lv_py ) * pz;
      const double mass = ppart.lv_mass;
      part->setMomentum( Gaudi::LorentzVector( px, py, pz,
                                               std::sqrt(px*px+py*py+pz*pz+mass*mass) ) );

      // reference point
      part->setReferencePoint( Gaudi::XYZPoint( m_pack.position(ppart.refx),
                                                m_pack.position(ppart.refy),
                                                m_pack.position(ppart.refz) ) );

      // Mom Cov
      Gaudi::SymMatrix4x4 & momCov = *(const_cast<Gaudi::SymMatrix4x4*>(&part->momCovMatrix()));
      const double merr00 = m_pack.slope( ppart.momCov00 ) * px;
      const double merr11 = m_pack.slope( ppart.momCov11 ) * py;
      const double merr22 = m_pack.energy( ppart.momCov22 );
      const double merr33 = m_pack.energy( ppart.momCov33 );
      momCov(0,0) = std::pow( merr00, 2 );
      momCov(1,1) = std::pow( merr11, 2 );
      momCov(2,2) = std::pow( merr22, 2 );
      momCov(3,3) = std::pow( merr33, 2 );
      momCov(1,0) = merr11*merr00 * m_pack.fraction( ppart.momCov10 );
      momCov(2,0) = merr22*merr00 * m_pack.fraction( ppart.momCov20 );
      momCov(2,1) = merr22*merr11 * m_pack.fraction( ppart.momCov21 );
      momCov(3,0) = merr33*merr00 * m_pack.fraction( ppart.momCov30 );
      momCov(3,1) = merr33*merr11 * m_pack.fraction( ppart.momCov31 );
      momCov(3,2) = merr33*merr22 * m_pack.fraction( ppart.momCov32 );

      // Pos Cov
      Gaudi::SymMatrix3x3 & posCov = *(const_cast<Gaudi::SymMatrix3x3*>(&part->posCovMatrix()));
      const double perr00 = m_pack.position( ppart.posCov00 );
      const double perr11 = m_pack.position( ppart.posCov11 );
      const double perr22 = m_pack.position( ppart.posCov22 );
      posCov(0,0) = std::pow( perr00, 2 );
      posCov(1,1) = std::pow( perr11, 2 );
      posCov(2,2) = std::pow( perr22, 2 );
      posCov(1,0) = perr11*perr00 * m_pack.fraction( ppart.posCov10 );
      posCov(2,0) = perr22*perr00 * m_pack.fraction( ppart.posCov20 );
      posCov(2,1) = perr22*perr11 * m_pack.fraction( ppart.posCov21 );

      // Pos Mom Cov
      Gaudi::Matrix4x3 & pmCov = *(const_cast<Gaudi::Matrix4x3*>(&part->posMomCovMatrix()));
      pmCov(0,0) = m_pack.fltPacked( ppart.pmCov00 );
      pmCov(0,1) = m_pack.fltPacked( ppart.pmCov01 );
      pmCov(0,2) = m_pack.fltPacked( ppart.pmCov02 );
      pmCov(1,0) = m_pack.fltPacked( ppart.pmCov10 );
      pmCov(1,1) = m_pack.fltPacked( ppart.pmCov11 );
      pmCov(1,2) = m_pack.fltPacked( ppart.pmCov12 );
      pmCov(2,0) = m_pack.fltPacked( ppart.pmCov20 );
      pmCov(2,1) = m_pack.fltPacked( ppart.pmCov21 );
      pmCov(2,2) = m_pack.fltPacked( ppart.pmCov22 );
      pmCov(3,0) = m_pack.fltPacked( ppart.pmCov30 );
      pmCov(3,1) = m_pack.fltPacked( ppart.pmCov31 );
      pmCov(3,2) = m_pack.fltPacked( ppart.pmCov32 );

      // extra info
      for ( int iE = ppart.firstExtra; iE < ppart.lastExtra; ++iE ) {
        const LHCb::PackedParticles::PackedExtraInfo& pInfo =  pparts->extra()[iE];
        part->addInfo( pInfo.first, m_pack.fltPacked(pInfo.second) );
      }

      // end vertex
      if ( -1 != ppart.vertex ) {
        int hintID(0), key(0);
        m_pack.hintAndKey( ppart.vertex, pparts, parts, hintID, key );
        SmartRef<LHCb::Vertex> ref(parts,hintID,key);
        part->setEndVertex( ref );
      }

      // protoparticle
      if ( -1 != ppart.proto ) {
        int hintID(0), key(0);
        m_pack.hintAndKeyLong( ppart.proto, pparts, parts, hintID, key );
        SmartRef<LHCb::ProtoParticle> ref(parts,hintID,key);
        part->setProto( ref );
      }

      // daughters

      for ( unsigned short int iiD = ppart.firstDaughter; iiD < ppart.lastDaughter; ++iiD ) {
        const int & iD1 = pparts->daughters()[iiD];
        int hintID(0), key(0);
        m_pack.hintAndKeyLong( iD1, pparts, parts, hintID, key );
        SmartRef<LHCb::Particle> ref(parts,hintID,key);
        part->addToDaughters( ref );
      }
    }
  }

  //=================================================================
  //== Process the vertices
  //=================================================================
  int nbVertContainer = 0;
  int nbVert = 0;
  prevLink = -1;
  LHCb::Vertices* verts = NULL;
  if ( exist<LHCb::PackedVertices>( m_inputStream + LHCb::PackedVertexLocation::InStream ) )
  {
    LHCb::PackedVertices* pverts = get<LHCb::PackedVertices>( m_inputStream + LHCb::PackedVertexLocation::InStream );
    for ( std::vector<LHCb::PackedVertex>::iterator itV = pverts->data().begin();
          pverts->data().end() != itV; ++itV )
    {
      const LHCb::PackedVertex& pvert = *itV;
      int key    = pvert.key & 0x0000FFFF;
      int linkID = pvert.key >> 16;
      if ( linkID != prevLink ) {
        prevLink = linkID;
        std::string containerName = pverts->linkMgr()->link( linkID )->path() + m_postFix;
        verts = new LHCb::Vertices();
        put( verts, containerName );
        nbVertContainer++;
      }
      //== Construct with verticle ID and key.
      LHCb::Vertex* vert = new LHCb::Vertex( key );
      verts->add( vert );
      nbVert++;

      // technique
      vert->setTechnique( static_cast<LHCb::Vertex::CreationMethod>(pvert.technique) );

      // outgoing particles
      for ( unsigned short int iiP = pvert.firstOutgoingPart; iiP < pvert.lastOutgoingPart; ++iiP ) {
        const int & iP = pverts->outgoingParticles()[iiP];
        int hintID(0), key(0);
        m_pack.hintAndKey( iP, pverts, verts, hintID, key );
        SmartRef<LHCb::Particle> ref(verts,hintID,key);
        vert->addToOutgoingParticles( ref );
      }
    }
  }

  //=================================================================
  //== Process the RecVertices
  //=================================================================
  int nbRecVertContainer = 0;
  int nbRecVert = 0;
  prevLink = -1;
  LHCb::RecVertices* recVerts = NULL;
  if ( exist<LHCb::PackedRecVertices>( m_inputStream + LHCb::PackedRecVertexLocation::InStream ) )
  {
    LHCb::PackedRecVertices* pRecVerts =
      get<LHCb::PackedRecVertices>( m_inputStream + LHCb::PackedRecVertexLocation::InStream );
    for ( std::vector<LHCb::PackedRecVertex>::iterator itV = pRecVerts->vertices().begin();
          pRecVerts->vertices().end() != itV; ++itV ) {
      const LHCb::PackedRecVertex& pRecVert = *itV;
      int key    = pRecVert.key & 0x0000FFFF;
      int linkID = pRecVert.key >> 16;
      if ( linkID != prevLink ) {
        prevLink = linkID;
        std::string containerName = pRecVerts->linkMgr()->link( linkID )->path() + m_postFix;
        recVerts = new LHCb::RecVertices();
        put( recVerts, containerName );
        nbRecVertContainer++;
      }
      //== Construct with RecVerticle ID and key.
      LHCb::RecVertex* recVert = new LHCb::RecVertex( key );
      recVerts->add( recVert );
      nbRecVert++;
      recVert->setTechnique( (LHCb::RecVertex::RecVertexType) pRecVert.technique );
      recVert->setChi2AndDoF( m_pack.fltPacked( pRecVert.chi2), pRecVert.nDoF );
      Gaudi::XYZPoint pos( m_pack.position( pRecVert.x ), m_pack.position( pRecVert.y ),
                           m_pack.position( pRecVert.z ) );
      recVert->setPosition( pos );

      // convariance Matrix
      double err0 = m_pack.position( pRecVert.cov00 );
      double err1 = m_pack.position( pRecVert.cov11 );
      double err2 = m_pack.position( pRecVert.cov22 );
      Gaudi::SymMatrix3x3  cov;
      cov(0,0) = err0 * err0;
      cov(1,0) = err1 * err0 * m_pack.fraction( pRecVert.cov10 );
      cov(1,1) = err1 * err1;
      cov(2,0) = err2 * err0 * m_pack.fraction( pRecVert.cov20 );
      cov(2,1) = err2 * err1 * m_pack.fraction( pRecVert.cov21 );
      cov(2,2) = err2 * err2;
      recVert->setCovMatrix( cov );

      //== Store the Tracks
      int hintID;
      int tKey;
      for ( int kk = pRecVert.firstTrack; pRecVert.lastTrack > kk; ++kk ) {
        const int trk = *(pRecVerts->beginRefs()+kk);
        m_pack.hintAndKey( trk, pRecVerts, recVerts, hintID, tKey );
        SmartRef<LHCb::Track> ref( recVerts, hintID, tKey );
        recVert->addToTracks( ref );
      }

      //== Handles the ExtraInfo
      for ( int kEx = pRecVert.firstInfo; pRecVert.lastInfo > kEx; ++kEx ) {
        const std::pair<int,int>& info = *(pRecVerts->beginExtra()+kEx);
        recVert->addInfo( info.first, m_pack.fltPacked( info.second ) );
      }
    }
  }

  //=================================================================
  //== Process the relations
  //=================================================================
  int nbRelContainer = 0;
  int nbRel = 0;
  RELATION* rels = NULL;
  if ( exist<LHCb::PackedRelations>( m_inputStream + LHCb::PackedRelationsLocation::InStream ) )
  {
    LHCb::PackedRelations* prels =
      get<LHCb::PackedRelations>( m_inputStream + LHCb::PackedRelationsLocation::InStream );
    for ( std::vector<LHCb::PackedRelation>::iterator itR = prels->relations().begin();
          prels->relations().end() != itR; ++itR )
    {
      const LHCb::PackedRelation& prel = *itR;
      int indx = prel.container >> 16;
      std::string containerName = prels->linkMgr()->link( indx )->path() + m_postFix;
      rels = new RELATION();
      put( rels, containerName );
      nbRelContainer++;
      LHCb::Particles* srcContainer = NULL;
      int prevSrcLink = -1;
      DataObject* dstContainer = NULL;
      int prevDstLink = -1;
      for ( int kk = prel.start;  prel.end > kk; ++kk ) {
        int srcLink = prels->sources()[kk] >> 16;
        int srcKey  = prels->sources()[kk] & 0xFFFF;
        if ( srcLink != prevSrcLink ) {
          prevSrcLink = srcLink;
          std::string srcName = prels->linkMgr()->link( srcLink )->path();
          srcContainer = get<LHCb::Particles>( srcName );
        }
        LHCb::Particle* from = srcContainer->object( srcKey );
        int dstLink = prels->dests()[kk] >> 16;
        int dstKey  = prels->dests()[kk] & 0xFFFF;
        if ( dstLink != prevDstLink ) {
          prevDstLink = dstLink;
          std::string dstName = prels->linkMgr()->link( dstLink )->path();
          dstContainer = get<DataObject>( dstName );
        }
        LHCb::VertexBase* to = NULL;
        if ( 0 != dynamic_cast<LHCb::RecVertices*>(dstContainer) ) {
          to = (dynamic_cast<LHCb::RecVertices*>(dstContainer))->object( dstKey );
        }
        if ( NULL == to ) info() << "Unknown objec: Container type " << (dstContainer->clID()>>16)
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
  if ( exist<LHCb::PackedParticle2Ints>( m_inputStream + LHCb::PackedParticle2IntsLocation::InStream ) )
  {
    LHCb::PackedParticle2Ints* pPartIds =
      get<LHCb::PackedParticle2Ints>( m_inputStream +
                                      LHCb::PackedParticle2IntsLocation::InStream );
    for ( std::vector<LHCb::PackedParticle2Int>::iterator itL = pPartIds->relations().begin();
          pPartIds->relations().end() != itL; ++itL ) {
      const LHCb::PackedParticle2Int& pPartId = *itL;
      const int linkID = pPartId.key >> 16;
      if ( linkID != prevLink ) {
        prevLink = linkID;
        const std::string containerName = pPartIds->linkMgr()->link( linkID )->path() + m_postFix;
        partIds = new DaVinci::Map::Particle2LHCbIDs();
        put( partIds, containerName );
        nbPartIdContainer++;
      }
      nbPartId++;
      const int partLink = pPartId.container >> 16;
      const int partKey  = pPartId.container & 0xFFFF;
      if ( partLink != prevPartLink ) {
        prevPartLink = partLink;
        const std::string partName = pPartIds->linkMgr()->link( partLink )->path();
        partContainer = get<LHCb::Particles>( partName );
      }
      LHCb::Particle* part = partContainer->object( partKey );
      std::vector<LHCb::LHCbID> temp;
      for ( int kk = pPartId.start; pPartId.end != kk; ++kk ) {
        temp.push_back( LHCb::LHCbID( pPartIds->ints()[kk] ) );
      }
      partIds->insert( part, temp );
    }
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "== Retrieved " << nbPart << " Particles in " << nbContainer << " containers, "
            << nbVert << " vertices in " << nbVertContainer << " containers, "
            << nbRecVert << " RecVerticess in " << nbRecVertContainer << " containers,"
            << nbRel << " relations in " << nbRelContainer << " containers, "
            << nbPartId << " Part-LHCbID in " << nbPartIdContainer << " containers, "
            <<endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
