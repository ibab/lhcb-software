#ifndef OTDET_OTWIRETRAJ
#define OTDET_OTWIRETRAJ

#include "Kernel/Trajectory.h"
#include "GaudiKernel/SystemOfUnits.h"

/*****************************/
/* class representing a piecewise trajectory, with pieces of fixed length, parameterized along y */
/*****************************/

namespace LHCb
{

  /// Interface class
  class OTWireTraj : public Trajectory
  {
  public:
    typedef unsigned int Index ;
    OTWireTraj(const double beginyrange, const double endyrange) : Trajectory(beginyrange,endyrange) {}
    virtual void applyTranslation( const Vector& vec ) = 0 ;
    virtual OTWireTraj* cloneOTWireTraj() const = 0 ;
    virtual void indexAndMu(double y, unsigned int& index, double& localmu) const = 0 ;
    virtual Index numSegments() const = 0 ;
  } ;
  
  /// Templated implementation class
  template<unsigned int N>
    class OTWireTrajImp : public OTWireTraj
    {
    private:
      // helper class to parameterize locally a line
      class Segment 
      {
      private:
	Point  m_begin ;
	Vector m_dir ;
      public:
	Segment() {} 
	Segment( const Point& begin,
		 const Point& end ) {
	  m_begin = begin ;
	  m_dir = (end - begin) / (end.y()-begin.y() ) ;
	}

	const Point& begin() const { return m_begin ; }
	
	Point position( double mu ) const {
	  return m_begin + (mu-m_begin.y()) * m_dir ;
	}
	
	Vector direction() const {
	  // do we really need unit here?
	  return m_dir.Unit();
	}
	
	void expansion( double mu, Point& p,
			Vector& dp, Vector& ddp ) const
	{
	  ddp = Vector(0,0,0);
	  dp  = m_dir;
	  p   = m_begin + (mu-m_begin.y()) * m_dir;
	}
	
	void applyTranslation( const Vector& translation ) {
	  m_begin += translation ;
	}
      } ;
      
      double yBegin() const { return m_segments.front().begin().y() ; }
      
    private:
      std::array<Segment,N> m_segments ; /// subsegments
      double m_deltay ; /// Length of 3 segments (not equal to range)
      double m_dndy ;   /// normalization for arclength
    
    public:
      /// Default constructor. The list of points must be even: each
      /// specifies the begin and end of a section between two wire
      /// locators. Effectively, all segments have the same length
      /// (808mm). The range is specified independent of these points
      /// because for short modules, the actual length of the wire is
      /// not 3 times the space between the wire locators.


    OTWireTrajImp( const std::array<Point,2*N>& points, const double beginyrange, const double endyrange) 
      : OTWireTraj(beginyrange, endyrange)
	{
	  for( size_t i=0; i<N; ++i)
	    m_segments[i] = Segment( points[2*i], points[2*i+1] ) ;
	  Vector delta = points.back() - points.front() ;
	  m_deltay = delta.y() ;
	  m_dndy = delta.R() / delta.y() ;
	}
      
      /// Destructor
      virtual ~OTWireTrajImp() {} 
      
      // We create this one with template specialization to speed things up
      inline Index index(double y) const ;

      /// apply a translation in space to entire trajectory
      void applyTranslation( const Vector& vec ) {
	// move the segments
	for( Segment& iseg : m_segments ) iseg.applyTranslation( vec ) ;
	// change the range
	Range r = range() ;
	setRange( r.first+vec.y(), r.second+vec.y() ) ;
      }

      /// returns a mu in units of range, within a segment. used in the calibration.
      void indexAndMu(double y, Index& index, double& localmu) const {
	double frac = N*(y - yBegin()) / m_deltay ;
	int i = int(frac) ;
	index = i<=0 ? 0 : (i>=int(N) ? N-1 : i ) ;
	localmu = frac - index ;
      }
        
      OTWireTraj* cloneOTWireTraj() const {
	return new OTWireTrajImp<N>(*this) ;
      }
      
      Index numSegments() const { return N ; }

    public: // implementation for member functions of the interface
      
      std::auto_ptr<Trajectory> clone() const {
	return std::auto_ptr<Trajectory>(new OTWireTrajImp<N>(*this)) ;
      }
      
      /// trajectory is parameterized along y
      virtual double muEstimate( const Point& p) const { 
	return p.y() ;
      }
      
      /// Point on the trajectory
      Point position( double mu ) const {
	return m_segments[index(mu)].position( mu ) ;
      }
      
      Vector direction( double mu) const {
	return m_segments[index(mu)].direction() ;
      }
      
      Vector curvature( double ) const {
	return Vector(0.0,0.0,0.0) ;
      }
      
      /// Expansion ... all that matters for TrajPoca
      void expansion( double mu, Point& p, Vector& dp, Vector& ddp ) const {
	m_segments[index(mu)].expansion(mu,p,dp,ddp ) ;
      }
      
      double distTo1stError( double, double, int) const {
	return 10*Gaudi::Units::km;
      }
      
      double distTo2ndError( double, double, int) const {
	return 10*Gaudi::Units::km;
      }
      
      /// arclength for given range
      virtual double arclength(double mu1, double mu2) const {
	return (mu2 - mu1)*m_dndy;
      }

      /// arclength
      virtual double arclength() const { 
	return arclength(beginRange(),endRange()) ;
      }
      
#ifndef GOD_NOALLOC
      /// operator new
      static void* operator new ( size_t size )
      {
	return ( sizeof(OTWireTrajImp<N>) == size ?
		 boost::singleton_pool<OTWireTrajImp<N>, sizeof(OTWireTrajImp<N>)>::malloc() :
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
	boost::singleton_pool<OTWireTrajImp<N>, sizeof(OTWireTrajImp<N>)>::is_from(p) ?
	  boost::singleton_pool<OTWireTrajImp<N>, sizeof(OTWireTrajImp<N>)>::free(p) :
	  ::operator delete(p);
      }
      
    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
#endif
  } ;

  template<unsigned int N>
    OTWireTraj::Index OTWireTrajImp<N>::index( double y) const { 
    double frac = (y - yBegin())/m_deltay ;
    int i = int(N*frac) ;
    return i<=0 ? 0 : (i>=int(N) ? N-1 : i ) ;
  }
  
  template<>
    OTWireTraj::Index OTWireTrajImp<1>::index(double) const { 
    return 0 ;
  }
}
#endif
