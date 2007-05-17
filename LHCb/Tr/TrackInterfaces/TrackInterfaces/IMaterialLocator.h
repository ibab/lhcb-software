// $Id: IMaterialLocator.h,v 1.1 2007-05-17 05:47:51 wouter Exp $
#ifndef TRACKINTERFACES_IMATERIALLOCATOR_H 
#define TRACKINTERFACES_IMATERIALLOCATOR_H

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// others
#include "DetDesc/ILVolume.h"

static const InterfaceID IID_IMaterialLocator ( "IMaterialLocator", 1, 0 );

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
  
  /// 
  /// virtual StatusCode process( const Trajectory& traj, TrackSymMatrix* noise, TrackVector* delta, bool upstream) const = 0 ;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMaterialLocator ; }
  
} ;

#endif
