// $Id: $
#ifndef VPTRACK_H 
#define VPTRACK_H 1
#include "gsl/gsl_cdf.h"
// Include files
#include "VPHit.h"
#include "Event/StateVector.h"
/** @class VPTrack VPTrack.h
 *  
 *
 *  @author Wenbin Qian
 *  @date   2011-02-18
 */
class VPTrack {
public: 
  /// Standard constructor
  VPTrack( ):
    m_valid(true),
    mx_e (0.),
    mx_x (0.),
    mx_z (0.),
    mx_xx(0.),
    mx_zz(0.),
    mx_xz(0.),
    my_e (0.),
    my_y (0.),
    my_z (0.),
    my_yy(0.),
    my_zz(0.),
    my_yz(0.),
    m_tx  (0.),
    m_x0  (0.),
    m_ty  (0.),
    m_y0  (0.),
    m_chi2(0.)
  {
    m_hits.reserve(30);
  }; 

  virtual ~VPTrack( ){
    m_hits.clear();
  }; ///< Destructor
  
  void addXHit (VPHit* hit)
  {
    m_hits.push_back(hit);

    double x = hit->x();
    double dx = hit->dx();
    double z = hit->z();
    double wx = 1./(dx*dx);
    
    mx_e += wx;
    mx_x += wx*x;
    mx_z += wx*z;
    mx_xx+= wx*x*x;
    mx_zz+= wx*z*z;
    mx_xz+= wx*x*z;
 
   if(my_e > 0.000000000001){
    my_e = 0;
    my_y = 0;
    my_z = 0;
    my_yy = 0;
    my_zz = 0;
    my_yz = 0;
    m_ty = 0;
    m_y0 = 0;
  }


    if(m_hits.size()>=2){
      m_tx = (mx_xz*mx_e-mx_z*mx_x)/(mx_zz*mx_e-mx_z*mx_z);
      m_x0 = (mx_x*mx_zz-mx_xz*mx_z)/(mx_zz*mx_e-mx_z*mx_z);
    }
  }
  
  void removeXHit (VPHit* hit)
  {
    //think whether use erase or popback?
    m_hits.erase( std::remove( m_hits.begin(), m_hits.end(), hit), m_hits.end());
    double x = hit->x();
    double dx = hit->dx();
    double z = hit->z();
    double wx = 1./(dx*dx);
    
    mx_e -= wx;
    mx_x -= wx*x;
    mx_z -= wx*z;
    mx_xx-= wx*x*x;
    mx_zz-= wx*z*z;
    mx_xz-= wx*x*z;

    if(my_e > 0.000000000001){    
    my_e = 0;
    my_y = 0;
    my_z = 0;
    my_yy = 0;
    my_zz = 0;
    my_yz = 0;
    m_ty = 0;
    m_y0 = 0;
  }
    if(m_hits.size()>=2){
      m_tx = (mx_xz*mx_e-mx_z*mx_x)/(mx_zz*mx_e-mx_z*mx_z);
      m_x0 = (mx_x*mx_zz-mx_xz*mx_z)/(mx_zz*mx_e-mx_z*mx_z);
    }
  }
  
  void UpdateYHits()
  {
    if(m_hits.size()>2){
      for(VPHits::iterator ihit = m_hits.begin(); ihit != m_hits.end(); ihit++){
        VPHit* hit = (*ihit);
        double y = hit->y();
        double dy = hit->dy();
        double z = hit->z();
        double wy = 1./(dy*dy);
        
        my_e += wy;
        my_y += wy*y;
        my_z += wy*z;
        my_yy+= wy*y*y;
        my_zz+= wy*z*z;
        my_yz+= wy*y*z;
      }
      m_ty = (my_yz*my_e-my_z*my_y)/(my_zz*my_e-my_z*my_z);
      m_y0 = (my_y*my_zz-my_yz*my_z)/(my_zz*my_e-my_z*my_z);
    }
  }

  
  double getChi2(){
    m_chi2 =  my_yy-m_ty*my_yz-m_y0*my_y + mx_xx-m_tx*mx_xz-m_x0*mx_x;
    return m_chi2;
  }
  int getHitsNum() const {return m_hits.size();}
  double xAtz( double z ) {return m_x0 + m_tx * z;}
  double yAtz( double z ) {return m_y0 + m_ty * z;}
  double zBeam () {return -( m_x0 * m_tx + m_y0 * m_ty )/( m_tx * m_tx + m_ty * m_ty);}
  VPHits hits() { return m_hits;}
  bool isValid() const { return m_valid; }
  void setValid( bool flag ) { m_valid = flag; }
  double probChi2(double chi2,int nDoF) const 
  {
    double val(0) ; 
    if (nDoF>0) { 
      const double limit = 1e-15;
      double chi2max = gsl_cdf_chisq_Qinv (limit, nDoF); 
      val = chi2 < chi2max ? gsl_cdf_chisq_Q(chi2,nDoF) : 0; 
    } 
    return val ;
  }
  

  
  LHCb::StateVector state( double z ){
    LHCb::StateVector temp;
    temp.setX ( m_x0 + z * m_tx );
    temp.setY ( m_y0 + z * m_ty );
    temp.setZ ( z );
    temp.setTx ( m_tx );
    temp.setTy ( m_ty );
    temp.setQOverP ( 0. );
    return temp;
  }
  
  Gaudi::TrackSymMatrix covariance( double z ){
    Gaudi::TrackSymMatrix cov;
    cov(0,0) = 6.e-5+ 0*z;
    cov(1,1) = 6.e-5;
    cov(2,2) = 6.e-5;
    cov(3,3) = 6.e-5;
    cov(4,4) = 6.e-5;
    return cov;
  }
    

  inline void display() const{
   for(VPHits::const_iterator itH = m_hits.begin(); itH != m_hits.end(); itH++){
      std::cout<<"hits: "<<(*itH)->z()<<" "<<(*itH)->x()
               <<" "<<(*itH)->y()<<" "<<(*itH)->getused()
               <<" "<<"dx: "<<(*itH)->x()-m_tx*(*itH)->z()-m_x0
               <<" dy: "<<(*itH)->y()-m_ty*(*itH)->z()-m_y0
               <<" chi2 "<<m_chi2<<" sensor "<<(*itH)->sensor()<<std::endl;
    }	
  }

protected:

private:
  bool m_valid;

  double mx_e;
  double mx_x;
  double mx_z;
  double mx_xx;
  double mx_zz;
  double mx_xz;
  
  double my_e;
  double my_y;
  double my_z;
  double my_yy;
  double my_zz;
  double my_yz;
  double m_tx;
  double m_x0;
  double m_ty;
  double m_y0;
  double m_chi2;
  VPHits m_hits;
};
typedef std::vector<VPTrack> VPTracks; 
#endif // VPTRACK_H
