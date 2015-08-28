// Include files

// local
#include "TrackExtrapolator.h"
#include "Event/TrackParameters.h"

#include <math.h>

#include "LHCbMath/GeomFun.h"
#include "LHCbMath/Line.h"

using namespace LHCb;
using namespace Gaudi;

//=============================================================================
// Propagate a state vector from zOld to zNew
//=============================================================================
StatusCode TrackExtrapolator::propagate( Gaudi::TrackVector& stateVec,
                                         double zOld,
                                         double zNew,
                                         LHCb::ParticleID pid ) const
{
  TrackMatrix* transMat = NULL;
  StatusCode sc = propagate( stateVec, zOld, zNew, transMat, pid );

  return sc;
}


//=============================================================================
// Propagate a state vector from zOld to zNew
//=============================================================================
StatusCode TrackExtrapolator::propagate( LHCb::StateVector& state,
					 double z,
					 Gaudi::TrackMatrix* transportmatrix,
					 LHCb::ParticleID pid ) const
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
                                         ParticleID pid ) const
{
  // get state closest to z
  const State& closest = track.closestState( z );
  state = closest;

  // propagate the closest state
  StatusCode sc = propagate( state, z, pid );

  return sc;
}

//=============================================================================
// Propagate a track to a given z-position
//=============================================================================
StatusCode TrackExtrapolator::propagate( const Track& track,
                                         double z,
                                         StateVector& state,
                                         ParticleID pid ) const
{
  // get state closest to z
  const State& closest = track.closestState( z );
  state = LHCb::StateVector( closest.stateVector(), closest.z()) ;

  // propagate the closest state
  StatusCode sc = propagate( state, z, 0, pid );
  
  return sc;
}

//=============================================================================
// Propagate a state to a given z-position
//=============================================================================
StatusCode TrackExtrapolator::propagate( State& state, 
                                         double z,
                                         ParticleID pid ) const
{
  Gaudi::TrackMatrix transMat = ROOT::Math::SMatrixIdentity();
  StatusCode sc = propagate( state, z, &transMat, pid );
  
  return sc;
}

//=============================================================================
// Propagate a state to a given z-position
// Transport matrix is calulated when transMat pointer is not NULL
//=============================================================================
StatusCode TrackExtrapolator::propagate( State& state, 
                                         double z,
                                         Gaudi::TrackMatrix* tm,
                                         ParticleID pid ) const
{
  Gaudi::TrackMatrix transMat = ROOT::Math::SMatrixIdentity();
  StatusCode sc = propagate( state.stateVector(), state.z(), z, &transMat, pid );
  state.setZ(z);
  state.setCovariance( ROOT::Math::Similarity<double,TrackMatrix::kRows,TrackMatrix::kCols>
                       ( transMat, state.covariance() ) );
  if( tm ) *tm = transMat ;
  return sc;
}

//=============================================================================
// Propagate a track to the closest point to the specified point
//=============================================================================
StatusCode TrackExtrapolator::propagate( const Track& track,
                                         const Gaudi::XYZPoint& point,
					 LHCb::State& state,
                                         ParticleID  pid ) const
{
  // get state closest to z of point
  const State& closest = track.closestState( point.z() );
  state = closest;
  
  // propagate the closest state
  StatusCode sc = propagate( state, point.z(), pid );

  return sc;
}

//=============================================================================
// Propagate a state to the closest point to the specified point
//=============================================================================
StatusCode TrackExtrapolator::propagate( State& state,
                                         const Gaudi::XYZPoint& point,
                                         ParticleID  pid ) const
{
  StatusCode sc = StatusCode::FAILURE;

  Warning( "Cannot propagate state to Z at given point. See debug for details",
           StatusCode::SUCCESS, 0 ).ignore();
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << " can not propagate state at " << state.z()
            << " to point at z " << point.z()
            << " of pid " << pid.pid() << endmsg;

  return sc;
}

//=============================================================================
// Propagate a track to within tolerance of a plane (default = 10 microns)
//=============================================================================
StatusCode TrackExtrapolator::propagate( const Track& track,
                                         const Gaudi::Plane3D& plane,
					 LHCb::State& state,
                                         double tolerance,
                                         ParticleID pid ) const
{
  // get state closest to the plane
  const State& closest = track.closestState( plane );
  state = closest;
  
  // propagate the closest state
  StatusCode sc = propagate( state, plane, tolerance, pid );

  return sc;
}

//=============================================================================
// Propagate a state to within tolerance of a plane (default = 10 microns)
//=============================================================================
StatusCode TrackExtrapolator::propagate( State& state,
                                         const Gaudi::Plane3D& plane,
                                         double tolerance,
                                         ParticleID pid ) const
{
  StatusCode sc = StatusCode::FAILURE ;
  Gaudi::XYZPoint intersect ;
  int iter ;
  double distance ;
  for(  iter = 0; iter < m_maxIter; ++iter ) {
    Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> line( state.position(), state.slopes() ) ;
    double dz ;
    bool success = Gaudi::Math::intersection( line, plane, intersect, dz) ;
    if( !success ) {
      Warning( "State parallel to plane!" ).ignore();
      break ;
    }
    distance = ( intersect - line.beginPoint()).R() ;
    
    if( distance < tolerance ) {
      sc = StatusCode::SUCCESS ;
      break ;
    } else {
      double ztarget = state.z() + dz ;
      sc = propagate( state, ztarget, pid );
      if( sc.isFailure() ) {
        Warning( "Failed to propagate to given z. See debug for details",StatusCode::SUCCESS,0 ).ignore();
        
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "Failed to propagate to z = " << ztarget << endmsg;
        break ;
      }
    }
  }
  
  if( iter == m_maxIter )
    Warning( "Failed to propagate to plane within tolerance." ).ignore();
  
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
                                                   ParticleID pid ) const
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
                                                   ParticleID pid ) const
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
                                        ParticleID pid ) const
  
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
                                        ParticleID pid ) const

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
                                      ParticleID pid ) const
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
                                      ParticleID pid ) const
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
                                 ParticleID pid ) const
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
                                  ParticleID pid ) const
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
                                        ParticleID pid ) const
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
                                        ParticleID pid ) const
{
  State tmpState;

  StatusCode sc = propagate( track, z, tmpState, pid );

  if ( sc.isSuccess() ) mom = tmpState.momentum();

  return sc;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackExtrapolator::TrackExtrapolator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackExtrapolator>( this );

  declareProperty( "Iterations", m_maxIter = 5 );
}

//=============================================================================
// Destructor
//=============================================================================
TrackExtrapolator::~TrackExtrapolator() {}

//=============================================================================
