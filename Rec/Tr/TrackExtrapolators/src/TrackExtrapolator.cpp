// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackExtrapolator.h"

// Declaration of the Tool Factory
static const  ToolFactory<TrackExtrapolator>          s_factory ;
const        IToolFactory& TrackExtrapolatorFactory = s_factory ; 

//=============================================================================
// Propagate a track to a given z-position
//=============================================================================
StatusCode TrackExtrapolator::propagate( const Track& track,
                                         double z,
                                         State& state,
                                         ParticleID pid )
{
  // get state closest to z
  const State& closest = track.closestState( z );
  state = closest;

  // propagate the closest state
  StatusCode sc = propagate( state, z, pid );

  return sc;
}

//=============================================================================
// Propagate a track to the intersection point with a given plane
//=============================================================================
StatusCode TrackExtrapolator::propagate( const Track& track,
                                         const HepPlane3D& plane,
                                         State& state,
                                         ParticleID pid )
{
  // get state closest to the plane
  const State& closest = track.closestState( plane );
  state = closest;

  // propagate the closest state
  StatusCode sc = propagate( state, plane, pid );

  return sc;
}

//=============================================================================
// Retrieve the position and momentum vectors and the corresponding
// 6D covariance matrix (pos:1->3,mom:4-6) of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::positionAndMomentum( const Track& track,
                                                double z,
                                                HepPoint3D& pos,
                                                HepVector3D& mom,
                                                HepSymMatrix& cov6D,
                                                ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) 
    tmpState.positionAndMomentum( pos, mom, cov6D );

  return sc;
}

//=============================================================================
// Retrieve the position and momentum vectors and the corresponding
// 6D covariance matrix (pos:1->3,mom:4-6) at the intersection of a
// track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::positionAndMomentum( const Track& track,
                                                const HepPlane3D& plane,
                                                HepPoint3D& pos,
                                                HepVector3D& mom,
                                                HepSymMatrix& cov6D,
                                                ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) 
    tmpState.positionAndMomentum( pos, mom, cov6D );

  return sc;
}

//=============================================================================
// Retrieve the position and momentum vectors of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::positionAndMomentum( const Track& track,
                                                double z,
                                                HepPoint3D& pos,
                                                HepVector3D& mom,
                                                ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    pos = tmpState.position();
    mom = tmpState.momentum();
  }
  
  return sc;

}

//=============================================================================
// Retrieve the position and momentum vectors at the intersection of a
// track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::positionAndMomentum( const Track& track,
                                                const HepPlane3D& plane,
                                                HepPoint3D& pos,
                                                HepVector3D& mom,
                                                ParticleID pid )
{

  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    pos = tmpState.position();
    mom = tmpState.momentum();
  }
  
  return sc;
}

//=============================================================================
// Retrieve the 3D-position vector and error matrix of a track
// at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::position( const Track& track,
                                     double z,
                                     HepPoint3D& pos,
                                     HepSymMatrix& errPos,
                                     ParticleID pid )

{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    pos    = tmpState.position();
    errPos = tmpState.errPosition();
  }
  
  return sc;
}

//=============================================================================
// Retrieve the 3D-position vector and error matrix at the intersection of a
// track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::position( const Track& track,
                                     const HepPlane3D& plane,
                                     HepPoint3D& pos,
                                     HepSymMatrix& errPos,
                                     ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    pos    = tmpState.position();
    errPos = tmpState.errPosition();
  }
  return sc;
}

//=============================================================================
// Retrieve the 3D-position vector of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::position( const Track& track,
                                     double z,
                                     HepPoint3D& pos,
                                     ParticleID pid )

{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    pos    = tmpState.position();
  }
  
  return sc;
}

//=============================================================================
// Retrieve the 3D-position vector at the intersection of a
// track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::position( const Track& track,
                                     const HepPlane3D& plane,
                                     HepPoint3D& pos,
                                     ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    pos    = tmpState.position();
  }

  return sc;
}

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix of a
// track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::slopes( const Track& track,
                                   double z,
                                   HepVector3D& slopes,
                                   HepSymMatrix& errSlopes,
                                   ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    slopes    = tmpState.slopes();
    errSlopes = tmpState.errSlopes();
  }
  
  return sc;
}

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix at the intersection
// of a track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::slopes( const Track& track,
                                   const HepPlane3D& plane,
                                   HepVector3D& slopes,
                                   HepSymMatrix& errSlopes,
                                   ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    slopes    = tmpState.slopes();
    errSlopes = tmpState.errSlopes();
  }

  return sc;
}

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::slopes( const Track& track,
                                   double z,
                                   HepVector3D& slopes,
                                   ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    slopes    = tmpState.slopes();
  }
  return sc;
}

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) at the intersection of a
// track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::slopes( const Track& track,
                                   const HepPlane3D& plane,
                                   HepVector3D& slopes,
                                   ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    slopes = tmpState.slopes();
  }
  return sc;
}

//=============================================================================
// Retrieve the momentum of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::p( const Track& track,
                              double z,
                              double& p,
                              ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    p = tmpState.p();
  }
  return sc;
}

//=============================================================================
// Retrieve the momentum at the intersection of a track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::p( const Track& track,
                              const HepPlane3D& plane,
                              double& p,
                              ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    p = tmpState.p();
  }
  
  return sc;
}

//=============================================================================
// Retrieve the transverse momentum of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::pt( const Track& track,
                               double z,
                               double& pt,
                               ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    pt = tmpState.pt();
  }
  return sc;
}

//=============================================================================
// Retrieve the transverse momentum at the intersection of a
// track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::pt( const Track& track,
                               const HepPlane3D& plane,
                               double& pt,
                               ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    pt = tmpState.pt();
  }
  return sc;

}

//=============================================================================
// Retrieve the momentum vector and error matrix of a
// track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::momentum( const Track& track,
                                     double z,
                                     HepVector3D& mom,
                                     HepSymMatrix& errMom,
                                     ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    mom    = tmpState.momentum();
    errMom = tmpState.errMomentum();
  }
  return sc;
}

//=============================================================================
// Retrieve the momentum vector and error matrix at the intersection of a
// track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::momentum( const Track& track,
                                     const HepPlane3D& plane,
                                     HepVector3D& mom,
                                     HepSymMatrix& errMom,
                                     ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    mom    = tmpState.momentum();
    errMom = tmpState.errMomentum();
  }
  return sc;

}

//=============================================================================
// Retrieve the momentum vector of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::momentum( const Track& track,
                                     double z,
                                     HepVector3D& mom,
                                     ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) {
    mom    = tmpState.momentum();
  }
  return sc;
}

//=============================================================================
// Retrieve the momentum vector at the intersection of a
// track with a given plane
//=============================================================================
StatusCode TrackExtrapolator::momentum( const Track& track,
                                     const HepPlane3D& plane,
                                     HepVector3D& mom,
                                     ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, plane, tmpState, pid );

  if ( sc.isSuccess() ) {
    mom    = tmpState.momentum();
  }
  return sc;
}

//=============================================================================
// Get reference to last used transport matrix
//=============================================================================
const HepMatrix& TrackExtrapolator::transportMatrix() const
{
  return m_F;
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackExtrapolator::TrackExtrapolator( const std::string& type,
				      const std::string& name,
				      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_F()
{
  declareInterface<ITrackExtrapolator>( this );

  // create transport matrix
  m_F = HepMatrix(5, 5, 1);
}

//=============================================================================
// Destructor
//=============================================================================
TrackExtrapolator::~TrackExtrapolator() {}; 
