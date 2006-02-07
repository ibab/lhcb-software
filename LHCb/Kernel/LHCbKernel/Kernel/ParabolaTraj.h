// $Id: ParabolaTraj.h,v 1.5 2006-02-07 11:07:10 erodrigu Exp $
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
    
    /// Default Constructor
    ParabolaTraj() {};
    
    /// Default Destructor
    virtual ~ParabolaTraj() {};
    
    /// Constructor from a (middle) point, a (unit) direction vector,
    /// a curvature vector and the pair of begin- and endpoints.
    ParabolaTraj( const Gaudi::XYZPoint& middle,
                  const Gaudi::XYZVector& dir,
                  const Gaudi::XYZVector& curv,
                  const std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> range );
    
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
    
    /// Retrieve the derivative of the parabolic approximation to the
    /// trajectory with respect to the state parameters
    virtual ROOT::Math::SMatrix<double,3,kSize> derivative( const double& arclength ) const;
    
    /// Determine the distance in arclenghts to the
    /// closest point on the trajectory to a given point
    virtual double distanceToPoint( const Gaudi::XYZPoint& point ) const;
    
    /// Number of arclengths until deviation of the trajectory from the
    /// expansion reaches the given tolerance.
    virtual double distTo1stError( double& arclength,
                                   const double& tolerance, 
                                   int pathDirection = +1 ) const;
    
    /// Number of arclengths until deviation of the trajectory from the
    /// expansion reaches the given tolerance.
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
    Gaudi::XYZVector m_curv;
    std::pair<double,double> m_range;
    
  }; // class ParabolaTraj
  
} // namespace LHCb

#endif /// LHCbKernel_ParabolaTraj_H
