#ifndef LHCbKernel_CircleTraj_H
#define LHCbKernel_CircleTraj_H 1

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

  /** @class CircleTraj CircleTraj.h Kernel/CircleTraj.h
   *
   * Trajectory parameterized as a circle
   *
   * @author Edwin Bos, Eduardo Rodrigues
   * @date   30/01/2006
   *
   */

  class CircleTraj : public Trajectory {

  public:

    /// Default Destructor
    virtual ~CircleTraj() {};

    // clone thyself...
    virtual std::unique_ptr<Trajectory> clone() const;

    /// Constructor from a center, the normal which defines the plane
    /// of the circle, a vector from the center to a point on the circle,
    /// and the range of the circle (radius*phirange, phi=0 corresponds to
    /// the above point on the circle, phi is the angle which is used to
    /// rotate this point around the normal)
    /// Only the component of origin2point perpendicular to normal
    /// is considered!!!...
    CircleTraj( const Point& origin,// center of circle
                const Vector& normal, // direction of end
                const Vector& origin2point, // direction of start
                const Trajectory::Range& range); // valid range, in radius*deltaphi

    /// Constructor from a center, the directions of the
    /// start and end of the traj wrt. the center, and the radius.
    /// The Traj goes along the 'short' arc from origin+radius*dir1.unit()
    /// at arclength=0 to origin+radius*dir2.unit() at the arclen=length().
    CircleTraj( const Point& origin,// center of circle
                const Vector& dir1, // direction of start
                const Vector& dir2, // direction of end
                double radius);

    /// Point on the trajectory at arclength from the starting point
    virtual Point position( double arclength ) const;

    /// First derivative of the trajectory at arclength from the starting point
    virtual Vector direction( double arclength ) const;

    /// Second derivative of the trajectory at arclength from the starting point
    virtual Vector curvature( double arclength ) const;

    /// Create a parabolic approximation to the trajectory
    /// at arclength from the starting point
    virtual void expansion( double arclength,
                            Point& p,
                            Vector& dp,
                            Vector& ddp ) const;

    /// Return arclen at which the trajectory is
    /// closest to the specified point
    virtual double muEstimate( const Point& point ) const;

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

    /// Distance, along the Trajectory, between position(mu1) and
    /// position(mu2). Trivial because CircleTraj is parameterized in
    /// arclength.
    using Trajectory::arclength;
    virtual double arclength(double mu1, double mu2) const { return mu2 - mu1 ; }

#ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(CircleTraj) == size ?
               boost::singleton_pool<CircleTraj, sizeof(CircleTraj)>::malloc() :
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
      boost::singleton_pool<CircleTraj, sizeof(CircleTraj)>::is_from(p) ?
        boost::singleton_pool<CircleTraj, sizeof(CircleTraj)>::free(p) :
        ::operator delete(p);
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
#endif


  private :

    Point  m_origin;
    Vector m_normal;
    Vector m_dirStart;
    double m_radius;
    double m_cbrt6radius2;

  }; // class CircleTraj

} // namespace LHCb

#endif /// LHCbKernel_CircleTraj_H
