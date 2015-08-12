// Include files 

// from Gaudi
#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Linker/LinkedTo.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// local
#include "MeasureIPResolution.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MeasureIPResolution
//
// 2010-10-05 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MeasureIPResolution )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MeasureIPResolution::MeasureIPResolution( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "ContainerName", m_containerName = LHCb::TrackLocation::Velo );
}
//=============================================================================
// Destructor
//=============================================================================
MeasureIPResolution::~MeasureIPResolution() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MeasureIPResolution::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );

  m_nTracks     = 0;
  m_averX       = 0.;
  m_averY       = 0.;
  m_nbInCore    = 0;
  m_sumRInCore  = 0.;
  m_sumR2InCore = 0.;
  m_sumIPS      = 0.;
  m_sumIPS2     = 0.;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MeasureIPResolution::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  //== Get the MC primary vertices
  std::vector<Gaudi::XYZPoint> pvPosition;
  LHCb::MCVertices* verts = get<LHCb::MCVertices>( LHCb::MCVertexLocation::Default );
  for ( LHCb::MCVertices::iterator itV = verts->begin(); verts->end() != itV; ++itV ) {
    if ( (*itV)->isPrimary() ) {
      pvPosition.push_back( (*itV)->position() );
      debug() << format( "Found PV at %6.3f %6.3f %8.3f", (*itV)->position().x(), (*itV)->position().y(), 
                         (*itV)->position().z() ) << endmsg;
      
    }
  }

  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_containerName );
  LinkedTo<LHCb::MCParticle> trackLinker( evtSvc(), msgSvc(), m_containerName );

  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) {
    if ( (*itT)->checkFlag( LHCb::Track::Backward ) ) continue;
    LHCb::MCParticle* part = trackLinker.first( *itT );
    /*
    if ( 0 == part ) continue;
    if ( !part->originVertex()->isPrimary() ) {
      bool isClose = false;
      for ( std::vector<Gaudi::XYZPoint>::iterator itV = pvPosition.begin(); pvPosition.end() != itV; ++itV ) {
        if ( fabs( (*itV).x() - part->originVertex()->position().x() ) > 0.01 ) continue;
        if ( fabs( (*itV).y() - part->originVertex()->position().y() ) > 0.01 ) continue;
        if ( fabs( (*itV).z() - part->originVertex()->position().z() ) > 0.1 ) continue;
        isClose = true;
        break;
      }
      if ( !isClose) continue;
    }
    //if ( part->pt() < 300. ) continue;
    */

    LHCb::State& state = (*itT)->firstState();
    double minIP = 1.e9;
    double dxAtMin = 1.e9;
    double dyAtMin = 1.e9;
    for ( std::vector<Gaudi::XYZPoint>::iterator itV = pvPosition.begin(); pvPosition.end() != itV; ++itV ) {
      double dx = state.x() + state.tx() * ( (*itV).z() - state.z() ) - (*itV).x();
      double dy = state.y() + state.ty() * ( (*itV).z() - state.z() ) - (*itV).y();
      double ip = sqrt( dx*dx + dy*dy );
      if ( ip < minIP ) {
        minIP = ip;
        dxAtMin = dx;
        dyAtMin = dy;
      }
    }
    m_nTracks ++;
    m_averX += dxAtMin;
    m_averY += dyAtMin;
    double signedIP = minIP;
    if ( dxAtMin * state.tx() + dyAtMin * state.ty() > 0. ) signedIP = -signedIP;
    double ipS = signedIP / sqrt( state.errX2() + state.errY2() );
    if ( minIP < 0.5 ) {
      m_nbInCore++;
      m_sumRInCore  += signedIP;
      m_sumR2InCore += minIP * minIP;
      m_sumIPS      += ipS;
      m_sumIPS2     += ipS * ipS;
    }
    if ( msgLevel( MSG::DEBUG ) ) {
      if ( 1. < ipS ) {
        info() << format( "Track %4d : signedIP %7.3f ipS %7.3f ", (*itT)->key(), signedIP, ipS );  
        if ( 0 == part ) {
          info() << endmsg;
        } else {
          printMCParticle( part );
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MeasureIPResolution::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( m_nTracks > 0 ) {
    m_averX /= m_nTracks;
    m_averY /= m_nTracks;
    info() << format( "On %7d tracks, <x> %8.3f  <y> %8.3f", m_nTracks, m_averX, m_averY ) << endmsg;
    if ( m_nbInCore > 0 ) {
      double aver = m_sumRInCore / m_nbInCore;
      double sigma = sqrt( m_sumR2InCore/m_nbInCore - aver * aver );
      double sIPS  = sqrt( m_sumIPS2 * m_nbInCore - m_sumIPS * m_sumIPS ) / m_nbInCore;
      info() << format( "   In core: %7d tracks, <> %8.3f sigma %8.3f  sigmaIPS %8.3f outside%7d", 
                        m_nbInCore, aver, sigma, sIPS, m_nTracks - m_nbInCore ) << endmsg;
    }
  }
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Print the info on the particle
//=========================================================================
void MeasureIPResolution::printMCParticle ( const LHCb::MCParticle* part ) {
  const LHCb::MCParticle* mother = part;
  const LHCb::MCVertex*   vert   = part->originVertex();
  double p = double( int(  part->p() ) /1000. );
  info() << "MC: [" << p << " GeV]";
  while ( 0 != mother ) {
    const LHCb::ParticleProperty* pp = m_ppSvc->find( mother->particleID() );
    if ( 0 == pp ) {
      info() << mother->key() << "[" << mother->particleID().pid() <<"]";
    } else {
      info() << mother->key() << "[" <<  pp->particle() <<"]";
    }
    vert = mother->originVertex();
    if ( 0 == vert ) {
      mother = 0;
    } else {
      info() << format( " <-(z=%7.2f)", vert->position().z() );
      mother = vert->mother();
    }
  }
  info() << endreq;
}
//=============================================================================
