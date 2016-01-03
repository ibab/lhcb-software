// $Id: StateTraj.h,v 1.1 2009-07-08 13:33:45 wouter Exp $
#ifndef TRACKFITEVENT_STATETRAJ_H
#define TRACKFITEVENT_STATETRAJ_H 1

// Include files
// -------------

// STL
#include <memory>

// from LHCbKernel
#include "Kernel/DifTraj.h"
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif
#include "GaudiKernel/boost_allocator.h"

// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/State.h"
#include "Event/StateVector.h"

namespace LHCb
{

  /** @class StateTraj StateTraj.h TrackKernel/StateTraj.h
   *
   * Trajectory created from a State.
   *
   * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
   * @date   01/12/2005
   */

  class StateTraj: public DifTraj<5> {

  public:

    /// Enum providing number of colums in derivative matrix
    enum { kSize = 5 };

    /// get me another one of these!
    std::unique_ptr<Trajectory> clone() const;

    /// Default Destructor
    virtual ~StateTraj() {};

    /// Constructor from a State and the magnetic field at the State position
    StateTraj( const LHCb::State& state,
               const Gaudi::XYZVector& bField );

    /// Constructor from a StateVector and the magnetic field at the State position
    StateTraj( const LHCb::StateVector& state,
               const Gaudi::XYZVector& bField );
    
    /// Constructor from a StateVector and the magnetic field at State position
    StateTraj( const Gaudi::TrackVector& stateVector,
               double z,
               const Gaudi::XYZVector& bField );

    /// Point on trajectory where parabolic approximation is made
    virtual Gaudi::XYZPoint position( double arclength ) const;

    /// First derivative of the trajectory at the approximation point
    virtual Gaudi::XYZVector direction( double arclength ) const;

    /// Second derivative of the trajectory at the approximation point,
    /// used as the constant value of the curvature of the parabolic approximation
    virtual Gaudi::XYZVector curvature( double arclength) const;


    /// Create a parabolic approximation to the trajectory
    virtual void expansion( double arclength,
                            Gaudi::XYZPoint& p,
                            Gaudi::XYZVector& dp,
                            Gaudi::XYZVector& ddp ) const;

    /// Retrieve the parameters of this traj...
    virtual Parameters parameters( ) const;

    /// Update the parameters of this traj...
    virtual StateTraj& operator+=(const Parameters& delta);

    /// Retrieve the derivative of the parabolic approximation to the
    /// trajectory with respect to the state parameters
    virtual Derivative derivative( double arclength ) const;

    /// give arclength where this trajectory is closest to the
    /// specified point
    virtual double muEstimate( const Gaudi::XYZPoint& point) const;

    /// Number of arclengths until deviation of the trajectory from the expansion
    /// reaches the given tolerance (does not account for the curvature).
    virtual double distTo1stError( double arclength,
                                   double tolerance,
                                   int pathDirection = +1 ) const;

    /// Number of arclengths until deviation of the trajectory from the
    /// expansion reaches the given tolerance (accounts for the curvature).
    virtual double distTo2ndError( double arclen,
                                   double tolerance,
                                   int pathDirection = +1 ) const;


    using LHCb::DifTraj<5>::arclength;
    /// Distance, along the Trajectory, between position(mu1) and
    /// position(mu2). Trivial because StateTraj is parameterized in
    /// arclength.
    virtual double arclength(double mu1, double mu2) const { return mu2 - mu1 ; }
    
#ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(StateTraj) == size ?
               boost::singleton_pool<StateTraj, sizeof(StateTraj)>::malloc() :
               ::operator new(size) );
    }

    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }

    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<StateTraj, sizeof(StateTraj)>::is_from(p) ?
        boost::singleton_pool<StateTraj, sizeof(StateTraj)>::free(p) :
        ::operator delete(p);
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
#endif

  private:

    Gaudi::XYZPoint  m_pos;    ///< the position of the State
    Gaudi::XYZVector m_dir;    ///< the unit direction of the State
    double           m_qOverP; ///< the charge-over-momentum Q/P of the State
    Gaudi::XYZVector m_bField; ///< the magnetic field vector at the State position
    Gaudi::XYZVector m_curv;   ///< constant value of parabola's curvature

  }; // class StateTraj

} // namespace LHCb

#endif /// TRACKFITEVENT_STATETRAJ_H
