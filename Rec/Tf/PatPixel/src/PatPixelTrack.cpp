// LHCb
#include "Event/State.h"
// Local
#include "PatPixelTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPixelTrack
//
// 2012-01-06 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPixelTrack::PatPixelTrack() :
  m_backward(false),
  m_x0(0.), m_tx(0.),
  m_y0(0.), m_ty(0.),
  m_s0(0.), m_sx(0.), m_sz(0.), m_sxz(0.), m_sz2(0.),
  m_u0(0.), m_uy(0.), m_uz(0.), m_uyz(0.), m_uz2(0.) {

  m_hits.reserve(20);

}

//=============================================================================
// Reset and prepare a new track with these two hits
//=============================================================================
void PatPixelTrack::set(PatPixelHit* h1, PatPixelHit* h2) {
  m_backward = false;
  m_hits.clear();
  m_hits.push_back(h1);

  const double x = h1->x();
  const double y = h1->y();
  const double z = h1->z();
  double w = h1->wx();
  m_s0  = w;
  m_sx  = w * x;
  m_sz  = w * z;
  m_sxz = w * x * z;
  m_sz2 = w * z * z;

  w = h1->wy();
  m_u0  = w;
  m_uy  = w * y;
  m_uz  = w * z;
  m_uyz = w * y * z;
  m_uz2 = w * z * z;

  addHit(h2);

}

//=========================================================================
//  Add a hit to the track. Update fit parameters.
//=========================================================================
void PatPixelTrack::addHit(PatPixelHit* hit) {

  m_hits.push_back(hit);

  const double x = hit->x();
  const double y = hit->y();
  const double z = hit->z();
  double w = hit->wx();
  m_s0  += w;
  m_sx  += w * x;
  m_sz  += w * z;
  m_sxz += w * x * z;
  m_sz2 += w * z * z;

  w = hit->wy();
  m_u0  += w;
  m_uy  += w * y;
  m_uz  += w * z;
  m_uyz += w * y * z;
  m_uz2 += w * z * z;

  if (m_hits.size() > 1) solve();

}

//=========================================================================
// Remove a hit from the track. Update fit parameters.
//=========================================================================
void PatPixelTrack::removeHit(PatPixelHit* hit) {

  PatPixelHits::iterator it = std::find(m_hits.begin(), m_hits.end(), hit);
  if (it == m_hits.end()) return;
  m_hits.erase(it);
  const double x = hit->x();
  const double y = hit->y();
  const double z = hit->z();
  double w = hit->wx();
  m_s0  -= w;
  m_sx  -= w * x;
  m_sz  -= w * z;
  m_sxz -= w * x * z;
  m_sz2 -= w * z * z;

  w = hit->wy();
  m_u0  -= w;
  m_uy  -= w * y;
  m_uz  -= w * z;
  m_uyz -= w * y * z;
  m_uz2 -= w * z * z;

  if (m_hits.size() > 1) solve();

}

//=========================================================================
// Recompute the track parameters
//=========================================================================
void PatPixelTrack::solve() {

  double den = (m_sz2 * m_s0 - m_sz * m_sz);
  if (fabs(den) < 10e-10) den = 1.;
  m_tx = (m_sxz * m_s0  - m_sx  * m_sz) / den;
  m_x0 = (m_sx  * m_sz2 - m_sxz * m_sz) / den;
  
  den = (m_uz2 * m_u0 - m_uz * m_uz);
  if (fabs(den) < 10e-10) den = 1.;
  m_ty = (m_uyz * m_u0  - m_uy  * m_uz) / den;
  m_y0 = (m_uy  * m_uz2 - m_uyz * m_uz) / den;

}

//=========================================================================
//  Return the covariance matrix of the last fit at the specified z
//=========================================================================
Gaudi::TrackSymMatrix PatPixelTrack::covariance(double z) {

  Gaudi::TrackSymMatrix cov;
  //== Ad hoc matrix inversion, as it is almost diagonal!
  double m00 = m_s0;
  double m11 = m_u0;
  double m20 = m_sz - z * m_s0;
  double m31 = m_uz - z * m_u0;
  double m22 = m_sz2 - 2 * z * m_sz + z * z * m_s0;
  double m33 = m_uz2 - 2 * z * m_uz + z * z * m_u0;
  double den20 = m22 * m00 - m20 * m20;
  double den31 = m33 * m11 - m31 * m31;
  
  cov(0,0) =  m22/den20;
  cov(2,0) = -m20/den20;
  cov(2,2) =  m00/den20;

  cov(1,1) =  m33/den31;
  cov(3,1) = -m31/den31;
  cov(3,3) =  m11/den31;

  cov(4,4) = 1.;
  return cov;

}

//===============================================================================

namespace
{
  /// Helper function to sort hits
  struct SortDecreasingZ
  { SortDecreasingZ() {}
    bool operator()( const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->z() > rhs->z(); }
  } ;

  /// Helper function to filter one hit
  inline double filter( double z,
			double& x, double& tx,
			double& covXX, double& covXTx, double& covTxTx,
			double zhit, double xhit, double whit )
  {
    // compute the prediction
    double dz = zhit - z ;
    double predx = x + dz * tx ;
    double predcovXX  = covXX + 2* dz * covXTx + dz * dz * covTxTx ;
    double predcovXTx = covXTx + dz * covTxTx ;
    double predcovTxTx = covTxTx ;
    // compute the gain matrix
    double R = 1/whit + predcovXX ;
    double Kx  = predcovXX  / R ;
    double KTx = predcovXTx / R ;
    // update the state vector
    double r = xhit - predx ;
    x  = predx + Kx  * r ;
    tx = tx    + KTx * r ;
    // update the covariance matrix. we can write it in many ways ...
    covXX   /*= predcovXX  - Kx * predcovXX */             = (1 - Kx) * predcovXX ;
    covXTx  /*= predcovXTx - predcovXX * predcovXTx / R */ = (1 - Kx) * predcovXTx ;
    covTxTx = predcovTxTx - KTx * predcovXTx ;
    // return the chi2
    return r*r/R ;
  }
}

//===============================================================================
// Fit the track with a Kalman filter, allowing for some scattering at
// every hit. Function arguments:
//  state: state at the last filtered hit.
//  direction=+1 : filter in positive z direction (not normally what you want)
//  direction=-1 : filter in negative z direction
//  noise2PerLayer: scattering contribution (squared) to tx and ty
// The return value is the chi2 of the fit  
// ===============================================================================
double
PatPixelTrack::fitKalman( LHCb::State& state, int direction, double noise2PerLayer ) const
{
  // WH: hits are apparently not perfectly sorted. if we really want
  // to use this in the future, we probably want to sort them only
  // once. for now, we sort on every call. since hits seem to be
  // almost sorted in decreasing Z, that's what we'll stick to.
  PatPixelTrack* nonconstthis = const_cast<PatPixelTrack*>(this) ;
  std::sort( nonconstthis->m_hits.begin(), nonconstthis->m_hits.end(), SortDecreasingZ() ) ;

  // assume the hits are sorted, but don't assume anything on the direction of sorting
  int N = int(m_hits.size()) ;
  int firsthit = 0 ;
  int lasthit  = N-1 ;
  int dhit = +1 ;
  if( ( m_hits[lasthit]->z() - m_hits[firsthit]->z() ) * direction < 0 ) {
    std::swap( firsthit, lasthit ) ;
    dhit = -1 ;
  }
  
  // we filter x and y simultaneously but take them uncorrelated.
  // filter first the first hit.

  const PatPixelHit* hit = m_hits[firsthit] ;
  double x  = hit->x() ;
  double tx = m_tx ;
  double y  = hit->y() ;
  double ty = m_ty ;
  double z  = hit->z() ; 

  // initialize the covariance matrix
  double covXX = 1/hit->wx() ;
  double covYY = 1/hit->wy() ;
  double covXTx  = 0 ; // no initial correlation
  double covYTy  = 0 ;
  double covTxTx = 1 ; // randomly large error
  double covTyTy = 1 ; 
  
  // add remaining hits
  double chi2(0) ;
  for(int i = firsthit + dhit; i != lasthit + dhit; i += dhit ) {
    hit = m_hits[i] ;
    // add the noise
    covTxTx += noise2PerLayer ;
    covTyTy += noise2PerLayer ;
    // filter X
    chi2 += filter( z, x, tx, covXX, covXTx, covTxTx, hit->z(), hit->x(), hit->wx() ) ;
    // filter Y
    chi2 += filter( z, y, ty, covYY, covYTy, covTyTy, hit->z(), hit->y(), hit->wy() ) ;
    // update z (note done in the filter, since needed only once)
    z = hit->z() ;
  }

  // add the noise at the last hit
  covTxTx += noise2PerLayer ;
  covTyTy += noise2PerLayer ;

  // finally, fill the state
  state.setX( x ) ;
  state.setY( y ) ;
  state.setZ( z ) ;
  state.setTx( tx ) ;
  state.setTy( ty ) ;
  state.covariance()(0,0) = covXX ;
  state.covariance()(0,2) = covXTx ;
  state.covariance()(2,2) = covTxTx ;
  state.covariance()(1,1) = covYY ;
  state.covariance()(1,3) = covYTy ;
  state.covariance()(3,3) = covTyTy ;
  return chi2 ;
}
