// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackExtrapolator.h"
#include "Event/TrackParameters.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TrackExtrapolator );

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
  StatusCode sc = StatusCode::FAILURE;

  warning() << " can not propagate state at z" << state.z() << endreq
            << " to the z position " << z << endreq
            << " of pid " << pid.pid() << endreq;

  return sc;
}

//=============================================================================
// Propagate a state to the closest point to the specified point
//=============================================================================
StatusCode TrackExtrapolator::propagate( State& state,
                                         const XYZPoint& point,
                                         ParticleID  pid )
{
  StatusCode sc = StatusCode::FAILURE;
  warning() << " can not propagate state at " << state.z() << endreq
            << " to point at z " << point.z() << endreq
            << " of pid " << pid.pid() << endreq;
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
                                                   XYZPoint& pos,
                                                   XYZVector& mom,
                                                   SymMatrix6x6& cov6D,
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
                                                   XYZPoint& pos,
                                                   XYZVector& mom,
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
                                        XYZPoint& pos,
                                        SymMatrix3x3& errPos,
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
                                        XYZPoint& pos,
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
                                      XYZVector& slopes,
                                      SymMatrix3x3& errSlopes,
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
                                      XYZVector& slopes,
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
                                        XYZVector& mom,
                                        SymMatrix3x3& errMom,
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
                                        XYZVector& mom,
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

  // create transport matrix
  m_F = TrackMatrix( ROOT::Math::SMatrixIdentity() );
}

//=============================================================================
// Destructor
//=============================================================================
TrackExtrapolator::~TrackExtrapolator() {}; 

//=============================================================================
