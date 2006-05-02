#ifndef TRACKEXTRAPOLATORS_TRACKEXTRAPOLATOR_H 
#define TRACKEXTRAPOLATORS_TRACKEXTRAPOLATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/State.h"

using namespace Gaudi;
using namespace LHCb;

/** @class TrackExtrapolator TrackExtrapolator.h
 *  
 *  A TrackExtrapolator is a base class implementing methods
 *  from the ITrackExtrapolator interface.
 *
 *  @author Edwin Bos (added extrapolation methods)
 *  @date   05/07/2005
 *
 *  @author Eduardo Rodrigues
 *  @date   2004-12-17
 */
class TrackExtrapolator : public GaudiTool,
                          virtual public ITrackExtrapolator {
public:

  /// Propagate a track to a given z-position
  virtual StatusCode propagate( const Track& track,
                                double z,
                                State& state,
                                ParticleID pid = ParticleID(211) );

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( State& state,
                                double z,
                                ParticleID pid = ParticleID(211) );

  /// Propagate a state to the closest point to the specified point
  virtual StatusCode propagate( State& state,
                                const XYZPoint& point,
                                ParticleID pid = ParticleID(211) );

  //--------- ACCESS METHOD ---------------------------------------

  /** Retrieve the position and momentum vectors and the corresponding
      6D covariance matrix (pos:1->3,mom:4-6) of a track at a given z-position
      @return status code
      @param[out] pos     the position of the track
      @param[out] mom     the momentum of the track
      @param[out] cov6D   the full position-momentum covariance matrix
      @param[in]  track   the input track
      @param[in]  z       the z-position at which to extrapolate
  */
  virtual StatusCode positionAndMomentum( const Track& track,
                                          double z,
                                          XYZPoint& pos,
                                          XYZVector& mom,
                                          SymMatrix6x6& cov6D,
                                          ParticleID pid = ParticleID(211) );

  /// Retrieve the position and momentum vectors of a track at a given z-position
  virtual StatusCode positionAndMomentum( const Track& track,
                                          double z,
                                          XYZPoint& pos,
                                          XYZVector& mom,
                                          ParticleID pid = ParticleID(211) );

  /// Retrieve the 3D-position vector and error matrix of a track at a given z-position
  virtual StatusCode position( const Track& track,
                               double z,
                               XYZPoint& pos,
                               SymMatrix3x3& errPos,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the 3D-position vector of a track at a given z-position
  virtual StatusCode position( const Track& track,
                               double z,
                               XYZPoint& pos,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix of a track at a given z-position
  virtual StatusCode slopes( const Track& track,
                             double z,
                             XYZVector& slopes,
                             SymMatrix3x3& errSlopes,
                             ParticleID pid = ParticleID(211) );

  /// Retrieve the slopes (dx/dz,dy/dz,1) of a track at a given z-position
  virtual StatusCode slopes( const Track& track,
                             double z,
                             XYZVector& slopes,
                             ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum of a track at a given z-position
  virtual StatusCode p( const Track& track,
                        double z,
                        double& p,
                        ParticleID pid = ParticleID(211) );

  /// Retrieve the transverse momentum of a track at a given z-position
  virtual StatusCode pt( const Track& track,
                         double z,
                         double& pt,
                         ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum vector and error matrix of a track at a given z-position
  virtual StatusCode momentum( const Track& track,
                               double z,
                               XYZVector& mom,
                               SymMatrix3x3& errMom,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum vector of a track at a given z-position
  virtual StatusCode momentum( const Track& track,
                               double z,
                               XYZVector& mom,
                               ParticleID pid = ParticleID(211) );

  /// retrieve transport matrix
  virtual const TrackMatrix& transportMatrix() const;

  /// Standard constructor
  TrackExtrapolator( const std::string& type, 
		     const std::string& name,
		     const IInterface* parent );
  
  /// Destructor
  virtual ~TrackExtrapolator( );

protected:
  /// Transport matrix
  TrackMatrix m_F; 

};
#endif // TRACKEXTRAPOLATORS_TRACKEXTRAPOLATOR_H
