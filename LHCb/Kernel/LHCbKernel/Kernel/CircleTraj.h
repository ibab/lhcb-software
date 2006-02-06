// $Id: CircleTraj.h,v 1.1 2006-02-06 11:12:29 ebos Exp $
#ifndef LHCbKernel_CircleTraj_H
#define LHCbKernel_CircleTraj_H 1

// Include files
#include "Kernel/DifTraj.h"

/** @class CircleTraj CircleTraj.h
 *
 * Trajectory parameterized as a circle
 *
 * @author Edwin Bos, Eduardo Rodrigues
 * @date   30/01/2006
 * 
 */

class CircleTraj: public DifTraj<3> {
    
public:

  /// Enum providing number of colums in derivative matrix
  enum { kSize = 3 };

  /// Default Constructor
  CircleTraj() {};

  /// Default Destructor
  virtual ~CircleTraj() {};
    
  /// Constructor from an origin, a radius
  /// and a range in angle w.r.t. angle of origin point
  CircleTraj( const Gaudi::XYZPoint& origin,
              const double radius,
              const std::pair<double,double> angularRange );
    
  /// Point on the trajectory at arclength from the starting point    
  virtual Gaudi::XYZPoint position( const double& arclength ) const;
    
  /// First derivative of the trajectory at arclength from the starting point
  virtual Gaudi::XYZVector direction( const double& arclength ) const;
    
  /// Second derivative of the trajectory at arclength from the starting point
  virtual Gaudi::XYZVector curvature( const double& arclength ) const;

  /// Create a parabolic approximation to the trajectory
  /// at arclength from the starting point
  virtual void expansion( const double& arclength,
                          Gaudi::XYZPoint& p,
                          Gaudi::XYZVector& dp,
                          Gaudi::XYZVector& ddp ) const;
    
  /// Retrieve the derivative of the parabolic approximation to the trajectory
  /// with respect to the state parameters
  virtual ROOT::Math::SMatrix<double,3,kSize> derivative( const double& arclength ) const;

  /// Determine the distance in arclenghts to the
  /// closest point on the trajectory to a given point
  virtual double distanceToPoint( const Gaudi::XYZPoint& point ) const;

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance.
  virtual double distTo1stError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const;

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance.
  virtual double distTo2ndError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const;

  /// Range in arclength w.r.t. the starting point
  /// over which the trajectory is valid
  virtual std::pair<double,double> range() const;

  /// Length of trajectory
  virtual double length() const;

private:

  Gaudi::XYZPoint m_origin;
  double m_radius;  
  std::pair<double,double> m_angularRange;
  std::pair<double,double> m_range;

}; // class CircleTraj

#endif /// LHCbKernel_CircleTraj_H
