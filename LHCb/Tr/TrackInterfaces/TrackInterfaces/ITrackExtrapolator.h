// $Id: ITrackExtrapolator.h,v 1.18 2009-09-24 14:46:14 wouter Exp $
#ifndef TRACKINTERFACES_ITRACKEXTRAPOLATOR_H
#define TRACKINTERFACES_ITRACKEXTRAPOLATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geometry Definitions
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

// from LHCbKernel
#include "Kernel/ParticleID.h"

// from TrackEvent
#include "Event/TrackTypes.h"

// Forward declarations
namespace LHCb {
 class Track;
 class State;
 class StateVector ;
};

static const InterfaceID IID_ITrackExtrapolator( "ITrackExtrapolator" , 3, 0 );

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

  /// Propagate a state vector from zOld to zNew
  virtual StatusCode propagate( Gaudi::TrackVector& stateVec,
                                double zOld,
                                double zNew,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a state vector from zOld to zNew
  /// Transport matrix is calulated when transMat pointer is not NULL
  virtual StatusCode propagate( Gaudi::TrackVector& stateVec,
                                double zOld,
                                double zNew,
                                Gaudi::TrackMatrix* transMat,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a statevector
  virtual StatusCode propagate( LHCb::StateVector& state,
				double z,
				Gaudi::TrackMatrix* transportmatrix=0,
				LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a track to a given z-position
  virtual StatusCode propagate( const LHCb::Track& track,
                                double z,
                                LHCb::State& state,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( LHCb::State& state,
                                double z,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a state to a given z-position
  /// Transport matrix is calulated when transMat pointer is not NULL
  virtual StatusCode propagate( LHCb::State& state,
                                double z,
                                Gaudi::TrackMatrix* transMat,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a track to the closest point to the specified point
  virtual StatusCode propagate( const LHCb::Track& track,
                                const Gaudi::XYZPoint& point,
                                LHCb::State& state,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a state to the closest point to the specified point
  virtual StatusCode propagate( LHCb::State& state,
                                const Gaudi::XYZPoint& point,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;
  
  /// Propagate a track to within tolerance of a plane (default = 10 microns)
  virtual StatusCode propagate( const LHCb::Track& track,
                                const Gaudi::Plane3D& plane,
                                LHCb::State& state,
                                double tolerance = 0.01,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Propagate a state to within tolerance of a plane (default = 10 microns)
  virtual StatusCode propagate( LHCb::State& state,
                                const Gaudi::Plane3D& plane,
                                double tolerance = 0.01,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  
  //--------- ACCESS METHODS ---------------------------------------

  /// Retrieve the position and momentum vectors and the corresponding
  /// 6D covariance matrix (pos:1->3,mom:4-6) of a track at a given z-position
  virtual StatusCode positionAndMomentum( const LHCb::Track& track,
                                          double z,
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

  /// Retrieve the 3D-position vector and error matrix of a track at a given z-position
  virtual StatusCode position( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZPoint& pos,
                               Gaudi::SymMatrix3x3& errPos,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the 3D-position vector of a track at a given z-position
  virtual StatusCode position( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZPoint& pos,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) and error matrix of a track at a given z-position
  virtual StatusCode slopes( const LHCb::Track& track,
                             double z,
                             Gaudi::XYZVector& slopes,
                             Gaudi::SymMatrix3x3& errSlopes,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the slopes (dx/dz,dy/dz,1) of a track at a given z-position
  virtual StatusCode slopes( const LHCb::Track& track,
                             double z,
                             Gaudi::XYZVector& slopes,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum of a track at a given z-position
  virtual StatusCode p( const LHCb::Track& track,
                        double z,
                        double& p,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the transverse momentum of a track at a given z-position
  virtual StatusCode pt( const LHCb::Track& track,
                         double z,
                         double& pt,
                         LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum vector and error matrix of a track at a given z-position
  virtual StatusCode momentum( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZVector& mom,
                               Gaudi::SymMatrix3x3& errMom,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

  /// Retrieve the momentum vector of a track at a given z-position
  virtual StatusCode momentum( const LHCb::Track& track,
                               double z,
                               Gaudi::XYZVector& mom,
                               LHCb::ParticleID pid = LHCb::ParticleID(211) ) = 0;

};
#endif // TRACKINTERFACES_ITRACKEXTRAPOLATOR_H
