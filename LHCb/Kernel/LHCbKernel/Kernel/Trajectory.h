// $Id: Trajectory.h,v 1.2 2006-01-17 09:34:35 ebos Exp $
#ifndef LHCbKernel_Trajectory_H
#define LHCbKernel_Trajectory_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Matrix/Matrix.h"

/** @class Trajectory Trajectory.h
 *
 * Trajectory is the base class for the trajectory classes. 
 *
 * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 * @date   01/12/2005
 * 
 */

class Trajectory
{
    
public:
    
  /// Point on trajectory where parabolic approximation is made
  virtual HepPoint3D position( const double& arclength ) const = 0;

  /// First derivative of the trajectory at the approximation point
  virtual HepVector3D direction( const double& arclength ) const = 0;

  /// Second derivative of the trajectory at the approximation point,
  /// used as the constant value of the curvature of the parabolic approximation
  virtual HepVector3D curvature() const = 0;
    
  /// Create a parabolic approximation to the trajectory
  virtual void expansion( const double& arclength,
                          HepPoint3D& p,
                          HepVector3D& dp,
                          HepVector3D& ddp ) const = 0;
  
  /// Retrieve the derivative of the parabolic approximation to the trajectory
  /// with respect to the state parameters
  virtual const HepMatrix derivative( const double& arclength ) const = 0;

  /// Determine the arclenghts of the trajectory to a given point
  virtual double distanceToPoint( const HepPoint3D& ) const
  { return 0.0; } // dummy implementation

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance (does not account for the curvature).
  virtual double distTo1stError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const = 0;

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance (accounts for the curvature).
  virtual double distTo2ndError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const = 0;

  /// Range in arclength over which the trajectory is valid
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

  /// Length from begin till end of trajectory
  double length() const { 
    std::pair<double,double> r = range(); 
    return r.second-r.first;
  }

}; // class Trajectory

#endif /// LHCbKernel_Trajectory_H
