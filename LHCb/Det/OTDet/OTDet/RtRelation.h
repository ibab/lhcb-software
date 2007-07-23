#ifndef OTDET_RTRELATION
#define OTDET_RTRELATION

#include <boost/assign/list_of.hpp>

namespace OTDet
{
  class RtRelation
  {
  public:
    RtRelation(float tmin=0,float tmax=42, const std::vector<float>& radius = boost::assign::list_of(0.0)(2.5) ) ;
    float radius(float time) const ;
    float extrapolatedradius( float time ) const ;
    float drdt(float time) const ;
    float drifttime( float radius) const ;
    float drdt() const { return (m_rmax-m_rmin)/(m_tmax-m_tmin) ; }
    float tmax() const { return m_tmax ; }
  private:
    float m_tmin ;
    float m_tmax ;
    std::vector<float> m_radius ;
    float m_dt ;
    float m_rmin ;
    float m_rmax ;
  } ;
  
  inline RtRelation::RtRelation(float tmin, float tmax, const std::vector<float>& radius) 
    : m_tmin(tmin), m_tmax(tmax), m_radius(radius), m_dt( tmax / (radius.size()-1)), m_rmin(radius.front()), m_rmax(radius.back()) 
  {}
  
  inline float RtRelation::radius( float time ) const 
  {
    float        fbin = (time-m_tmin)/m_dt ;
    unsigned int ibin = int(fbin) ;
    float        w    = fbin - ibin ;
    return time <= m_rmin ? m_rmin : ( time >= m_tmax ? m_rmax : (1-w)*m_radius[ibin] + w*m_radius[ibin+1] ) ;
  }

  inline float RtRelation::extrapolatedradius( float time ) const 
  {
    // horribly expensive
    float fbin = (time-m_tmin)/m_dt ;
    int ibin = int(fbin) ;
    int maxbin = m_radius.size()-2 ;
    ibin = ibin<=0 ? 0 : (ibin >= maxbin ? maxbin: ibin) ;
    float w    = fbin - ibin ;
    return (1-w)*m_radius[ibin] + w*m_radius[ibin+1] ;
  }
  
  inline float RtRelation::drdt( float time ) const 
  {
    float fbin = time/m_dt ;
    unsigned int ibin = std::min((unsigned int)(fbin),(unsigned int)m_radius.size()-1) ;
    return (m_radius[ibin+1] - m_radius[ibin])/m_dt ;
  }
  
  inline float RtRelation::drifttime( float r ) const 
  {
    // first find the bin with a binary search
    std::vector<float>::const_iterator it = std::lower_bound(m_radius.begin(),m_radius.end(),r) ;
    size_t ibin = it - m_radius.begin() ;
    return r <= m_rmin ? m_tmin :
      (r >= m_rmax ? m_tmax :
       m_tmin + m_dt * (ibin + (r - m_radius[ibin])/(m_radius[ibin+1]-m_radius[ibin])) ) ;
  }
}

#endif
