#ifndef TRACKKERNEL_TrackTraj_h
#define TRACKKERNEL_TrackTraj_h

#include <memory>
#include <vector>
#include <algorithm>
#include "Kernel/Trajectory.h"
#include "Event/Track.h"
#include "TrackKernel/CubicStateInterpolationTraj.h"

class IMagneticFieldSvc ;

namespace LHCb
{

  /** @class TrackTraj TrackTraj.h TrackKernel/TrackTraj.h
   *
   * Trajectory representation of a LHCb::Track
   *
   * @author Wouter Hulsbergen
   * @date   15/10/2007
   */

  class TrackTraj : public ZTrajectory
  {
  public:
    /// Container of states
    typedef std::vector<const LHCb::State*> StateContainer ;

    /// Constructor from a track and (optionally) pointer to mag field service
    TrackTraj(const LHCb::Track& track, const IMagneticFieldSvc* magfieldsvc=0) ;

    /// Constructor from an unsorted set of states and (optionally) pointer to mag field service
    TrackTraj(const StateContainer& states, const IMagneticFieldSvc* magfieldsvc=0) ;

    /// Constructor from a (sorted) Track::StateContainer and (optionally) pointer to mag field service
    TrackTraj(const LHCb::Track::StateContainer& states, const IMagneticFieldSvc* magfieldsvc=0) ;

    /// Constructor from a (sorted) Track::NodesContainer and (optionally) pointer to mag field service
    TrackTraj(const std::vector<Node*>& nodes, const IMagneticFieldSvc* magfieldsvc=0) ;

    /// Destructor
    virtual ~TrackTraj() {}

    /// Clone method
    std::unique_ptr<Trajectory> clone() const
      { return std::unique_ptr<Trajectory>(new TrackTraj(*this)); }

    /// Point on the trajectory at arclength from the starting point
    Point position( double z ) const ;

    /// First derivative of the trajectory at arclength from the starting point
    Vector direction( double z ) const ;

    /// Second derivative of the trajectory at arclength from the starting point
    Vector curvature( double z ) const ;

    /// Point on the trajectory at arclength from the starting point
    Vector momentum( double z ) const ;

    /// State at given z
    State state(double z) const ;

    /// State at given z
    StateVector stateVector(double z) const ;

    /// Expand this track in z
    void expansion( double z, Point& p, Vector& dp, Vector& ddp ) const ;

    /// distance where the deviation of the trajectory from the expansion
    /// reaches the given tolerance.
    double distTo1stError( double mu,
                           double tolerance,
                           int pathDirection = +1 ) const ;

    /// distance where the deviation of the trajectory from the expansion
    /// reaches the given tolerance.
    double distTo2ndError( double mu,
                           double tolerance,
                           int pathDirection = +1 ) const;

    using ZTrajectory::arclength;
    /// arclength between 2 coordinates on the track
    double arclength( double z1, double z2 ) const ;

     /// Derivative of arclength to mu
    double dArclengthDMu(double z) const ;

    /// Estimate for mu which minimizes point poca
    double muEstimate(const Gaudi::XYZPoint& p) const ;

    /// return the set of reference states
    const StateContainer& refStates() const { return m_states ; }

    /// return the set of reference statevectors for this parameterization (if any)
    std::vector<StateVector> refStateVectors() const ;

  protected:
    /// return the set of reference states
    StateContainer& refStates() { return m_states ; }

#ifdef __INTEL_COMPILER       // Disable ICC warning
  #pragma warning(disable:68) // integer conversion resulted in a change of sign
  #pragma warning(push)
#endif
    /// invalidate the cache
    void invalidateCache() { m_cachedindex = InvalidCacheIndex ; }
#ifdef __INTEL_COMPILER       // End disable ICC warning
  #pragma warning(pop)
#endif

  private:
    void updatecache(double z) const ;
    void init(const IMagneticFieldSvc* magfieldsvc) ;
  private:
    enum { InvalidCacheIndex = -1 } ;
    StateContainer m_states ; ///< Container of states
    Gaudi::XYZVector m_bfield ; ///< Bfield at upstream end of track
    mutable size_t m_cachedindex ; ///< Index for cached z-range
    mutable CubicStateInterpolationTraj m_cachedinterpolation ; ///< Cached interpolation for z-range
  } ;

  /*************************************************************************************************/
  // inline functions
  /*************************************************************************************************/

  inline Trajectory::Point TrackTraj::position( double z ) const {
    updatecache(z) ;
    return m_cachedinterpolation.position(z) ;
  }

  inline Trajectory::Vector TrackTraj::momentum( double z ) const {
    updatecache(z) ;
    return m_cachedinterpolation.momentum(z) ;
  }

  inline Trajectory::Vector TrackTraj::direction( double z ) const {
    updatecache(z) ;
    return m_cachedinterpolation.direction(z) ;
  }

  inline Trajectory::Vector TrackTraj::curvature( double z ) const {
    updatecache(z) ;
    return m_cachedinterpolation.curvature(z) ;
  }

  inline LHCb::State TrackTraj::state(double z) const {
    updatecache(z) ;
    return m_cachedinterpolation.CubicStateInterpolationTraj::state(z) ;
  }

  inline LHCb::StateVector TrackTraj::stateVector(double z) const {
    updatecache(z) ;
    return m_cachedinterpolation.CubicStateInterpolationTraj::stateVector(z) ;
  }

  inline void TrackTraj::expansion( double z, Trajectory::Point& p, Trajectory::Vector& dp, Trajectory::Vector& ddp ) const {
    updatecache(z) ;
    return m_cachedinterpolation.expansion(z,p,dp,ddp) ;
  }

  inline double TrackTraj::muEstimate(const Gaudi::XYZPoint& p) const {
    updatecache(p.z()) ;
    return m_cachedinterpolation.muEstimate(p) ;
  }

  inline double TrackTraj::dArclengthDMu(double z) const {
    updatecache(z) ;
    return m_cachedinterpolation.dArclengthDMu(z) ;
  }

}

#endif
