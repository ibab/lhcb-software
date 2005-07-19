#ifndef TRACKINTERFACES_ITRACKEXTRAPOLATOR_H
#define TRACKINTERFACES_ITRACKEXTRAPOLATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCB
#include "Kernel/ParticleID.h"

// from CLHEP
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

class Track;
class State;

static const InterfaceID IID_ITrackExtrapolator( "ITrackExtrapolator" , 2, 0 );

/** @class ITrackExtrapolator ITrackExtrapolator.h
 *
 *  Interface for track extrapolator tools
 *
 *  @author Edwin Bos (added method)
 *  @date 06/07/2005
 *  @author Eduardo Rodrigues (changes and new features for new track event model)
 *  @date   14/12/2004
 *  @author Marco Cattaneo
 *  @date   09/01/2002
 *
 *  Based on TrackExtrapolator ABS by Rutger van der Eijk, 07-04-1999
 */

class ITrackExtrapolator: virtual public IAlgTool {
public:
  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackExtrapolator; }

  /// Propagate a track to a given z-position
  virtual StatusCode propagate( const Track& track,
                                double z,
                                State& state,
                                ParticleID pid = ParticleID(211) ) = 0;

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( State& state,
                                double z,
                                ParticleID pid = ParticleID(211) ) = 0;

  /// Propagate a track to the intersection point with a given plane
  virtual StatusCode propagate( const Track& track,
                                const HepPlane3D& plane,
                                State& state,
                                ParticleID pid = ParticleID(211) ) = 0;

  /// Propagate a state to the intersection point with a given plane
  virtual StatusCode propagate( State& state,
                                const HepPlane3D& plane,
                                ParticleID pid = ParticleID(211) ) = 0;
  
  /// Predict where the plane will be intersected
  virtual StatusCode predict( const State& state,
                              const HepPlane3D& plane,
                              double& dZ ) = 0;

  /// Propagate a state to the closest point to the specified point
  virtual StatusCode propagate( State& state,
                                const HepPoint3D& point,
                                ParticleID pid = ParticleID(211) ) = 0;

  //--------- ACCESS METHOD ---------------------------------------

  /// Retrieve the position and momentum vectors and the corresponding
  /// 6D covariance matrix (pos:1->3,mom:4-6) of a track at a given z-position
  virtual StatusCode positionAndMomentum( const Track& track,
                                          double z,
                                          HepPoint3D& pos,
                                          HepVector3D& mom,
                                          HepSymMatrix& cov6D,
                                          ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the position and momentum vectors and the corresponding
  /// 6D covariance matrix (pos:1->3,mom:4-6) at the intersection of a track with a given plane
  virtual StatusCode positionAndMomentum( const Track& track,
                                          const HepPlane3D& plane,
                                          HepPoint3D& pos,
                                          HepVector3D& mom,
                                          HepSymMatrix& cov6D,
                                          ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the position and momentum vectors of a track at a given z-position
  virtual StatusCode positionAndMomentum( const Track& track,
                                          double z,
                                          HepPoint3D& pos,
                                          HepVector3D& mom,
                                          ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the position and momentum vectors at the intersection of a track with a given plane
  virtual StatusCode positionAndMomentum( const Track& track,
                                          const HepPlane3D& plane,
                                          HepPoint3D& pos,
                                          HepVector3D& mom,
                                          ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector and error matrix of a track at a given z-position
  virtual StatusCode position( const Track& track,
                               double z,
                               HepPoint3D& pos,
                               HepSymMatrix& errPos,
                               ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector and error matrix at the intersection of a track with a given plane
  virtual StatusCode position( const Track& track,
                               const HepPlane3D& plane,
                               HepPoint3D& pos,
                               HepSymMatrix& errPos,
                               ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector of a track at a given z-position
  virtual StatusCode position( const Track& track,
                               double z,
                               HepPoint3D& pos,
                               ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector at the intersection of a track with a given plane
  virtual StatusCode position( const Track& track,
                               const HepPlane3D& plane,
                               HepPoint3D& pos,
                               ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix of a track at a given z-position
  virtual StatusCode slopes( const Track& track,
                             double z,
                             HepVector3D& slopes,
                             HepSymMatrix& errSlopes,
                             ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix at the intersection of a track with a given plane
  virtual StatusCode slopes( const Track& track,
                             const HepPlane3D& plane,
                             HepVector3D& slopes,
                             HepSymMatrix& errSlopes,
                             ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) of a track at a given z-position
  virtual StatusCode slopes( const Track& track,
                             double z,
                             HepVector3D& slopes,
                             ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) at the intersection of a track with a given plane
  virtual StatusCode slopes( const Track& track,
                             const HepPlane3D& plane,
                             HepVector3D& slopes,
                             ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the momentum of a track at a given z-position
  virtual StatusCode p( const Track& track,
                        double z,
                        double& p,
                        ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the momentum at the intersection of a track with a given plane
  virtual StatusCode p( const Track& track,
                        const HepPlane3D& plane,
                        double& p,
                        ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the transverse momentum of a track at a given z-position
  virtual StatusCode pt( const Track& track,
                         double z,
                         double& pt,
                         ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the transverse momentum at the intersection of a track with a given plane
  virtual StatusCode pt( const Track& track,
                         const HepPlane3D& plane,
                         double& pt,
                         ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the momentum vector and error matrix of a track at a given z-position
  virtual StatusCode momentum( const Track& track,
                               double z,
                               HepVector3D& mom,
                               HepSymMatrix& errMom,
                               ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the momentum vector and error matrix at the intersection of a track with a given plane
  virtual StatusCode momentum( const Track& track,
                               const HepPlane3D& plane,
                               HepVector3D& mom,
                               HepSymMatrix& errMom,
                               ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the momentum vector of a track at a given z-position
  virtual StatusCode momentum( const Track& track,
                               double z,
                               HepVector3D& mom,
                               ParticleID pid = ParticleID(211) ) = 0;

  /// Retrieve the momentum vector at the intersection of a track with a given plane
  virtual StatusCode momentum( const Track& track,
                               const HepPlane3D& plane,
                               HepVector3D& mom,
                               ParticleID pid = ParticleID(211) ) = 0;

  /// Getters & setters
  virtual const HepMatrix& transportMatrix() const = 0;

};

//==============================================================================
//   end of class
//==============================================================================

#endif // TRACKINTERFACES_ITRACKEXTRAPOLATOR_H
