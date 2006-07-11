// $Id: LineTraj.h,v 1.13 2006-07-11 09:49:54 mneedham Exp $
#ifndef LHCbKernel_LineTraj_H
#define LHCbKernel_LineTraj_H 1

// Include files
#include "Kernel/DifTraj.h"

#include "GaudiKernel/boost_allocator.h"


/** @class LineTraj LineTraj.h
 *
 * Trajectory parameterized as a straight line
 *
 * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 * @date   17/01/2005
 * 
 */

namespace LHCb
{

  class LineTraj: public DifTraj<5> {
    
  public:
    
    /// Enum providing number of colums in derivative matrix
    enum { kSize = 5 };
    
    /// Destructor
    virtual ~LineTraj() {};

    // clone thyself...
    virtual std::auto_ptr<Trajectory> clone() const;
    
    /// Constructor from the middle point and a (unit) direction vector
    LineTraj( const Point& middle,
              const Vector& dir,
              const Range& range );
    
    // constructor from a normalized vector
    LineTraj( const Point& middle,
              const Vector& dir,
              const Range& range,
              bool normalized );
    

    /// Constructor from a begin and an end point
    LineTraj( const Point& begPoint,
              const Point& endPoint );
    
    /// Point on the trajectory at arclength from the starting point    
    virtual Point position( double arclength ) const;
    
    /// First derivative of the trajectory at arclength from the starting point
    virtual Vector direction( double arclength=0 ) const;
    
    /// Second derivative of the trajectory at arclength from the starting point
    virtual Vector curvature( double arclength=0 ) const;
    
    /// Create a parabolic approximation to the trajectory
    /// at arclength from the starting point
    virtual void expansion( double arclength,
                            Point& p,
                            Vector& dp,
                            Vector& ddp ) const;
    
    /// Retrieve the derivative of the parabolic approximation to the
    /// trajectory with respect to the state parameters
    virtual Derivative derivative( double arclength=0 ) const;
    
    /// Determine the distance in arclenghts to the
    /// closest point on the trajectory to a given point
    virtual double arclength( const Point& point ) const;
    
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

    
#ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(LineTraj) == size ?
               boost::singleton_pool<LineTraj, sizeof(LineTraj)>::malloc() :
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
      boost::singleton_pool<LineTraj, sizeof(LineTraj)>::is_from(p) ?
      boost::singleton_pool<LineTraj, sizeof(LineTraj)>::free(p) :
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
    
    Vector m_dir;
    Point  m_pos;
    
  }; // class LineTraj
  
} // namespace LHCb

/// Constructor from the middle point and a unit direction vector
inline LHCb::LineTraj::LineTraj( const Point& middle,
                    const Vector& dir,
                    const Range& range ) 
  : DifTraj<kSize>(range),
    m_dir(dir.Unit()),
    m_pos(middle)
{
};

/// Constructor from the middle point and a unit direction vector
inline LHCb::LineTraj::LineTraj( const Point& middle,
                    const Vector& dir,
                    const Range& range, bool ) 
  : DifTraj<kSize>(range),
    m_dir(dir),
    m_pos(middle)
{
};


#endif /// LHCbKernel_LineTraj_H
