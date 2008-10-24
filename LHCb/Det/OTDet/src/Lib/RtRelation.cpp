#include "OTDet/RtRelation.h"
//#include <math.h>
#include <cmath>
#include <iostream>

namespace OTDet
{
  
  RtRelation::RtRelation(double rmax, 
			 const std::vector<double>& tcoeff, 
			 const std::vector<double>& terrcoeff,
			 size_t ntbins)
    : m_rmax(rmax),
      m_tcoeff(tcoeff),
      m_terrcoeff(terrcoeff)
  {
    initRTable(ntbins) ;
  }

  RtRelation::RtRelation(double rmax, 
			 const std::vector<double>& tcoeff, 
			 double sigmaR, size_t ntbins)
    : m_rmax(rmax),
      m_tcoeff(tcoeff),
      m_terrcoeff(tcoeff.size()-1)
  {
    // we want a fixed error in the radius. that just means that
    // sigma_t(r) is the derivative of t(r)
    for(size_t i=0; i<m_terrcoeff.size(); ++i) m_terrcoeff[i] = sigmaR * m_tcoeff[i+1] * (i+1) / m_rmax ;
    // now initialize the table
    initRTable(ntbins) ;
  }
  
  void RtRelation::initRTable(size_t ntbins)
  {
    // create a table for the r(t) relation. this table has a fixed bin
    // size, otherwise things are not fast enough.
    m_tmin = polyeval(m_tcoeff,0) ;
    m_tmax = polyeval(m_tcoeff,1) ;
    m_dt   = (m_tmax-m_tmin)/(ntbins-1) ;
    m_rtable.resize( ntbins ) ;
    std::vector<double> derivcoeff(m_tcoeff.size()-1) ;
    for(size_t i=0; i<derivcoeff.size(); ++i) derivcoeff[i] = m_tcoeff[i+1] * (i+1) / m_rmax ;
    m_rtable.front() = RadiusWithError(0,drifttimeError(0)/polyeval(derivcoeff,0)) ;
    m_rtable.back()  = RadiusWithError(m_rmax,drifttimeError(m_rmax)/polyeval(derivcoeff,1)) ;
    // the rest goes numeric (newton-raphson) 
    for(size_t i = 1; i<ntbins-1; ++i) {
      double t = m_tmin + i*m_dt ;
      const double precision = 0.001 ;
      double r = m_rtable[i-1].val ;
      const unsigned char maxtries = 10 ;
      size_t ntries = 0 ;
      double residual(0) ;
      do {
	residual = t - drifttime(r) ;
	r += residual / polyeval(derivcoeff,r/m_rmax) ;
      } while( ++ntries<maxtries && std::abs(residual) > precision) ;
      
      double dtdr = polyeval(derivcoeff,r/m_rmax) ;
      m_rtable[i] = RadiusWithError(r, drifttimeError(r)/dtdr ) ;
    }
  }
  
  //   RtRelation::RtRelation(double tmin, double tmax, const std::vector<double>& radius) 
  //     : m_tmin(tmin), m_tmax(tmax), m_rtable(radius), m_dt( (tmax-tmin) / (radius.size()-1)), m_rmin(radius.front()), m_rmax(radius.back()) 
  //   {
  //     // now we still need to create the coefficients for the t(r)
  //     // function. the best is just to make a chebychev approximation
  //     // (evaluation on nodes) , but I don't have time to think about
  //     // that now. I'll just decapriacte this function for the time
  //     // being and make a linear approximation on the first and last
  //     // point. this makes only sense for the MC linear rt-relation (for
  //     // which we are never going to use it).
  //     double dtdr = (m_tmax-m_tmin) / (m_rmin - m_rmax) ;
  //     m_tcoeff = boost::assign::list_of(m_tmin - m_rmin * dtdr)(dtdr) ;
  //     std::cout << "This constructor is not complete. Please FIXME." << std::endl ;
  //   }
  
}
