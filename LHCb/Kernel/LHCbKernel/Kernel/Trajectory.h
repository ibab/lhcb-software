#ifndef LHCbKernel_Trajectory_H
#define LHCbKernel_Trajectory_H 1

// Includes

// from Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"

#include <utility>
#include <memory>

namespace LHCb
{

  /** @class Trajectory Trajectory.h
   *
   * This is the base class for the trajectory classes. 'mu' is the expansion
   * parameter. For the existing trajectories it is either the arclength or z.
   *
   * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
   * @date   01/12/2005
   *
   */

  class Trajectory {

  public:
    typedef std::pair<double,double> Range;
    typedef Gaudi::XYZPoint          Point;
    typedef Gaudi::XYZVector         Vector;

    /// Clone a trajectory...
    virtual std::unique_ptr<Trajectory> clone() const = 0;

    /// Constructor taking the values of mu that defined the valid range of the trajectory
    Trajectory( double begin, double end ): m_range(Range(begin,end)) {}

    /// constructer taking a range
    Trajectory( const Range& range ): m_range(range) {}

    /// destructor
    virtual ~Trajectory() {}

    /// Point on the trajectory at position mu from the starting point
    virtual Point position( double mu ) const = 0;

    /// Beginpoint of the Trajectory
    Point beginPoint() const { return position( beginRange() ); }

    /// Endpoint of the Trajectory
    Point endPoint() const { return position( endRange() ); }

    /// First derivative of the position to mu. Note: not normalized!
    virtual Vector direction( double mu ) const = 0;

    /// Second derivative of the position to mu. Note: not normalized!
    virtual Vector curvature( double mu ) const = 0;

    /// Create a parabolic approximation to the trajectory at position mu
    virtual void expansion( double mu, Point& p, Vector& dp, Vector& ddp ) const = 0;

    /// Estimate the value mu that corresponds to the closest point on the
    /// trajectory to a given point. The position corresponding to mu may differ
    /// from the exact point of closest approach for trajectories with non-zero
    /// curvature.
    virtual double muEstimate( const Point& ) const = 0;

    /// Distance in mu until the deviation from the linear approximation differs
    /// from this trajectory by a given tolerance.
    virtual double distTo1stError( double mu, double tolerance, int pathDirection = +1 ) const = 0;

    /// Distance in mu until the deviation from the quadratic approximation differs
    /// from this trajectory by a given tolerance.
    virtual double distTo2ndError( double mu, double tolerance, int pathDirection = +1 ) const = 0;

    /// Range of expansion parameter over which the trajectory is defined
    Range range() const { return m_range; }

    /// Maximum value of mu for which the trajectory is defined
    double endRange() const { return range().second; }

    /// Minimum value of mu for which the trajectory is defined
    double beginRange() const { return range().first; }

    /// Distance, along the Trajectory, between Trajectory::position(mu1) and
    /// Trajectory::position(mu2)
    virtual double arclength(double mu1, double mu2) const = 0 ;

    // Arclength of valid range
    virtual double arclength() const { return arclength(beginRange(),endRange()) ; }

    /// Set the range
    void setRange( double begin, double end ) {
      m_range.first  = begin ;
      m_range.second = end ;
    }

    /// obsolete, must be removed/renamed in inherited classes/clients
    double length() const { return arclength() ; };
    /// obsolete, must be removed/renamed in inherited classes/clients
    double arclength(const Point& point) const { return muEstimate(point) ; }

  private:

    Range m_range;

  }; // class Trajectory

} // namespace LHCb

#endif /// LHCbKernel_Trajectory_H
