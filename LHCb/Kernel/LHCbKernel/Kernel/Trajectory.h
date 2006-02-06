// $Id: Trajectory.h,v 1.4 2006-02-06 11:11:35 ebos Exp $
#ifndef LHCbKernel_Trajectory_H
#define LHCbKernel_Trajectory_H 1

// Includes

// from Kernel/LHCbDefinitions
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/GenericMatrixTypes.h"
#include "Kernel/PhysicalConstants.h"

/** @class Trajectory Trajectory.h
 *
 * This is the base class for the trajectory classes. 
 *
 * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 * @date   01/12/2005
 * 
 */

class Trajectory {
    
public:
    
  /// Point on the trajectory at arclength from the starting point
  virtual Gaudi::XYZPoint position( const double& arclength ) const = 0;

  /// First derivative of the trajectory at arclength from the starting point
  virtual Gaudi::XYZVector direction( const double& arclength ) const = 0;

  /// Second derivative of the trajectory at arclength from the starting point
  virtual Gaudi::XYZVector curvature( const double& arclength ) const = 0;
    
  /// Create a parabolic approximation to the trajectory
  /// at arclength from the starting point
  virtual void expansion( const double& arclength,
                          Gaudi::XYZPoint& p,
                          Gaudi::XYZVector& dp,
                          Gaudi::XYZVector& ddp ) const = 0;
  
  /// Determine the distance in arclenghts to the
  /// closest point on the trajectory to a given point
  virtual double distanceToPoint( const Gaudi::XYZPoint& ) const
  { return 0.0; } // dummy implementation

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance.
  virtual double distTo1stError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const = 0;

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance.
  virtual double distTo2ndError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const = 0;

  /// Range in arclength w.r.t. the starting point
  /// over which the trajectory is valid
  virtual std::pair<double,double> range() const = 0;

  /// Maximal valid value of arclength according to range()
  double endRange() const { return range().second; }

  /// Minimal valid value of arclength according to range()
  double beginRange() const { return range().first; }

  /// Restrict arclength to the valid range of the trajectory
  double restrictToRange( double& arclength ) const 
  {
    std::pair<double,double> r = range(); 
    return std::max( r.first, std::min(r.second, arclength) );
  }

  /// Length of trajectory
  virtual double length() const = 0;

}; // class Trajectory

#endif /// LHCbKernel_Trajectory_H
