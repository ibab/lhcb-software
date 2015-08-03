// LHCb
#include "Event/State.h"
// Local
#include "PrPixelTrack.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrPixelTrack::PrPixelTrack()
    : m_tx(0.), m_ty(0.),
      m_x0(0.), m_y0(0.) {

  m_hits.reserve(20);

  // vector constants initialization
  v_compValue = _mm_set1_ps(10e-10);
  v_1s = _mm_set1_ps(1.0f);
  v_2s = _mm_set1_ps(2.0f);
  v_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
}

//=========================================================================
// Compute the track parameters
//=========================================================================
void PrPixelTrack::fit() {
  // fit in intrinsics (SSE, SSE2)
  __m128 v0, v1, v2, v3, v4, v5, v6, v7, v_wxwy;

  // First iteration of loop, unrolled (shuffle instead of addition)
  // Precondition: m_hits.size() > 0
  auto ith = m_hits.cbegin();
  const auto end = m_hits.cend();

  v0 = _mm_loadu_ps( (*ith)->p_x() ); // x,y,z
  const float wx = (*ith)->wx();  
  const float wy = (*ith)->wy();
  v1 = _mm_load_ss( &wy );
  v2 = _mm_load_ss( &wx );
  v_wxwy = _mm_shuffle_ps(v2, v1, _MM_SHUFFLE(0,0,0,0)); // wx,wx,wy,wy  
  
  v1 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(2,1,2,0)); // x,z,y,z
  v2 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(2,2,2,2)); // z,z,z,z
  v3 = _mm_mul_ps(v1, v_wxwy); // wx*x, wx*z, wy*y, wy*z
  v4 = _mm_mul_ps(v3, v2); // wx*x*z, wx*z*z, wy*y*z, wy*z*z

  v_s0 = _mm_shuffle_ps(v_wxwy, v_wxwy, _MM_SHUFFLE(2,2,0,0)); // v_s0: s0,s0,u0,u0
  v_sx = _mm_shuffle_ps(v3, v3, _MM_SHUFFLE(3,2,1,0));     // v_sx: sx,sz,uy,uz
  v_sxz = _mm_shuffle_ps(v4, v4, _MM_SHUFFLE(3,2,1,0));   // v_sxz: sxz,sz2,uyz,uz2

  ++ith;
  for (; ith != end; ++ith) {
    
    v0 = _mm_loadu_ps( (*ith)->p_x() ); // x,y,z  

    const float wx = (*ith)->wx();  
    const float wy = (*ith)->wy();
    v1 = _mm_load_ss( &wy );
    v2 = _mm_load_ss( &wx );
    v_wxwy = _mm_shuffle_ps(v2, v1, _MM_SHUFFLE(0,0,0,0)); // wx,wx,wy,wy  
    
    v1 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(2,1,2,0)); // x,z,y,z
    v2 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(2,2,2,2)); // z,z,z,z
    v3 = _mm_mul_ps(v1, v_wxwy); // wx*x, wx*z, wy*y, wy*z
    v4 = _mm_mul_ps(v3, v2); // wx*x*z, wx*z*z, wy*y*z, wy*z*z

    v_s0 = _mm_add_ps(v_s0, v_wxwy); // v_s0: s0,s0,u0,u0
    v_sx = _mm_add_ps(v_sx, v3); // v_sx: sx,sz,uy,uz
    v_sxz = _mm_add_ps(v_sxz, v4); // v_sxz: sxz,sz2,uyz,uz2
  }

  // float den =  (b->m_sz2 * b->m_s0 - b->m_sz * b->m_sz);
  // float den2 = (b->m_uz2 * b->m_u0 - b->m_uz * b->m_uz);
  
  v1 = _mm_shuffle_ps(v_sxz, v_sx, _MM_SHUFFLE(3,1,3,1));
  v2 = _mm_shuffle_ps(v_s0, v_sx, _MM_SHUFFLE(3,1,2,0));
  v3 = _mm_mul_ps(v1, v2);
  v1 = _mm_shuffle_ps(v3, v3, _MM_SHUFFLE(1,0,1,0));
  v2 = _mm_shuffle_ps(v3, v3, _MM_SHUFFLE(3,2,3,2));
  v7 = _mm_sub_ps(v1, v2); // den,den2,den,den2

  // if (fabs(den) < 10e-10) den = 1.f;
  // if (fabs(den2) < 10e-10) den2 = 1.f;
  // abs value of den
  v6 = _mm_andnot_ps(v_sign_mask, v7);
  
  // den = den & mask + 1 & ~mask
  v1 = _mm_cmplt_ps(v6, v_compValue); // v1 is mask of denX < 10e-10
  v2 = _mm_andnot_ps(v1, v7);
  v3 = _mm_and_ps(v1, v_1s);
  v7 = _mm_add_ps(v2, v3);

  // b->m_tx = (b->m_sxz * b->m_s0  - b->m_sx  * b->m_sz) / den;
  // b->m_ty = (b->m_uyz * b->m_u0  - b->m_uy  * b->m_uz) / den2;
  // b->m_x0 = (b->m_sx  * b->m_sz2 - b->m_sxz * b->m_sz) / den;
  // b->m_y0 = (b->m_uy  * b->m_uz2 - b->m_uyz * b->m_uz) / den2;

  v1 = _mm_shuffle_ps(v_sxz, v_sx, _MM_SHUFFLE(2,0,2,0));
  v2 = _mm_shuffle_ps(v_s0, v_sxz, _MM_SHUFFLE(3,1,2,0));
  v3 = _mm_mul_ps(v1, v2);
  v4 = _mm_shuffle_ps(v_sx, v_sxz, _MM_SHUFFLE(2,0,2,0));
  v5 = _mm_shuffle_ps(v_sx, v_sx, _MM_SHUFFLE(3,1,3,1));
  v6 = _mm_mul_ps(v4, v5);

  v1 = _mm_sub_ps(v3, v6);
  v2 = _mm_div_ps(v1, v7);

  // Store final result
  _mm_storeu_ps(&(m_tx), v2);
}

//=========================================================================
// Return the covariance matrix of the last fit at the specified z
//=========================================================================
Gaudi::TrackSymMatrix PrPixelTrack::covariance(const float z) const {
  // covariance in intrinsics (SSE, SSE2)
  __m128 v1, v2, v3, v4, v5, v6, v7;
  float vec_cov[8];
  Gaudi::TrackSymMatrix cov;

  // const float m00 = m_s0;
  // const float m11 = m_u0;
  // const float m20 = m_sz - z * m_s0;
  // const float m31 = m_uz - z * m_u0;
  // const float m22 = m_sz2 - 2 * z * m_sz + z * z * m_s0;
  // const float m33 = m_uz2 - 2 * z * m_uz + z * z * m_u0;
  // const float den20 = 1.0 / (m22 * m00 - m20 * m20);
  // const float den31 = 1.0 / (m33 * m11 - m31 * m31);

  v7 = _mm_set1_ps(z);
  v1 = _mm_shuffle_ps(v_s0, v_sx, _MM_SHUFFLE(3,1,2,0)); // s0,u0,sz,uz
  v1 = _mm_mul_ps(v1, v7); // z*s0,z*u0,z*sz,z*uz

  v2 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(1,0,1,0)); // z*s0,z*u0,X,X
  v3 = _mm_shuffle_ps(v7, v_2s, _MM_SHUFFLE(0,0,0,0)); // z,z,2,2
  v1 = _mm_mul_ps(v3, v1); // z*z*s0, z*z*u0, 2*z*sz, 2*z*uz

  v3 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(1,0,0,0)); // X, X, z*z*s0, z*z*u0
  v1 = _mm_sub_ps(v1, v3); // X, X, 2*z*s0 - z*z*sz, 2*z*u0 - z*z*uz

  v1 = _mm_shuffle_ps(v2, v1, _MM_SHUFFLE(3,2,1,0));      // z*s0, z*u0, 2*z*s0 + z*z*sz, 2*z*u0 + z*z*uz
  v4 = _mm_shuffle_ps(v_sx, v_sxz, _MM_SHUFFLE(3,1,3,1)); // sz, uz, sz2, uz2

  v1 = _mm_sub_ps(v4, v1); // m20, m31, m22, m33

  // float den20 = m22 * m00 - m20 * m20;
  // float den31 = m33 * m11 - m31 * m31;

  v2 = _mm_shuffle_ps(v_s0, v1, _MM_SHUFFLE(1,0,2,0)); // m00, m11, m20, m31
  v3 = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(3,2,3,2)); // m22, m33, m20, m31
  v2 = _mm_mul_ps(v3, v2); // m22 * m00, m33 * m11, m20 * m20, m31 * m31

  v3 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3,2,3,2)); // m20 * m20, m31 * m31, X, X
  v2 = _mm_sub_ps(v2, v3); // den20, den31, X, X
  v3 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,0,1,0)); // den20, den31, den20, den31

  // Use this technique for sign
  // v6 = _mm_xor_ps(v_sign_mask, v7);

  v4 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(1,0,1,0)); // m20, m31, X, X
  v4 = _mm_xor_ps(v_sign_mask, v4); // -m20, -m31, X, X
  v6 = _mm_div_ps(v4, v3); // -m20/den20, -m31/den31

  v5 = _mm_shuffle_ps(v_s0, v1, _MM_SHUFFLE(3,2,2,0)); // m00, m11, m22, m33
  v7 = _mm_div_ps(v5, v3); // m00/den20, m11/den31, m22/den20, m33/den31
  
  _mm_storeu_ps(&(vec_cov[0]), v7);
  cov(2,2) = vec_cov[0];
  cov(3,3) = vec_cov[1];
  cov(0,0) = vec_cov[2];
  cov(1,1) = vec_cov[3];

  _mm_storeu_ps(&(vec_cov[4]), v6);
  cov(2,0) = vec_cov[4];
  cov(3,1) = vec_cov[5];

  // v0 = _mm_shuffle_ps(v6, v6, _MM_SHUFFLE(1, 1, 1, 1));
  // _mm_store_ss(&(cov(2,0)), v6);
  // _mm_store_ss(&(cov(3,1)), v0);

  cov(4,4) = 1.f;
  return cov;
}

//===============================================================================

namespace {

/// Helper function to filter one hit
inline float filter(const float z, float &x, float &tx, float &covXX,
                    float &covXTx, float &covTxTx, const float zhit, 
                    const float xhit, const float whit) {
  // compute the prediction
  const float dz = zhit - z;
  const float predx = x + dz * tx;

  const float dz_t_covTxTx = dz * covTxTx;
  const float predcovXTx = covXTx + dz_t_covTxTx;
  const float dx_t_covXTx = dz * covXTx;

  const float predcovXX = covXX + 2 * dx_t_covXTx + dz * dz_t_covTxTx;
  const float predcovTxTx = covTxTx;
  // compute the gain matrix
  const float R = 1.0 / (1.0 / whit + predcovXX);
  const float Kx = predcovXX * R;
  const float KTx = predcovXTx * R;
  // update the state vector
  const float r = xhit - predx;
  x = predx + Kx * r;
  tx = tx + KTx * r;
  // update the covariance matrix. we can write it in many ways ...
  covXX /*= predcovXX  - Kx * predcovXX */ = (1 - Kx) * predcovXX;
  covXTx /*= predcovXTx - predcovXX * predcovXTx / R */ = (1 - Kx) * predcovXTx;
  covTxTx = predcovTxTx - KTx * predcovXTx;
  // return the chi2
  return r * r * R;
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
float PrPixelTrack::fitKalman(LHCb::State &state, const int direction,
                              const float noise2PerLayer) const {

  // assume the hits are sorted, but don't assume anything on the direction of
  // sorting
  int firsthit = 0;
  int lasthit = m_hits.size() - 1;
  int dhit = +1;
  if ((m_hits[lasthit]->z() - m_hits[firsthit]->z()) * direction < 0) {
    std::swap(firsthit, lasthit);
    dhit = -1;
  }

  // we filter x and y simultaneously but take them uncorrelated.
  // filter first the first hit.

  const PrPixelHit *hit = m_hits[firsthit];
  float x = hit->x();
  float tx = m_tx;
  float y = hit->y();
  float ty = m_ty;
  float z = hit->z();

  // initialize the covariance matrix
  float covXX = 1 / hit->wx();
  float covYY = 1 / hit->wy();
  float covXTx = 0;  // no initial correlation
  float covYTy = 0;
  float covTxTx = 1;  // randomly large error
  float covTyTy = 1;

  // add remaining hits
  float chi2(0);
  for (int i = firsthit + dhit; i != lasthit + dhit; i += dhit) {
    hit = m_hits[i];
    // add the noise
    covTxTx += noise2PerLayer;
    covTyTy += noise2PerLayer;
    // filter X
    chi2 +=
        filter(z, x, tx, covXX, covXTx, covTxTx, hit->z(), hit->x(), hit->wx());
    // filter Y
    chi2 +=
        filter(z, y, ty, covYY, covYTy, covTyTy, hit->z(), hit->y(), hit->wy());
    // update z (not done in the filter, since needed only once)
    z = hit->z();
  }

  // add the noise at the last hit
  covTxTx += noise2PerLayer;
  covTyTy += noise2PerLayer;

  // finally, fill the state
  state.setX(x);
  state.setY(y);
  state.setZ(z);
  state.setTx(tx);
  state.setTy(ty);
  state.covariance()(0, 0) = covXX;
  state.covariance()(0, 2) = covXTx;
  state.covariance()(2, 2) = covTxTx;
  state.covariance()(1, 1) = covYY;
  state.covariance()(1, 3) = covYTy;
  state.covariance()(3, 3) = covTyTy;
  return chi2;
}
