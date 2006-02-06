// $Id: LineTraj.h,v 1.4 2006-02-06 11:12:03 ebos Exp $
#ifndef LHCbKernel_LineTraj_H
#define LHCbKernel_LineTraj_H 1

// Include files
#include "Kernel/DifTraj.h"

/** @class LineTraj LineTraj.h
 *
 * Trajectory parameterized as a straight line
 *
 * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 * @date   17/01/2005
 * 
 */

class LineTraj: public DifTraj<6> {
    
public:

  /// Enum providing number of colums in derivative matrix
  enum { kSize = 6 };

  /// Default Constructor
  LineTraj() {};

  /// Default Destructor
  virtual ~LineTraj() {};
    
  /// Constructor from the middle point and a (unit) direction vector
  LineTraj( const Gaudi::XYZPoint& middle,
            const Gaudi::XYZVector& dir,
            const std::pair<double,double> range );
  
  /// Constructor from a begin and an end point
  LineTraj( const Gaudi::XYZPoint& begPoint,
            const Gaudi::XYZPoint& endPoint );

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

  Gaudi::XYZPoint  m_pos;
  Gaudi::XYZVector m_dir;
  std::pair<double,double> m_range;

}; // class LineTraj

#endif /// LHCbKernel_LineTraj_H
