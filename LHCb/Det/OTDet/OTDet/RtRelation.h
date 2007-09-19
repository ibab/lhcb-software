#ifndef OTDET_RTRELATION
#define OTDET_RTRELATION

#include <boost/assign/list_of.hpp>
#include <gsl/gsl_poly.h>
#include <vector>

namespace OTDet
{
  
  struct ValueWithError
  {
    ValueWithError() : val(0), err(0) {}
    ValueWithError(float v, float e) : val(v), err(e) {}
    float val ;
    float err ;
  } ;

  inline ValueWithError interpolate( const float wlhs, const ValueWithError lhs, 
				     const float wrhs, const ValueWithError rhs) 
  { 
    return ValueWithError( wlhs*lhs.val + wrhs*rhs.val, wlhs*lhs.err + wrhs*rhs.err ) ;
  }

  typedef ValueWithError RadiusWithError ;
  typedef ValueWithError DriftTimeWithError  ;
    
  class RtRelation
  {
  public:
    /// default constructor. does nothing sensible.
    RtRelation() {}
    /// constructor from a polynomial t(r). this is what you should use.
    RtRelation(float rmax, const std::vector<double>& tcoeff, const std::vector<double>& terrcoeff, size_t nbins=25) ;
    ///  constructor from a polynomial t(r) and a fixed error in r. this is only for convenience with MC.
    RtRelation(float rmax, const std::vector<double>& tcoeff, float sigmaR, size_t nbins=25) ;
    /// constructor from a table o bins r[t]. decapricated.
    /// RtRelation(float tmin,float tmax, const std::vector<float>& radius = boost::assign::list_of(0.0)(2.5) ) ;
    /// return radius for given drifttime
    RadiusWithError radiusWithError(float time) const ;
    /// return radius for given drifttime
    float radius(float time) const ;
    /// return radius for given drifttime. extrapolate into 'unphysical' region.
    float extrapolatedradius( float time ) const ;
    /// return derivative to time
    float drdt(float time) const ;
    /// return derivative to radius
    float dtdr(float radius) const ;
    /// return drifttime for given radius
    float drifttime( float radius) const ;
    /// return sigma(drifttime) for given radius
    float drifttimeError( float radius) const ;
    /// return drifttime with error
    DriftTimeWithError drifttimeWithError( float radius ) const ;
    /// return average driftvelocity
    float drdt() const { return m_rmax/(m_tmax-m_tmin) ; }
    /// return maximum drift time
    float tmax() const { return m_tmax ; }
    /// return minimum drift time
    float tmin() const { return m_tmin ; }
    /// return maximum drift radius
    float rmax() const { return m_rmax ; }
    /// return coefficients of polynimial t(r)
    const std::vector<double>& tcoeff() const { return m_tcoeff ; }
    /// return coefficients of polynimial sigma_t(r)
    const std::vector<double>& terrcoeff() const { return m_terrcoeff ; }
    /// number of points in the r(t) table
    size_t nbins() const { return m_rtable.size() ; }
  private:
    static double polyeval(const std::vector<double>& coeff, double x) { 
      return gsl_poly_eval(&(coeff.front()),coeff.size(),x) ; }
    void initRTable(size_t ntbins) ;
  private:
    float m_rmax ;
    std::vector<double> m_tcoeff ;    /// coefficients of polynomial t(r)
    std::vector<double> m_terrcoeff ; /// coefficients of polynomial sigma_t(r)
    //float m_rmin ;
    float m_tmin ;
    float m_tmax ;
    float m_dt ;
    std::vector<RadiusWithError> m_rtable ; /// table which maps to a radius with error
  } ;

  inline RadiusWithError RtRelation::radiusWithError( float time ) const 
  {
    float        fbin = (time-m_tmin)/m_dt ;
    unsigned int ibin = int(fbin) ;
    float        w    = fbin - ibin ;
    return time <= m_tmin ? m_rtable.front() : 
      ( time >= m_tmax ? m_rtable.back() : 
	interpolate( (1-w), m_rtable[ibin], w, m_rtable[ibin+1] ) ) ;
  }
  
  inline float RtRelation::radius( float time ) const 
  {
    float        fbin = (time-m_tmin)/m_dt ;
    unsigned int ibin = int(fbin) ;
    float        w    = fbin - ibin ;
    return time <= m_tmin ? m_rtable.front().val : 
      ( time >= m_tmax ? m_rtable.back().val : 
	(1-w)*m_rtable[ibin].val + w*m_rtable[ibin+1].val ) ;
  }
  
  inline float RtRelation::extrapolatedradius( float time ) const 
  {
    // horribly expensive
    float fbin = (time-m_tmin)/m_dt ;
    int ibin = int(fbin) ;
    int maxbin = m_rtable.size()-2 ;
    ibin = ibin<=0 ? 0 : (ibin >= maxbin ? maxbin: ibin) ;
    float w    = fbin - ibin ;
    return (1-w)*m_rtable[ibin].val + w*m_rtable[ibin+1].val ;
  }
  
  inline float RtRelation::drdt( float time ) const 
  {
    float fbin = time/m_dt ;
    unsigned int ibin = std::min((unsigned int)(fbin),(unsigned int)m_rtable.size()-2) ;
    return (m_rtable[ibin+1].val - m_rtable[ibin].val)/m_dt ;
  }
  
  inline float RtRelation::drifttime( float r ) const 
  {
    return float(polyeval(m_tcoeff,r )) ;
  }

  inline float RtRelation::drifttimeError( float r ) const 
  {
    return float(polyeval(m_terrcoeff,r)) ;
  }

  inline DriftTimeWithError RtRelation::drifttimeWithError( float r ) const 
  {
    return DriftTimeWithError(drifttime(r),drifttimeError(r)) ;
  }

  inline float RtRelation::dtdr( float r ) const 
  {
    // we probably want to cache these ...
    std::vector<double> derivcoeff(m_tcoeff.size()-1) ;
    for( size_t i=0; i<derivcoeff.size(); ++i) derivcoeff[i] = m_tcoeff[i+1] * (i+1) ;
    return float(polyeval(derivcoeff,r)) ;
  }
  
}

#endif
