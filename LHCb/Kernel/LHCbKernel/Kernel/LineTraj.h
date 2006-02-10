// $Id: LineTraj.h,v 1.6 2006-02-10 12:29:04 graven Exp $
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

namespace LHCb
{

  class LineTraj: public DifTraj<6> {
    
  public:
    
    /// Enum providing number of colums in derivative matrix
    enum { kSize = 6 };
    
    /// Destructor
    virtual ~LineTraj() {};
    
    /// Constructor from the middle point and a (unit) direction vector
    LineTraj( const Gaudi::XYZPoint& middle,
              const Gaudi::XYZVector& dir,
              const Range& range );
    
    /// Constructor from a begin and an end point
    LineTraj( const Gaudi::XYZPoint& begPoint,
              const Gaudi::XYZPoint& endPoint );
    
    /// Point on the trajectory at arclength from the starting point    
    virtual Gaudi::XYZPoint position( double arclength ) const;
    
    /// First derivative of the trajectory at arclength from the starting point
    virtual Gaudi::XYZVector direction( double arclength ) const;
    
    /// Second derivative of the trajectory at arclength from the starting point
    virtual Gaudi::XYZVector curvature( double arclength ) const;
    
    /// Create a parabolic approximation to the trajectory
    /// at arclength from the starting point
    virtual void expansion( double arclength,
                            Gaudi::XYZPoint& p,
                            Gaudi::XYZVector& dp,
                            Gaudi::XYZVector& ddp ) const;
    
    /// Retrieve the derivative of the parabolic approximation to the
    /// trajectory with respect to the state parameters
    virtual ROOT::Math::SMatrix<double,3,kSize> derivative( double arclength ) const;
    
    /// Determine the distance in arclenghts to the
    /// closest point on the trajectory to a given point
    virtual double arclength( const Gaudi::XYZPoint& point ) const;
    
    /// Number of arclengths until deviation of the trajectory from the
    /// expansion reaches the given tolerance.
    virtual double distTo1stError( double arclength,
                                   double tolerance, 
                                   int pathDirection = +1 ) const;
    
    /// Number of arclengths until deviation of the trajectory from the
    /// expansion reaches the given tolerance. 
    virtual double distTo2ndError( double arclength,
                                   double tolerance, 
                                   int pathDirection = +1 ) const;
    
    /// Range in arclength w.r.t. the starting point
    /// over which the trajectory is valid
    virtual Range range() const;
    
    /// Length of trajectory
    virtual double length() const;
    
  private:
    
    Gaudi::XYZPoint  m_pos;
    Gaudi::XYZVector m_dir;
    Range            m_range;
    
  }; // class LineTraj
  
} // namespace LHCb

#endif /// LHCbKernel_LineTraj_H
