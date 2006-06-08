// $Id: LineTraj.h,v 1.12 2006-06-08 12:22:19 janos Exp $
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

  class LineTraj: public DifTraj<5> {
    
  public:
    
    /// Enum providing number of colums in derivative matrix
    enum { kSize = 5 };
    
    /// Destructor
    virtual ~LineTraj() {};

    // clone thyself...
    virtual std::auto_ptr<Trajectory> clone() const;
    
    /// Constructor from the middle point and a (unit) direction vector
    LineTraj( const Point& middle,
              const Vector& dir,
              const Range& range );
    
    /// Constructor from a begin and an end point
    LineTraj( const Point& begPoint,
              const Point& endPoint );
    
    /// Point on the trajectory at arclength from the starting point    
    virtual Point position( double arclength ) const;
    
    /// First derivative of the trajectory at arclength from the starting point
    virtual Vector direction( double arclength=0 ) const;
    
    /// Second derivative of the trajectory at arclength from the starting point
    virtual Vector curvature( double arclength=0 ) const;
    
    /// Create a parabolic approximation to the trajectory
    /// at arclength from the starting point
    virtual void expansion( double arclength,
                            Point& p,
                            Vector& dp,
                            Vector& ddp ) const;
    
    /// Retrieve the derivative of the parabolic approximation to the
    /// trajectory with respect to the state parameters
    virtual Derivative derivative( double arclength=0 ) const;
    
    /// Determine the distance in arclenghts to the
    /// closest point on the trajectory to a given point
    virtual double arclength( const Point& point ) const;
    
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
    
  private:
    
    Vector m_dir;
    Point  m_pos;
    
  }; // class LineTraj
  
} // namespace LHCb

#endif /// LHCbKernel_LineTraj_H
