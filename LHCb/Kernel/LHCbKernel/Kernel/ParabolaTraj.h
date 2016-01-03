#ifndef LHCbKernel_ParabolaTraj_H
#define LHCbKernel_ParabolaTraj_H 1

// Include files
#include "Kernel/DifTraj.h"
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif
#include "GaudiKernel/boost_allocator.h"

namespace LHCb
{

  /** @class ParabolaTraj ParabolaTraj.h Kernel/ParabolaTraj.h
   *
   * Trajectory parameterized as a parabola
   *
   * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
   * @date   09/01/2006
   */

  class ParabolaTraj: public Trajectory {

  public:

    /// Destructor
    virtual ~ParabolaTraj() {};

    // clone thyself...
    virtual std::unique_ptr<Trajectory> clone() const;

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

    /// Determine the distance in arclenghts to the
    /// closest point on the trajectory to a given point
    virtual double muEstimate( const Point& point ) const;

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

    /// Distance, along the Trajectory, between position(mu1) and
    /// position(mu2). Trivial because ParabolaTraj is parameterized in
    /// arclength.
    using Trajectory::arclength;
    virtual double arclength(double mu1, double mu2) const { return mu2 - mu1 ; }


#ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(ParabolaTraj) == size ?
               boost::singleton_pool<ParabolaTraj, sizeof(ParabolaTraj)>::malloc() :
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
      boost::singleton_pool<ParabolaTraj, sizeof(ParabolaTraj)>::is_from(p) ?
        boost::singleton_pool<ParabolaTraj, sizeof(ParabolaTraj)>::free(p) :
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

    Point  m_pos;
    Vector m_dir;
    Vector m_curv;

  }; // class ParabolaTraj

} // namespace LHCb

#endif /// LHCbKernel_ParabolaTraj_H
