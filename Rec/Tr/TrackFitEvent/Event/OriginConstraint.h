#ifndef TRACKFITEVENT_ORIGINCONSTRAINT_H
#define TRACKFITEVENT_ORIGINCONSTRAINT_H

// Include files
#include "Event/Measurement.h"

#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif

#include "GaudiKernel/boost_allocator.h"
#include <ostream>

// Forward declarations

namespace LHCb 
{

  // Forward declarations
  class ChiSquare ;
  class State ;

  class OriginConstraint : public LHCb::Measurement
  {
  public:
    OriginConstraint( const Gaudi::XYZPoint& point,
		      const Gaudi::SymMatrix3x3& covariance ) ;
  
    /// Default Destructor
    virtual ~OriginConstraint() {}
  
    /// Clone the OTMeasurement
    LHCb::Measurement* clone() const { return new OriginConstraint(*this) ; }

    /// filter this constraint. returns the chi2
    LHCb::ChiSquare filter( LHCb::State& state,
			    const Gaudi::TrackVector& reference) const ;
    
#ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(OriginConstraint) == size ? 
               boost::singleton_pool<OriginConstraint, sizeof(OriginConstraint)>::malloc() :
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
      boost::singleton_pool<OriginConstraint, sizeof(OriginConstraint)>::is_from(p) ?
	boost::singleton_pool<OriginConstraint, sizeof(OriginConstraint)>::free(p) :
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
    Gaudi::XYZPoint m_origin ;
    Gaudi::SymMatrix3x3 m_weight ; // weight matrix (inverse covariance)  
  };
}  
#endif
