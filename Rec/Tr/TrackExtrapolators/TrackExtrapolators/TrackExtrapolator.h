// $Id: TrackExtrapolator.h,v 1.2 2005-03-11 10:09:22 hernando Exp $
#ifndef TRACKEXTRAPOLATORS_TREXTRAPOLATOR_H 
#define TRACKEXTRAPOLATORS_TREXTRAPOLATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Tools/ITrackExtrapolator.h"

class Track;
class State;

/** @class TrackExtrapolator TrackExtrapolator.h TrackExtrapolator/TrackExtrapolator.h
 *  
 *  A TrackExtrapolator is a base class implementing methods
 *  from the ITrackExtrapolator interface.
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

  /// Propagate a track to the intersection point with a given plane
  virtual StatusCode propagate( const Track& track,
                                const HepPlane3D& plane,
                                State& state,
                                ParticleID pid = ParticleID(211) );

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( State& state,
                                double z,
                                ParticleID pid = ParticleID(211) ) 
  { 
    warning() << " can not propagate " << pid.pid() 
              << " state " << state.position() << " at " << z << endreq;
    return StatusCode::FAILURE;
  }
  

  /// Propagate a state to the intersection point with a given plane
  virtual StatusCode propagate( State& state,
                                const HepPlane3D& plane,
                                ParticleID pid = ParticleID(211) ) 
  {
    warning() << " can not propagate " << pid.pid() 
              << "state " << state.position() << " at " << plane.normal() << endreq;
    return StatusCode::FAILURE;
  }
  

  /** Retrieve the position and momentum vectors and the corresponding
      6D covariance matrix (pos:1->3,mom:4-6) of a track at a given z-position
      @return status code
              the position of the track
              the momentum of the track
              the full position-momentum covariance matrix
      @param the input track
      @param the z-position at which to extrapolate
  */
  virtual StatusCode positionAndMomentum( const Track& track,
                                          double z,
                                          HepPoint3D& pos,
                                          HepVector3D& mom,
                                          HepSymMatrix& cov6D,
                                          ParticleID pid = ParticleID(211) );

  /// Retrieve the position and momentum vectors and the corresponding
  /// 6D covariance matrix (pos:1->3,mom:4-6) at the intersection of a track with a given plane
  virtual StatusCode positionAndMomentum( const Track& track,
                                          const HepPlane3D& plane,
                                          HepPoint3D& pos,
                                          HepVector3D& mom,
                                          HepSymMatrix& cov6D,
                                          ParticleID pid = ParticleID(211) );

  /// Retrieve the position and momentum vectors of a track at a given z-position
  virtual StatusCode positionAndMomentum( const Track& track,
                                          double z,
                                          HepPoint3D& pos,
                                          HepVector3D& mom,
                                          ParticleID pid = ParticleID(211) );

  /// Retrieve the position and momentum vectors at the intersection of a track with a given plane
  virtual StatusCode positionAndMomentum( const Track& track,
                                          const HepPlane3D& plane,
                                          HepPoint3D& pos,
                                          HepVector3D& mom,
                                          ParticleID pid = ParticleID(211) );

  /// Retrieve the 3D-position vector and error matrix of a track at a given z-position
  virtual StatusCode position( const Track& track,
                               double z,
                               HepPoint3D& pos,
                               HepSymMatrix& errPos,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the 3D-position vector and error matrix at the intersection of a track with a given plane
  virtual StatusCode position( const Track& track,
                               const HepPlane3D& plane,
                               HepPoint3D& pos,
                               HepSymMatrix& errPos,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the 3D-position vector of a track at a given z-position
  virtual StatusCode position( const Track& track,
                               double z,
                               HepPoint3D& pos,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the 3D-position vector at the intersection of a track with a given plane
  virtual StatusCode position( const Track& track,
                               const HepPlane3D& plane,
                               HepPoint3D& pos,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix of a track at a given z-position
  virtual StatusCode slopes( const Track& track,
                             double z,
                             HepVector3D& slopes,
                             HepSymMatrix& errSlopes,
                             ParticleID pid = ParticleID(211) );

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix at the intersection of a track with a given plane
  virtual StatusCode slopes( const Track& track,
                             const HepPlane3D& plane,
                             HepVector3D& slopes,
                             HepSymMatrix& errSlopes,
                             ParticleID pid = ParticleID(211) );

  /// Retrieve the slopes (dx/dz,dy/dz,1) of a track at a given z-position
  virtual StatusCode slopes( const Track& track,
                             double z,
                             HepVector3D& slopes,
                             ParticleID pid = ParticleID(211) );

  /// Retrieve the slopes (dx/dz,dy/dz,1) at the intersection of a track with a given plane
  virtual StatusCode slopes( const Track& track,
                             const HepPlane3D& plane,
                             HepVector3D& slopes,
                             ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum of a track at a given z-position
  virtual StatusCode p( const Track& track,
                        double z,
                        double& p,
                        ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum at the intersection of a track with a given plane
  virtual StatusCode p( const Track& track,
                        const HepPlane3D& plane,
                        double& p,
                        ParticleID pid = ParticleID(211) );

  /// Retrieve the transverse momentum of a track at a given z-position
  virtual StatusCode pt( const Track& track,
                         double z,
                         double& pt,
                         ParticleID pid = ParticleID(211) );

  /// Retrieve the transverse momentum at the intersection of a track with a given plane
  virtual StatusCode pt( const Track& track,
                         const HepPlane3D& plane,
                         double& pt,
                         ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum vector and error matrix of a track at a given z-position
  virtual StatusCode momentum( const Track& track,
                               double z,
                               HepVector3D& mom,
                               HepSymMatrix& errMom,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum vector and error matrix at the intersection of a track with a given plane
  virtual StatusCode momentum( const Track& track,
                               const HepPlane3D& plane,
                               HepVector3D& mom,
                               HepSymMatrix& errMom,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum vector of a track at a given z-position
  virtual StatusCode momentum( const Track& track,
                               double z,
                               HepVector3D& mom,
                               ParticleID pid = ParticleID(211) );

  /// Retrieve the momentum vector at the intersection of a track with a given plane
  virtual StatusCode momentum( const Track& track,
                               const HepPlane3D& plane,
                               HepVector3D& mom,
                               ParticleID pid = ParticleID(211) );

  /// retrieve transport matrix
  virtual const HepMatrix& transportMatrix() const;

  /// Standard constructor
  TrackExtrapolator( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent );

  /// Destructor
  virtual ~TrackExtrapolator( );

protected:
  /// Transport matrix
  HepMatrix m_F; 

  /// Update the properties of the state
  virtual void updateState( State& state, double z ) const;

private:

};
#endif // TRACKEXTRAPOLATORS_TREXTRAPOLATOR_H
