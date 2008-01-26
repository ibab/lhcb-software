// $Id: PiecewiseTrajectory.h,v 1.1 2008-01-26 13:31:19 mneedham Exp $
#ifndef LHCbKernel_PiecewiseTrajectory_H
#define LHCbKernel_PiecewiseTrajectory_H 1

// Includes
#include <utility>
#include <deque>
#include "Kernel/Trajectory.h"

// from Gaudi

namespace LHCb 
{

  /** @class PiecewiseTrajectory PiecewiseTrajectory.h
   *
   *
   * @author Gerhard Raven
   * @date   20/04/2007
   * 
   */
  class PiecewiseTrajectory : public Trajectory {
  public:
    template <typename ITER>
    PiecewiseTrajectory(ITER begin,ITER end) : Trajectory(0,0) {
        while (begin!=end) { append(*begin); ++begin; }
    }

    PiecewiseTrajectory() :Trajectory(0,0){}

    ~PiecewiseTrajectory();

    PiecewiseTrajectory(const PiecewiseTrajectory& lhs);

    // clone thyself...
    virtual std::auto_ptr<Trajectory> clone() const;

    /// Point on the trajectory at mu
    Point position( double mu ) const;

    /// First derivative of the trajectory at mu
    Vector direction( double mu ) const;

    /// Second derivative of the trajectory at mu
    Vector curvature( double mu ) const;
    /// Create a parabolic approximation to the trajectory
    /// at mu 
    void expansion( double arclength,
                            Point& p,
                            Vector& dp,
                            Vector& ddp ) const;
    /// Determine the distance in arclenghts to the
    /// closest point on the trajectory to a given point
    double muEstimate( const Point& ) const;

    /// Number of arclengths until deviation of the trajectory from the expansion
    /// reaches the given tolerance.
    double distTo1stError( double arclength,
                           double tolerance, 
                           int pathDirection = +1 ) const ;
    
    /// Number of arclengths until deviation of the trajectory from the expansion
    /// reaches the given tolerance.
    double distTo2ndError( double arclength,
                           double tolerance, 
                           int pathDirection = +1 ) const;
     
    // note: we _will_ assume ownership of the passed Trajectory!
    void append(Trajectory*); 
    // void prepend(Trajectory*); 
    
    /// Distance, along the Trajectory, between position(mu1) and
    /// position(mu2). .
    virtual double arclength(double mu1, double mu2) const { return mu2 - mu1 ; }

  private:
     typedef std::pair<Trajectory*,double> Elem;
     typedef std::deque<Elem>         Trajs;
     Trajs                            m_traj; // note: (we assume that) we OWN the Trajectory*!
     mutable Trajs::iterator          m_last;      // (cached) starting point for searches
                                                        // speeds up access when staying in a small
                                                        // local area...
     // generic forwarding to local trajectories
     template <typename RET, typename FUNC> RET local(double arclen, FUNC f) const;
  };
 
}
#endif
