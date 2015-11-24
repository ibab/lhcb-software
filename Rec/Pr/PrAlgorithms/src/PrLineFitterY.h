#ifndef PRLINEFITTERY_H 
#define PRLINEFITTERY_H 1

// Include files
#include "Math/CholeskyDecomp.h"
#include "PrKernel/PrHit.h"
#include "PrHybridSeedTrack.h"
#include "PrKernel/PrHitManager.h"
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
  PrLineFitterY(const double zRef , const PrHybridSeedTrack& track):
    m_zRef(zRef),
    m_nHitsUV(0),
    m_ax(track.ax()),
    m_bx(track.bx()),
    m_cx(track.cx()),
    m_dRatio(track.dRatio()),
    m_SlopeCorr(track.slopeCorr())
  {
    m_ay = 9999.;
    m_by = 999.;
    m_chi2line = 1.e14;
  };
  /// Standard constructor setting only the zReference
  PrLineFitterY( const double zRef):
    m_zRef(zRef),
    m_nHitsUV(0),
    m_ax(-9999.),
    m_bx(-9999.),
    m_cx(-9999.),
    m_dRatio(-9999.),
    m_SlopeCorr(false){
    m_ay = 9999.;
    m_by = 999.;
    m_chi2line = 1.e14;
    //m_nHitsUV = 0;
  }
  /** Set values of the xzprojection 
   */
  void setXProj( const PrHybridSeedTrack& track){
    m_ax = track.ax();
    m_bx = track.bx();
    m_cx = track.cx();
    m_dRatio =  track.dRatio();
    m_SlopeCorr = track.slopeCorr();
  }
  
  double zref() const
  {
    return m_zRef;
  }
  
  void reset(){
    m_ay=999.;
    m_by=999.;
    m_chi2line = 1.e14;
    m_nHitsUV = 0; 
  }
  

  // return xzprojection slope at a given z
  double xSlope( const double z) const{
    const double dz = z-m_zRef;
    return m_bx + 2*m_cx*dz + 3*m_cx*m_dRatio*dz*dz;
  }
  
  // return xzprojection x position at a given z
  double x(const double z) const{
    const double dz = z - m_zRef;
    return ( m_ax + m_bx*dz + m_cx*dz*dz*(1.+m_dRatio*dz));
  }

  // return track-hit distance
  double distance( PrHit *hit) const{
    const double z = hit->z();
    const double yAtZ = y( z );
    return(hit->x(yAtZ)-x(z) ); //new
    //return (hit->x(0.)-x( z ) )/hit->dxDy() - y( z );
  }
  
  // return the chi2 contribution from a hit
  double chi2( PrHit* hit) const{
    //double erry = hit->w()*std::pow(hit->dxDy(),2);
    double erry = hit->w();
    const double dist = distance( hit);
    if(m_SlopeCorr){
      double z = hit->z();
      double xSlopeVal = xSlope( z ) ;
      double cos = std::cos( xSlopeVal );
      erry = erry/(cos*cos);
    }
    return dist*dist*erry;
  }
  
  // return the y position on track given a hit
  double yOnTrack( PrHit *hit) const{ return hit->yOnTrack( m_ay - m_zRef*m_by , m_by);}
  double y( double z) const{ return  m_ay + m_by*(z-m_zRef);}
  double DeltaY( PrHit *hit){
    if( hit->isX() ) return 0.;
    return distance(hit);
  }

  // method to fit the track passing the iterators pointing to the initial and last element of the hough clusters
  bool fit(PrHits::const_iterator itBeg, PrHits::const_iterator itEnd){
    //Re-initialize the track params
    m_ay = 0.;
    m_by = 0.;
    m_nHitsUV = 0 ;
    for( int i = 0; i<3; i++){
      std::fill(m_rhs,m_rhs+2,0.);
      std::fill(m_mat,m_mat+3,0.);
      for(PrHits::const_iterator hit = itBeg; itEnd!= hit; ++hit){
        if( (*hit)->isX()) continue;
        double w = (*hit)->w();
        const double dz = (*hit)->z()-m_zRef;
        const double dist = distance((*hit));
        const double dxDy = (*hit)->dxDy();
        const double wdxDy = dxDy*w;
        const double wdxDydz = wdxDy * dz;
        if(m_SlopeCorr){
          double cos = std::cos( xSlope( (double)(*hit)->z()));
          w = w*(cos*cos);
        }
        m_mat[0]+=wdxDy*dxDy;
        m_mat[1]+=wdxDy*dxDy*dz;
        m_mat[2]+=wdxDydz*dz * dxDy;
        m_rhs[0]-=wdxDy * dist;
        m_rhs[1]-=wdxDydz * dist;
      }
      ROOT::Math::CholeskyDecomp<double,2> decomp(m_mat);
      if(!decomp){ //if(msgLevel(MSG::DEBUG)) std::cout<<"UNvable to Fit"<<std::endl;
        m_chi2line = 1.e14;
        return false;
      }
      decomp.Solve(m_rhs);
      m_ay+=m_rhs[0];
      m_by+=m_rhs[1];
    }
    m_chi2line = 0.;
    for( PrHits::const_iterator hit = itBeg; itEnd!=hit ;++hit){
      if( (*hit )->isX()) continue;
      m_nHitsUV++;
      m_chi2line+= chi2(*hit);
    }
    m_chi2line = m_chi2line/(m_nHitsUV-2.);
    return true;
  }
  unsigned int nHitsLine(){
    return m_nHitsUV;
  }
  double ay() const{    return m_ay;}
  double ay0() const{ return m_ay-m_by*m_zRef;}
  
  double by() const{    return m_by;}

  double Chi2DoF() const{
    if(m_nHitsUV >0)
      return m_chi2line;
    return 1.e30;
  }
  ~PrLineFitterY() {} ///< Destructor
  protected:
private:
  double m_zRef;
  unsigned int m_nHitsUV;
  double m_mat[3];
  double m_rhs[2];
  double m_ay;
  double m_by;
  double m_chi2line;
  double m_ax;
  double m_bx;
  double m_cx;
  double m_dRatio;
  bool m_SlopeCorr;
  //PrHybridSeedTrack m_track;
};
#endif // PRLINEFITTERY_H
