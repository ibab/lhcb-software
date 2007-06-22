// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackExtrapolator.h"
#include "Event/TrackParameters.h"

#include <math.h>

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TrackExtrapolator );

//=============================================================================
// Propagate a state vector from zOld to zNew
//=============================================================================
StatusCode TrackExtrapolator::propagate( TrackVector& stateVec,
                                         double zOld,
                                         double zNew,
                                         ParticleID pid )
{
  TrackMatrix* transMat = NULL;
  StatusCode sc = propagate( stateVec, zOld, zNew, transMat, pid );

  return sc;
}


//=============================================================================
// Propagate a state vector from zOld to zNew
// Transport matrix is calulated when transMat pointer is not NULL
//=============================================================================
StatusCode TrackExtrapolator::propagate( TrackVector& stateVec,
                                         double zOld,
                                         double zNew,
                                         TrackMatrix* transMat,
                                         ParticleID pid )
{
  StatusCode sc = StatusCode::FAILURE;

  Warning( "Cannot propagate state vector to given Z position, see debug.",
           StatusCode::SUCCESS, 1 );
  
  debug() << " can not propagate state vector at z" << zOld
          << " to the z position " << zNew
          << " of pid " << pid.pid() << endmsg;

  return sc;
}



//=============================================================================
// Propagate a state vector from zOld to zNew
//=============================================================================
StatusCode TrackExtrapolator::propagate(LHCb::StateVector& state, double z, Gaudi::TrackMatrix* transportmatrix,
					LHCb::ParticleID pid) 
{
  StatusCode sc = propagate(state.parameters(),state.z(),z,transportmatrix,pid) ;
  if (sc.isSuccess()) state.setZ(z);
  return sc;
}

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
// Propagate a state to a given z-position
//=============================================================================
StatusCode TrackExtrapolator::propagate( State& state, 
                                         double z,
                                         ParticleID pid )
{
  StatusCode sc = propagate( state.stateVector(), state.z(), z, &m_F, pid );
  state.setZ(z);
  state.setCovariance( ROOT::Math::Similarity<double,TrackMatrix::kRows,TrackMatrix::kCols>
                       ( m_F, state.covariance() ) );
  
  return sc;
}

//=============================================================================
// Propagate a track to the closest point to the specified point
//=============================================================================
StatusCode TrackExtrapolator::propagate( const Track& track,
                                         const Gaudi::XYZPoint& point,
                                         ParticleID  pid )
{
  // get state closest to z of point
  const State& closest = track.closestState( point.z() );
  State& state = const_cast<State&>(closest);
  
  // propagate the closest state
  StatusCode sc = propagate( state, point.z(), pid );

  return sc;
}

//=============================================================================
// Propagate a state to the closest point to the specified point
//=============================================================================
StatusCode TrackExtrapolator::propagate( State& state,
                                         const Gaudi::XYZPoint& point,
                                         ParticleID  pid )
{
  StatusCode sc = StatusCode::FAILURE;

  Warning( "Cannot propagate state to Z at given point. See debug for details",
           StatusCode::SUCCESS, 1 );
  
  debug() << " can not propagate state at " << state.z()
          << " to point at z " << point.z()
          << " of pid " << pid.pid() << endmsg;

  return sc;
}

//=============================================================================
// Propagate a track to within tolerance of a plane (default = 10 microns)
//=============================================================================
StatusCode TrackExtrapolator::propagate( const Track& track,
                                         Gaudi::Plane3D& plane,
                                         double tolerance,
                                         ParticleID pid )
{
  // get state closest to the plane
  const State& closest = track.closestState( plane );
  State& state = const_cast<State&>(closest);
  
  // propagate the closest state
  StatusCode sc = propagate( state, plane, tolerance, pid );

  return sc;
}

//=============================================================================
// Propagate a state to within tolerance of a plane (default = 10 microns)
//=============================================================================
StatusCode TrackExtrapolator::propagate( State& state,
                                         Gaudi::Plane3D& plane,
                                         double tolerance,
                                         ParticleID pid )
{
  StatusCode sc = StatusCode::FAILURE;

  // the distance is signed!
  double distance = fabs( plane.Distance( state.position() ) );
  if( distance < tolerance ) { return StatusCode::SUCCESS; }

  for( int iter = 0; iter < m_maxIter; ++iter ) {
    distance = fabs( plane.Distance( state.position() ) );
    XYZPoint inPlane( plane.ProjectOntoPlane( state.position() ) ); 
    sc = propagate( state, inPlane.z(), pid );
    if( sc.isFailure() ) {
      error() << "Failed to propagate to z = " << inPlane.z() << endreq;
      return sc;
    }
    if( distance < tolerance ) {
      debug() << "Success in iteration # " << iter << endreq; 
      return StatusCode::SUCCESS;
    }
  }
  if( sc.isFailure() ) {
    error() << "Failed to propagate to plane within tolerance." << endreq;
  }
  
  return sc;
}


//--------------------------------------------------------------------------
//  ACCESS METHODS
//--------------------------------------------------------------------------

//=============================================================================
// Retrieve the position and momentum vectors and the corresponding
// 6D covariance matrix (pos:1->3,mom:4-6) of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::positionAndMomentum( const Track& track,
                                                   double z,
                                                   Gaudi::XYZPoint& pos,
                                                   Gaudi::XYZVector& mom,
                                                   Gaudi::SymMatrix6x6& cov6D,
                                                   ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) tmpState.positionAndMomentum( pos, mom, cov6D );

  return sc;
}

//=============================================================================
// Retrieve the position and momentum vectors of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::positionAndMomentum( const Track& track,
                                                   double z,
                                                   Gaudi::XYZPoint& pos,
                                                   Gaudi::XYZVector& mom,
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
// Retrieve the 3D-position vector and error matrix of a track
// at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::position( const Track& track,
                                        double z,
                                        Gaudi::XYZPoint& pos,
                                        Gaudi::SymMatrix3x3& errPos,
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
// Retrieve the 3D-position vector of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::position( const Track& track,
                                        double z,
                                        Gaudi::XYZPoint& pos,
                                        ParticleID pid )

{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) pos = tmpState.position();
  
  return sc;
}

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix of a
// track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::slopes( const Track& track,
                                      double z,
                                      Gaudi::XYZVector& slopes,
                                      Gaudi::SymMatrix3x3& errSlopes,
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
// Retrieve the slopes (dx/dz,dy/dz,1) of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::slopes( const Track& track,
                                      double z,
                                      Gaudi::XYZVector& slopes,
                                      ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) slopes = tmpState.slopes();

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

  if ( sc.isSuccess() ) p = tmpState.p();
  
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

  if ( sc.isSuccess() ) pt = tmpState.pt();

  return sc;
}

//=============================================================================
// Retrieve the momentum vector and error matrix of a
// track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::momentum( const Track& track,
                                        double z,
                                        Gaudi::XYZVector& mom,
                                        Gaudi::SymMatrix3x3& errMom,
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
// Retrieve the momentum vector of a track at a given z-position
//=============================================================================
StatusCode TrackExtrapolator::momentum( const Track& track,
                                        double z,
                                        Gaudi::XYZVector& mom,
                                        ParticleID pid )
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) mom = tmpState.momentum();

  return sc;
}

//=============================================================================
// Get reference to last used transport matrix
//=============================================================================
const TrackMatrix& TrackExtrapolator::transportMatrix() const
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

  declareProperty( "Iterations", m_maxIter = 5 );

  // create transport matrix
  m_F = TrackMatrix( ROOT::Math::SMatrixIdentity() );
}

//=============================================================================
// Destructor
//=============================================================================
TrackExtrapolator::~TrackExtrapolator() {}; 

//=============================================================================
