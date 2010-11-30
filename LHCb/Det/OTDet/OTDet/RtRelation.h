#ifndef OTDET_RTRELATION
#define OTDET_RTRELATION

#include <gsl/gsl_poly.h>
#include <vector>

namespace OTDet
{
  
  struct ValueWithError
  {
    ValueWithError() : val(0), err(0) {}
    ValueWithError(double v, double e) : val(v), err(e) {}
    double val ;
    double err ;
  } ;

  inline ValueWithError interpolate( const double wlhs, const ValueWithError lhs, 
				     const double wrhs, const ValueWithError rhs) 
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
    /// constructor from a polynomial t(r) and sigmat(r).  the
    /// coefficients are actually those of a polynominal in r/rmax,
    /// such that they all have unit [ns].
    RtRelation(double rmax, const std::vector<double>& tcoeff, const std::vector<double>& terrcoeff, size_t nbins=49) ;
    ///  constructor from a polynomial t(r) and a fixed error in r. this is only for convenience with MC.
    RtRelation(double rmax, const std::vector<double>& tcoeff, double sigmaR, size_t nbins=25) ;
    /// return radius for given drifttime
    RadiusWithError radiusWithError(double time) const ;
    /// return radius for given drifttime
    double radius(double time) const ;
    /// return radius for given drifttime. extrapolate into 'unphysical' region.
    double extrapolatedradius( double time ) const ;
    /// return derivative to time
    double drdt(double time) const ;
    /// return derivative to radius
    double dtdr(double radius) const ;
    /// return drifttime for given radius
    double drifttime( double radius) const ;
    /// return sigma(drifttime) for given radius
    double drifttimeError( double radius) const ;
    /// return drifttime with error
    DriftTimeWithError drifttimeWithError( double radius ) const ;
    /// return average driftvelocity
    double drdt() const { return m_rmax/(m_tmax-m_tmin) ; }
    /// return maximum drift time
    double tmax() const { return m_tmax ; }
    /// return minimum drift time
    double tmin() const { return m_tmin ; }
    /// return maximum drift radius
    double rmax() const { return m_rmax ; }
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
    double m_rmax ;
    std::vector<double> m_tcoeff ;    /// coefficients of polynomial t(r/rmax) all in [ns]
    std::vector<double> m_terrcoeff ; /// coefficients of polynomial sigma_t(r/rmax) all in [ns]
    double m_tmin ;
    double m_tmax ;
    double m_dt ;
    std::vector<RadiusWithError> m_rtable ; /// table which maps to a radius with error
  } ;

  inline RadiusWithError RtRelation::radiusWithError( double time ) const 
  {
    double        fbin = (time-m_tmin)/m_dt ;
    unsigned int ibin = int(fbin) ;
    double        w    = fbin - ibin ;
    return time <= m_tmin ? m_rtable.front() : 
      ( time >= m_tmax ? m_rtable.back() : 
	interpolate( (1-w), m_rtable[ibin], w, m_rtable[ibin+1] ) ) ;
  }
  
  inline double RtRelation::radius( double time ) const 
  {
    double        fbin = (time-m_tmin)/m_dt ;
    unsigned int ibin = int(fbin) ;
    double        w    = fbin - ibin ;
    return time <= m_tmin ? m_rtable.front().val : 
      ( time >= m_tmax ? m_rtable.back().val : 
	(1-w)*m_rtable[ibin].val + w*m_rtable[ibin+1].val ) ;
  }
  
  inline double RtRelation::extrapolatedradius( double time ) const 
  {
    // horribly expensive
    double fbin = (time-m_tmin)/m_dt ;
    int ibin = int(fbin) ;
    int maxbin = m_rtable.size()-2 ;
    ibin = ibin<=0 ? 0 : (ibin >= maxbin ? maxbin: ibin) ;
    double w    = fbin - ibin ;
    return (1-w)*m_rtable[ibin].val + w*m_rtable[ibin+1].val ;
  }
  
  inline double RtRelation::drdt( double time ) const 
  {
    double fbin = time/m_dt ;
    unsigned int ibin = std::min((unsigned int)(fbin),(unsigned int)m_rtable.size()-2) ;
    return (m_rtable[ibin+1].val - m_rtable[ibin].val)/m_dt ;
  }
  
  inline double RtRelation::drifttime( double r ) const 
  {
    return polyeval(m_tcoeff, r<m_rmax ? r/m_rmax : 1 ) ;
  }

  inline double RtRelation::drifttimeError( double r ) const 
  {
    return polyeval(m_terrcoeff, r<m_rmax ? r/m_rmax : 1) ;
  }

  inline DriftTimeWithError RtRelation::drifttimeWithError( double r ) const 
  {
    return DriftTimeWithError(drifttime(r),drifttimeError(r)) ;
  }

  inline double RtRelation::dtdr( double r ) const 
  {
    // we probably want to cache these ...
    std::vector<double> derivcoeff(m_tcoeff.size()-1) ;
    for( size_t i=0; i<derivcoeff.size(); ++i) derivcoeff[i] = m_tcoeff[i+1] * (i+1) / m_rmax ;
    return polyeval(derivcoeff,r/m_rmax) ;
  }
  
}

#endif
