// $Id: $
// Include files 

// local
#include "PatPixelTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPixelTrack
//
// 2012-01-06 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPixelTrack::PatPixelTrack(  ) :
  m_backward(false),
  m_x0( 0. ),
  m_tx( 0. ),
  m_y0( 0. ),
  m_ty( 0. ),

  m_s0 ( 0. ),
  m_sx ( 0. ),
  m_sz ( 0. ),
  m_sxz( 0. ),
  m_sz2( 0. ),

  m_u0 ( 0. ),
  m_uy ( 0. ),
  m_uz ( 0. ),
  m_uyz( 0. ),
  m_uz2( 0. )
{
  m_hits.reserve( 20 );
}

//=========================================================================
//  Add a hit to the track. Update fit parameters
//=========================================================================
void PatPixelTrack::addHit ( PatPixelHit* hit ) {
  m_hits.push_back( hit );
  double z = hit->z();
  double w = hit->wx();
  double x = hit->x();

  m_s0  = m_s0  + w;
  m_sx  = m_sx  + w * x;
  m_sz  = m_sz  + w * z;
  m_sxz = m_sxz + w * x * z;
  m_sz2 = m_sz2 + w * z * z;

  w = hit->wy();
  double y = hit->y();

  m_u0  = m_u0  + w;
  m_uy  = m_uy  + w * y;
  m_uz  = m_uz  + w * z;
  m_uyz = m_uyz + w * y * z;
  m_uz2 = m_uz2 + w * z * z;

  if( m_hits.size() > 2 ){
    double den = ( m_sz2 * m_s0 - m_sz * m_sz );
    if ( fabs(den) < 10e-10 ) den = 1.;
    m_tx     = ( m_sxz * m_s0  - m_sx  * m_sz ) / den;
    m_x0     = ( m_sx  * m_sz2 - m_sxz * m_sz ) / den;

    den = ( m_uz2 * m_u0 - m_uz * m_uz );
    if ( fabs(den) < 10e-10 ) den = 1.;
    m_ty     = ( m_uyz * m_u0  - m_uy  * m_uz ) / den;
    m_y0     = ( m_uy  * m_uz2 - m_uyz * m_uz ) / den;
  }
}

//=========================================================================
//  Return the covariance matrix of the last fit at the specified z
//=========================================================================
Gaudi::TrackSymMatrix PatPixelTrack::covariance( double z ) {
  Gaudi::TrackSymMatrix cov;

  cov(0,0) = m_s0;
  cov(1,1) = m_u0;

  cov(2,0) = m_sz - z * m_s0;
  cov(3,1) = m_uz - z * m_u0;

  cov(2,2) = m_sz2 - 2 * z * m_sz + z * z * m_s0;
  cov(3,3) = m_uz2 - 2 * z * m_uz + z * z * m_u0;

  cov(4,4) = 1.;

  bool ifail = cov.InvertChol( );
  if (!ifail ) std::cout << "PatPixelTrack:covariance: Choleski inverstion failed." << std::endl;
  
  return cov;
}
//=============================================================================
