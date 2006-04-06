// $Id: CircleTraj.h,v 1.6 2006-04-06 14:06:09 ebos Exp $
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

namespace LHCb
{

  class CircleTraj : public DifTraj<3> {
    
  public:

    /// Enum providing number of colums in derivative matrix
    enum { kSize = 3 };

    /// Default Destructor
    virtual ~CircleTraj() {};

    // clone thyself...
    virtual std::auto_ptr<Trajectory> clone() const;

    /// Constructor from a center, the normal which defines the plane
    /// of the circle, a vector from the center to a point on the circle,
    /// and the range of the circle (radius*phirange, phi=0 corresponds to
    /// the above point on the circle, phi is the angle which is used to
    /// rotate this point around the normal)
    /// Only the component of origin2point perpendicular to normal
    /// is considered!!!...
    CircleTraj( const Gaudi::XYZPoint& origin,// center of circle
                const Gaudi::XYZVector& normal, // direction of end
                const Gaudi::XYZVector& origin2point, // direction of start
                const Trajectory::Range& range); // valid range, in radius*deltaphi
    
    /// Constructor from a center, the directions of the
    /// start and end of the traj wrt. the center, and the radius.
    /// The Traj goes along the 'short' arc from origin+radius*dir1.unit()
    /// at arclength=0 to origin+radius*dir2.unit() at the arclen=length().
    CircleTraj( const Gaudi::XYZPoint& origin,// center of circle
                const Gaudi::XYZVector& dir1, // direction of start
                const Gaudi::XYZVector& dir2, // direction of end
                double radius);
    
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
    
    /// Retrieve the derivative of the point at the fixed arclength 'arclength'
    /// with respect to the parameters
    virtual Derivative derivative( double arclength ) const;
    
    /// Return arclen at which the trajectory is
    /// closest to the specified point
    virtual double arclength( const Gaudi::XYZPoint& point ) const;
    
    /// distance along the trajectory until deviation from the
    /// 1st order expansion reaches the given tolerance.
    virtual double distTo1stError( double arclength,
                                   double tolerance, 
                                   int pathDirection = +1 ) const;
    
    /// distance along trajectory until deviation from the 
    /// 2nd order expansion reaches the given tolerance.
    virtual double distTo2ndError( double arclength,
                                   double tolerance, 
                                   int pathDirection = +1 ) const;
    
  private :

    Gaudi::XYZPoint  m_origin;
    Gaudi::XYZVector m_normal;
    Gaudi::XYZVector m_dirStart;
    double m_radius;  

  }; // class CircleTraj
  
} // namespace LHCb

#endif /// LHCbKernel_CircleTraj_H
