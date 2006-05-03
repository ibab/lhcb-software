// $Id: ParabolaTraj.h,v 1.10 2006-05-03 15:00:47 graven Exp $
#ifndef LHCbKernel_ParabolaTraj_H
#define LHCbKernel_ParabolaTraj_H 1

// Include files
#include "Kernel/DifTraj.h"

/** @class ParabolaTraj ParabolaTraj.h
 *
 * Trajectory parameterized as a parabola
 *
 * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 * @date   09/01/2006
 * 
 */

namespace LHCb
{
  
  class ParabolaTraj: public DifTraj<9> {
    
  public:
    
    /// Enum providing number of colums in derivative matrix
    enum { kSize = 9 };
    
    /// Destructor
    virtual ~ParabolaTraj() {};
    
    // clone thyself...
    virtual std::auto_ptr<Trajectory> clone() const;
    
    /// Constructor from a (middle) point, a (unit) direction vector,
    /// a curvature vector and the pair of begin- and endpoints.
    ParabolaTraj( const Point& middle,
                  const Vector& dir,
                  const Vector& curv,
                  const Trajectory::Range& range );
    
    /// Point on the trajectory at arclength from the starting point    
    virtual Point position( double arclength ) const;
    
    /// First derivative of the trajectory at arclength from the starting point
    virtual Vector direction( double arclength ) const;
    
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
    virtual Derivative derivative( double arclength ) const;
    
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
    
    Point  m_pos;
    Vector m_dir;
    Vector m_curv;
    
  }; // class ParabolaTraj
  
} // namespace LHCb

#endif /// LHCbKernel_ParabolaTraj_H
