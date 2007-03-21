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
  virtual StatusCode propagate( const LHCb::Track& track,
                                double z,
                                LHCb::State& state,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( LHCb::State& state,
                                double z,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Propagate a state to the closest point to the specified point
  virtual StatusCode propagate( LHCb::State& state,
                                const Gaudi::XYZPoint& point,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Propagate a state to within tolerance of a plane (default = 10 microns)
  virtual StatusCode propagate( LHCb::State& state,
                                Gaudi::Plane3D& plane,
                                double tolerance = 0.01,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) );
  
  //--------- ACCESS METHOD ---------------------------------------

  /** Retrieve the position and momentum vectors and the corresponding
      6D covariance matrix (pos:1->3,mom:4-6) of a track at a given z-position
      @return status code
      @param[out] pos     the position of the track
      @param[out] mom     the momentum of the track
      @param[out] cov6D   the full position-momentum covariance matrix
      @param[in]  track   the input track
      @param[in]  z       the z-position at which to extrapolate
      @param[in]  pid     pid of the particle to extrapolate (default is pion)
  */
  virtual StatusCode positionAndMomentum( const LHCb::Track& track,
                                          double z,
                                          Gaudi::XYZPoint& pos,
                                          Gaudi::XYZVector& mom,
                                          Gaudi::SymMatrix6x6& cov6D,
                                          LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the position and momentum vectors of a track at a given z-position
  virtual StatusCode positionAndMomentum( const LHCb::Track& track,
                                          double z,
                                          Gaudi::XYZPoint& pos,
                                          Gaudi::XYZVector& mom,
                                          LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the 3D-position vector and error matrix of a track at a given z-position
  virtual StatusCode position( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZPoint& pos,
                               Gaudi::SymMatrix3x3& errPos,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the 3D-position vector of a track at a given z-position
  virtual StatusCode position( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZPoint& pos,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix of a track at a given z-position
  virtual StatusCode slopes( const LHCb::Track& track,
                             double z,
                             Gaudi::XYZVector& slopes,
                             Gaudi::SymMatrix3x3& errSlopes,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the slopes (dx/dz,dy/dz,1) of a track at a given z-position
  virtual StatusCode slopes( const LHCb::Track& track,
                             double z,
                             Gaudi::XYZVector& slopes,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the momentum of a track at a given z-position
  virtual StatusCode p( const LHCb::Track& track,
                        double z,
                        double& p,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the transverse momentum of a track at a given z-position
  virtual StatusCode pt( const LHCb::Track& track,
                         double z,
                         double& pt,
                         LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the momentum vector and error matrix of a track at a given z-position
  virtual StatusCode momentum( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZVector& mom,
                               Gaudi::SymMatrix3x3& errMom,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// Retrieve the momentum vector of a track at a given z-position
  virtual StatusCode momentum( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZVector& mom,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) );

  /// retrieve transport matrix
  virtual const Gaudi::TrackMatrix& transportMatrix() const;

  /// Standard constructor
  TrackExtrapolator( const std::string& type, 
		     const std::string& name,
		     const IInterface* parent );
  
  /// Destructor
  virtual ~TrackExtrapolator( );

protected:
  
  /// Transport matrix
  Gaudi::TrackMatrix m_F; 
  
private :
  
  /// Maximum number of steps in propagation to a plane
  int m_maxIter;
  
};
#endif // TRACKEXTRAPOLATORS_TRACKEXTRAPOLATOR_H
