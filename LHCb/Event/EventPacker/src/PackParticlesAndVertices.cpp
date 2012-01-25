// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartIF.h"


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
  declareProperty( "InputStream",        m_inputStream    = "/Event" );
  declareProperty( "AlwaysCreateOutput", m_alwaysOutput = false     );
  declareProperty( "ForceReading",       m_forceReading = false     );
}
//=============================================================================
// Destructor
//=============================================================================
PackParticlesAndVertices::~PackParticlesAndVertices() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackParticlesAndVertices::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  std::vector<std::string>::const_iterator itS;

  DataObject* root = get<DataObject*>( m_inputStream );
  std::vector<std::string> names;
  if ( m_forceReading ) selectContainers( root, names, "??????????", true );  // fake name to get no container, but load.

  selectContainers( root, names, "/Particles" );
  LHCb::PackedParticles* pparts = new LHCb::PackedParticles();
  put( pparts, m_inputStream + "/PackedParticles" );
  
  
  if ( msgLevel( MSG::DEBUG ) ) debug() << "=== Process Particle containers :" << endmsg;
  for ( itS = names.begin(); names.end() != itS; ++itS ) {
    LHCb::Particles* parts = get<LHCb::Particles>( *itS );
    if ( parts->size() == 0 ) continue;
    if ( msgLevel( MSG::DEBUG ) ) debug() << format( "%4d particles in", parts->size() ) << *itS << endmsg;
    packAParticleContainer( parts, *pparts );
  }
  if ( msgLevel( MSG::DEBUG ) ) debug() << "Stored " << pparts->data().size() << " packed tracks" << endmsg;

  names.clear();
  selectContainers( root, names, "/decayVertices" );
  LHCb::PackedVertices* pverts = new LHCb::PackedVertices();
  put( pverts, m_inputStream + "/PackedVertices" );

  if ( msgLevel( MSG::DEBUG ) ) debug() << "=== Process Vertex containers :" << endmsg;
  for ( itS = names.begin(); names.end() != itS; ++itS ) {
    LHCb::Vertices* verts = get<LHCb::Vertices>( *itS );
    if ( verts->size() == 0 ) continue;
    if ( msgLevel( MSG::DEBUG ) ) debug () << format( "%4d vertices in", verts->size() ) << *itS << endmsg;
    packAVertexContainer( verts, *pverts );
  }
  if ( msgLevel( MSG::DEBUG ) ) debug() << "Stored " << pverts->data().size() << " packed vertices" << endmsg;

  names.clear();
  selectContainers( root, names, "/Particle2VertexRelations" );
  LHCb::PackedRelations* prels = new LHCb::PackedRelations();
  put( prels, m_inputStream + "/PackedRelations" );

  if ( msgLevel( MSG::DEBUG ) ) debug() << "=== Process Relation containers :" << endmsg;
  for ( itS = names.begin(); names.end() != itS; ++itS ) {
    RELATION* rels = get<RELATION>( *itS );
    if ( rels->relations().size() == 0 ) continue;
    if ( msgLevel( MSG::DEBUG ) ) debug () << format( "%4d relations in", rels->relations().size() ) << *itS << endmsg;
    packARelationContainer( rels, *prels );
  }
  if ( msgLevel( MSG::DEBUG ) ) debug() << "Stored " << prels->relations().size() << " packed relation" << endmsg;
  
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Select iteratively the containers ending with 'Prefix'
//=========================================================================
void PackParticlesAndVertices::selectContainers ( DataObject* obj, std::vector<std::string>& names, 
                                          std::string postFix, bool forceRead ) {
  SmartIF<IDataManagerSvc> mgr( eventSvc() );
  typedef std::vector<IRegistry*> Leaves;
  unsigned int postSize = postFix.size();
  Leaves leaves;
  StatusCode sc = mgr->objectLeaves( obj, leaves );
  if ( sc ) {
    for ( Leaves::const_iterator iL = leaves.begin(); leaves.end() != iL; ++iL) {
      const std::string id = (*iL)->identifier();
      DataObject* tmp;
      if ( forceRead ) {
        sc = eventSvc()->retrieveObject( id, tmp );
      } else {
        sc = eventSvc()->findObject( id, tmp );
      }
      if ( sc ) {
        if ( postSize < id.size() &&
             id.substr( id.size()-postSize ) == postFix ) {
          if( msgLevel(MSG::DEBUG) ) info() << "Found container " << id << endmsg;
          names.push_back( id );
        }
        selectContainers( tmp, names, postFix, forceRead );
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
  parts->registry()->setAddress( 0 );
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
  verts->registry()->setAddress( 0 );
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
}
//=============================================================================
