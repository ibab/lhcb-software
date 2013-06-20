// $Id: IMaterialLocator.h,v 1.2 2007-12-06 14:50:17 wouter Exp $
#ifndef TRACKINTERFACES_IMATERIALLOCATOR_H 
#define TRACKINTERFACES_IMATERIALLOCATOR_H

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbKernel
#include "Kernel/ParticleID.h"
#include "Event/TrackTypes.h"

// others
#include "DetDesc/ILVolume.h"

static const InterfaceID IID_IMaterialLocator ( "IMaterialLocator", 1, 0 );

// forwarded
namespace LHCb {
  class State ;
  class StateVector ;
  class ZTrajectory ;
}

/** @class IMaterialLocatorLocator
 *  
 *  Interface for tools that locate materil intersections on a trajectory
 *
 *  @author Wouter Hulsbergen
 *  @date   2006-05-16
 */

class IMaterialLocator : virtual public IAlgTool 
{
public: 
  
  /// embedded class representing intersection
  struct Intersection {
    double z1 ;
    double z2 ;
    double tx ;
    double ty ;
    const Material* material ;
  } ;
  
  /// container of intersections
  typedef std::vector<Intersection> Intersections ;
    
  /// Intersect a line with volumes in the geometry
  virtual size_t intersect( const Gaudi::XYZPoint& p, const Gaudi::XYZVector& v, 
			    ILVolume::Intersections& intersepts ) const =  0 ;
  
  /// Intersect a line with volumes in the geometry
  virtual size_t intersect( const Gaudi::XYZPoint& p, const Gaudi::XYZVector& v, 
			    Intersections& intersepts) const = 0  ;
  
  /// Intersect a trajectory with volumes in the geometry
  virtual size_t intersect( const LHCb::ZTrajectory& traj, Intersections& intersepts ) const = 0;

  /// Intersect a trajectory interpolated between two statevectors with volumes in the geometry
  virtual size_t intersect( const LHCb::StateVector& origin, const LHCb::StateVector& target,
			    Intersections& intersepts ) const = 0 ;

  /// Apply material corrections using material in intersepts
  virtual void applyMaterialCorrections( LHCb::State& stateAtTarget,
					 const Intersections& intersepts,
					 double zorigin,
					 LHCb::ParticleID pid,
					 bool applyScatteringCorrection = true,
					 bool applyELossCorrection = true ) const = 0 ;
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMaterialLocator ; }
  
} ;

#endif
