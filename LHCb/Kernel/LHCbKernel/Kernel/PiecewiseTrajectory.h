#ifndef LHCbKernel_PiecewiseTrajectory_H
#define LHCbKernel_PiecewiseTrajectory_H 1

// Includes
#include <utility>
#include <memory>
#include <deque>
#include <type_traits>
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

    ~PiecewiseTrajectory() = default;

    PiecewiseTrajectory(const PiecewiseTrajectory& rhs);

    // clone thyself...
    virtual std::unique_ptr<Trajectory> clone() const;

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


    /// Distance, along the Trajectory, between position(mu1) and
    /// position(mu2). .
    using Trajectory::arclength;
    virtual double arclength(double mu1, double mu2) const { return mu2 - mu1 ; }

    // functions specific to a PieceWiseTrajectory
    // note: we _will_ assume ownership of the passed Trajectory!
    void append(Trajectory*);
    void prepend(Trajectory*);
    unsigned int numberOfPieces() const { return m_traj.size(); }

    std::ostream& print(std::ostream&) const;

  private:
     typedef std::pair<std::unique_ptr<Trajectory>,double> Elem; // Traj, global mu for start for Traj
     std::deque<Elem>                 m_traj; // note: (we assume that) we OWN the Trajectory*!

     // global -> local mapping
     std::pair<const Trajectory*, double> loc(double mu) const;

     // generic forwarding to local trajectories
     template <typename FUN> auto local(double mu, FUN fun) const 
     -> typename std::result_of<FUN(const LHCb::Trajectory*,double)>::type 
     {
        auto j = loc(mu);
        return fun(j.first,j.second);
     }

     typedef double (LHCb::Trajectory::*distFun)(double,double,int) const;

     double distToError( double s, double tolerance, int pathDirection,
                         distFun fun) const;

  };

}
#endif
