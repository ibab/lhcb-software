#ifndef TRACKINTERFACES_ITRACKEXTRAPOLATOR_H
#define TRACKINTERFACES_ITRACKEXTRAPOLATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbKernel
#include "Kernel/ParticleID.h"

// from LHCbDefinitions
#include "Kernel/SymmetricMatrixTypes.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Plane3DTypes.h"

// Forward declarations
namespace LHCb {
 class Track;
 class State;
};

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

class ITrackExtrapolator: virtual public IAlgTool 
{

public:

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackExtrapolator; }

  /// Propagate a track to a given z-position
  virtual StatusCode propagate( const LHCb::Track& track,
                                double z,
                                LHCb::State& state,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( LHCb::State& state,
                                double z,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a track to the intersection point with a given plane
  virtual StatusCode propagate( const LHCb::Track& track,
                                const Gaudi::Plane3D& plane,
                                LHCb::State& state,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a state to the intersection point with a given plane
  virtual StatusCode propagate( LHCb::State& state,
                                const Gaudi::Plane3D& plane,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;
  
  /// Predict where the plane will be intersected
  virtual StatusCode predict( const LHCb::State& state,
                              const Gaudi::Plane3D& plane,
                              double& dZ ) = 0;

  /// Propagate a state to the closest point to the specified point
  virtual StatusCode propagate( LHCb::State& state,
                                const Gaudi::XYZPoint& point,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  //--------- ACCESS METHOD ---------------------------------------

  /// Retrieve the position and momentum vectors and the corresponding
  /// 6D covariance matrix (pos:1->3,mom:4-6) of a track at a given z-position
  virtual StatusCode positionAndMomentum( const LHCb::Track& track,
                                          double z,
                                          Gaudi::XYZPoint& pos,
                                          Gaudi::XYZVector& mom,
                                          Gaudi::SymMatrix6x6& cov6D,
                                          LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the position and momentum vectors and the corresponding
  /// 6D covariance matrix (pos:1->3,mom:4-6) at the intersection of a track with a given plane
  virtual StatusCode positionAndMomentum( const LHCb::Track& track,
                                          const Gaudi::Plane3D& plane,
                                          Gaudi::XYZPoint& pos,
                                          Gaudi::XYZVector& mom,
                                          Gaudi::SymMatrix6x6& cov6D,
                                          LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the position and momentum vectors of a track at a given z-position
  virtual StatusCode positionAndMomentum( const LHCb::Track& track,
                                          double z,
                                          Gaudi::XYZPoint& pos,
                                          Gaudi::XYZVector& mom,
                                          LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the position and momentum vectors at the intersection of a track with a given plane
  virtual StatusCode positionAndMomentum( const LHCb::Track& track,
                                          const Gaudi::Plane3D& plane,
                                          Gaudi::XYZPoint& pos,
                                          Gaudi::XYZVector& mom,
                                          LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector and error matrix of a track at a given z-position
  virtual StatusCode position( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZPoint& pos,
                               Gaudi::SymMatrix3x3& errPos,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector and error matrix at the intersection of a track with a given plane
  virtual StatusCode position( const LHCb::Track& track,
                               const Gaudi::Plane3D& plane,
                               Gaudi::XYZPoint& pos,
                               Gaudi::SymMatrix3x3& errPos,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector of a track at a given z-position
  virtual StatusCode position( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZPoint& pos,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector at the intersection of a track with a given plane
  virtual StatusCode position( const LHCb::Track& track,
                               const Gaudi::Plane3D& plane,
                               Gaudi::XYZPoint& pos,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix of a track at a given z-position
  virtual StatusCode slopes( const LHCb::Track& track,
                             double z,
                             Gaudi::XYZVector& slopes,
                             Gaudi::SymMatrix3x3& errSlopes,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix at the intersection of a track with a given plane
  virtual StatusCode slopes( const LHCb::Track& track,
                             const Gaudi::Plane3D& plane,
                             Gaudi::XYZVector& slopes,
                             Gaudi::SymMatrix3x3& errSlopes,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) of a track at a given z-position
  virtual StatusCode slopes( const LHCb::Track& track,
                             double z,
                             Gaudi::XYZVector& slopes,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) at the intersection of a track with a given plane
  virtual StatusCode slopes( const LHCb::Track& track,
                             const Gaudi::Plane3D& plane,
                             Gaudi::XYZVector& slopes,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum of a track at a given z-position
  virtual StatusCode p( const LHCb::Track& track,
                        double z,
                        double& p,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum at the intersection of a track with a given plane
  virtual StatusCode p( const LHCb::Track& track,
                        const Gaudi::Plane3D& plane,
                        double& p,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the transverse momentum of a track at a given z-position
  virtual StatusCode pt( const LHCb::Track& track,
                         double z,
                         double& pt,
                         LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the transverse momentum at the intersection of a track with a given plane
  virtual StatusCode pt( const LHCb::Track& track,
                         const Gaudi::Plane3D& plane,
                         double& pt,
                         LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum vector and error matrix of a track at a given z-position
  virtual StatusCode momentum( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZVector& mom,
                               Gaudi::SymMatrix3x3& errMom,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum vector and error matrix at the intersection of a track with a given plane
  virtual StatusCode momentum( const LHCb::Track& track,
                               const Gaudi::Plane3D& plane,
                               Gaudi::XYZVector& mom,
                               Gaudi::SymMatrix3x3& errMom,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum vector of a track at a given z-position
  virtual StatusCode momentum( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZVector& mom,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum vector at the intersection of a track with a given plane
  virtual StatusCode momentum( const LHCb::Track& track,
                               const Gaudi::Plane3D& plane,
                               Gaudi::XYZVector& mom,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Getters & setters
  virtual const Gaudi::TransportMatrix& transportMatrix() const = 0;

};

//==============================================================================
//   end of class
//==============================================================================

#endif // TRACKINTERFACES_ITRACKEXTRAPOLATOR_H
