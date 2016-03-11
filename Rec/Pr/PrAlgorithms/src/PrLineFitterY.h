#ifndef PRLINEFITTERY_H 
#define PRLINEFITTERY_H 1

// Include files
#include "Math/CholeskyDecomp.h"
#include "PrKernel/PrHit.h"
#include "PrHybridSeedTrack.h"
#include "PrKernel/PrHitManager.h"
#include "PrPlaneHybridCounter.h"
#include "SIMDLine.h"

/** @class PrLineFitterY PrLineFitterY.h
 *  Support class to perform Fit of Lines using solely hits in UV layers setting the parameters of
 *  the xz projection and fitting passing iterators of the hough-like cluster
 *  @author renato quagliani
 *  @date   2015-08-01
*/
//#define Truth_Match// (switch on if want to store minCoord; maxCoord and hits uv)
class PrLineFitterY {
public: 
  /// Standard constructor
  PrLineFitterY(const float zRef , const PrHybridSeedTrack& track):
    m_zRef(zRef),
    m_nHitsUV(0),
    m_ax(track.ax()),
    m_bx(track.bx()),
    m_cx(track.cx()),
    m_dRatio(track.dRatio())
  {
    //    m_Vectorized = false;    
    m_ay = std::numeric_limits<float>::max();
    m_by = std::numeric_limits<float>::max();
    m_chi2line = std::numeric_limits<float>::max();
  };
  /// Standard constructor setting only the zReference
  PrLineFitterY( const float zRef):
    m_zRef(zRef),
    m_nHitsUV(0),
    m_ax(std::numeric_limits<float>::max()),
    m_bx(std::numeric_limits<float>::max()),
    m_cx(std::numeric_limits<float>::max()),
    m_dRatio(std::numeric_limits<float>::max()){
    m_ay = std::numeric_limits<float>::max();
    m_by = std::numeric_limits<float>::max();
    m_chi2line = std::numeric_limits<float>::max();
    //m_nHitsUV = 0;
  }
  /** Set values of the xzprojection 
   */
  void setXProj( const PrHybridSeedTrack& track){
    m_ax = track.ax();
    m_bx = track.bx();
    m_cx = track.cx();
    m_dRatio =  track.dRatio();
  }
  float zref() const{
    return m_zRef;
  }
  void reset(){
    m_ay= std::numeric_limits<float>::max();
    m_by= std::numeric_limits<float>::max();
    m_chi2line = std::numeric_limits<float>::max();
    m_nHitsUV = 0; 
  }
  // return xzprojection slope at a given z
  float xSlope( const float z) const{
    const float dz = z-m_zRef;
    return m_bx + 2*m_cx*dz + 3*m_cx*m_dRatio*dz*dz;
  }
  // return xzprojection x position at a given z
  float x(const float z) const{
    const float dz = z - m_zRef;
    return ( m_ax + m_bx*dz + m_cx*dz*dz*(1.+m_dRatio*dz));
  }
  // return track-hit distance
  float distance( PrHit *hit) const{
    const float z = hit->z();
    const float yAtZ = y( z );
    return(hit->x(yAtZ)-x(z) ); //new
  }
  // return the chi2 contribution from a hit
  float chi2hit( PrHit* hit) const{
    float erry = hit->w();//*(corr*corr);
    const float dist = distance( hit);
    return dist*dist*erry;
  }
  float yOnTrack( PrHit *hit) const{ return hit->yOnTrack( m_ay - m_zRef*m_by , m_by);}
  float y( float z) const{ return  m_ay + m_by*(z-m_zRef);}
  float DeltaY( PrHit *hit){
    if( hit->isX() ) return 0.;
    return distance(hit);
  }
  bool fit2(PrHits::const_iterator itBeg, PrHits::const_iterator itEnd, PrPlaneHybridCounter& counter){
    m_ay = 0.f;// std::numeric_limits<float>::max();
    m_by = 0.f;//std::numeirc_limits<float>::max();
    m_chi2line = std::numeric_limits<float>::max();
    m_nHitsUV=0;
    std::fill(m_rhs,m_rhs+2,0.);
    std::fill(m_mat,m_mat+3,0.);
    for(PrHits::const_iterator hit = itBeg; itEnd!= hit; ++hit){
      if( (*hit)->isX()) continue;
      //const double wdxDy = (hit)->w()/(float)counter.nbInPlane[ (*hit)->planeCode()];
      const float nbInPlane =  (float)counter.nbInPlane( (*hit)->planeCode() );
      const double dz = (*hit)->z()-m_zRef;
      const double dist = distance( (*hit) );
      const double dxDy = (*hit)->dxDy()  ;
      //const double CorrError = 1./(1.+ 1.27389* std::fabs( std::atan( xSlope( (*hit) ->z( )) )));
      //const double err2 = (*hit)->werr()*CorrError;
      const double wdxDy = (*hit)->w()*dxDy/nbInPlane;
      const double wdxDydz = wdxDy*dz;
      m_mat[0]+=wdxDy*dxDy;
      m_mat[1]+=wdxDy*dxDy*dz;
      m_mat[2]+=wdxDydz*dz * dxDy;
      m_rhs[0]-=wdxDy*dist;
      m_rhs[1]-=wdxDydz*dist; 
    }
    ROOT::Math::CholeskyDecomp<double,2> decomp(m_mat);
    if(!decomp){
      //if(msgLevel(MSG::DEBUG)) std::cout<<"UNvable to Fit"<<std::endl;
      m_ay = std::numeric_limits<float>::max();
      m_by = std::numeric_limits<float>::max();
      m_chi2line = std::numeric_limits<float>::max();
      return false;
    }
    decomp.Solve(m_rhs);
    m_ay = m_rhs[0];
    m_by = m_rhs[1];
    m_chi2line = 0.f;
    m_nHitsUV =0 ;
    for( PrHits::const_iterator hit = itBeg; itEnd!=hit ;++hit){
      if( (*hit )->isX()) continue;
      m_nHitsUV++;
      m_chi2line+= chi2hit(*hit);  
    }
    m_chi2line= m_chi2line/(float)(m_nHitsUV-2);
    return true;
  }
  // method to fit the track passing the iterators pointing to the initial and last element of the hough clusters
  bool fit(PrHits::const_iterator itBeg, PrHits::const_iterator itEnd){
    //initialize params to 0
    m_ay = 0.f;
    m_by = 0.f;
    m_chi2line = std::numeric_limits<float>::max();
    m_nHitsUV=0;
    //m_nHitsUV = std::count_if( itBeg, itEnd, [](const PrHit *h) {return !h->isX();});
    float __attribute__((aligned(64))) dzeta[8];
    float __attribute__((aligned(16))) dw[8];
    float __attribute__((aligned(16))) dy[8];
    unsigned j = 0;
    for(auto hit = itBeg; itEnd!= hit; ++hit, ++j){
      if( (*hit)->isX()){ j--; continue;}
      const float dxDy =  (*hit)->dxDy()  ;
      const float errhit = (*hit)->werr();
      dzeta[j] = (*hit)->z()-m_zRef;
      dw[j] = std::fabs(dxDy)*errhit;
      dy[j] = distance((*hit)) / dxDy;
    }
    while (j & 3){
      dzeta[j] = dw[j] = dy[j] = 0.f;
      ++j;
    }
    SIMDLineFit fit;
    fit.addHits(dzeta,&dzeta[j],dw,dy);
    SIMDLine line = fit.solve();
    if(!line.ok()){
      m_chi2line = std::numeric_limits<float>::max();
      m_ay = std::numeric_limits<float>::max();
      m_by = std::numeric_limits<float>::max();
      return false;
    }else{
      m_ay-=line.x();
      m_by-=line.tx();
      m_nHitsUV =  std::count_if( itBeg, itEnd, [](const PrHit *h) {return !h->isX();});
      m_chi2line = line.chi2( dzeta, &dzeta[j], dw, dy)/float(m_nHitsUV-2);
      return true;
    }
    return false;
  }
  unsigned int nHitsLine(){
    return m_nHitsUV;
  }
  
  
  float ay() const{    return m_ay;}
  float ay0() const{ return m_ay-m_by*m_zRef;}
  float by() const{    return m_by;}
  float Chi2DoF() const{
    if(m_nHitsUV >0){
      return m_chi2line;
    }
    return std::numeric_limits<float>::max();
  }
  ~PrLineFitterY() {} ///< Destructor
  protected:
private:
  //  bool  m_vectorised;
  float m_zRef;
  unsigned int m_nHitsUV;
  float m_mat[3];
  float m_rhs[2];
  float m_ay;
  float m_by;
  float m_chi2line;
  float m_ax;
  float m_bx;
  float m_cx;
  float m_dRatio;
//PrHybridSeedTrack m_track;
};
#endif // PRLINEFITTERY_H
