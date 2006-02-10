// $Id: CircleTraj.h,v 1.3 2006-02-10 12:29:04 graven Exp $
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
                double radius,
                const Range& angularRange );
    
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
    
    /// Retrieve the derivative of the parabolic approximation to the trajectory
    /// with respect to the state parameters
    virtual ROOT::Math::SMatrix<double,3,kSize> derivative( double arclength ) const;
    
    /// Determine the distance in arclenghts to the
    /// closest point on the trajectory to a given point
    virtual double arclength( const Gaudi::XYZPoint& point ) const;
    
    /// Number of arclengths until deviation of the trajectory from the expansion
    /// reaches the given tolerance.
    virtual double distTo1stError( double arclength,
                                   double tolerance, 
                                   int pathDirection = +1 ) const;
    
    /// Number of arclengths until deviation of the trajectory from the expansion
    /// reaches the given tolerance.
    virtual double distTo2ndError( double arclength,
                                   double tolerance, 
                                   int pathDirection = +1 ) const;
    
    /// Range in arclength w.r.t. the starting point
    /// over which the trajectory is valid
    virtual Range range() const;
    
    /// Length of trajectory
    virtual double length() const;
    
  private:
    double phi(double arclen) const { return m_angularRange.first + arclen/m_radius; }
    Gaudi::XYZPoint m_origin;
    double m_radius;  
    Range m_angularRange;
    Range m_range;
    
  }; // class CircleTraj
  
} // namespace LHCb

#endif /// LHCbKernel_CircleTraj_H
