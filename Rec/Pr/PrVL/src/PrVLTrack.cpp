#include <iostream>
// Local
#include "PrVLTrack.h"

/** @file PrVLTrack.cpp
 *
 *  Implementation of class : PrVLTrack
 *
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrVLTrack::PrVLTrack() :
    m_rzone(0), m_overlap(false), m_backward(false),
    m_s0(0.), m_sr(0.), m_sz(0.), m_srz(0.), m_sz2(0.),
    m_r0(-999.), m_tr(-999.),  m_r0Err2(999.), m_trErr2(999.),
    m_nbUsedRHits(0),
    m_missedSensors(-1),
    m_valid(true),
    m_x0(0.), m_y0(0.), m_tx(0.), m_ty(0.), 
    m_qFactor(0.),
    m_sa2(0.), m_sa2z(0.), m_sa2z2(0.),
    m_sab(0.), m_sabz(0.), m_sabz2(0.),
    m_sb2(0.), m_sb2z(0.), m_sb2z2(0.),
    m_sac(0.), m_sacz(0.), m_sbc(0.), m_sbcz(0.) {
  m_rHits.reserve(20);
  m_phiHits.reserve(20);
}

//============================================================================
/// Add a hit to the track. Update fit parameters
//============================================================================
void PrVLTrack::addRHit(PrVLHit* hit) {
  m_rHits.push_back(hit);
  if (0 != hit->nUsed()) ++m_nbUsedRHits;
  const double z = hit->z();
  const double w = hit->weight();
  const double r = hit->r();
  m_s0 += w;
  m_sr += w * r;
  m_sz += w * z;
  m_srz += w * r * z;
  m_sz2 += w * z * z;
  if (m_rHits.size() > 2) {
    double det = m_sz2 * m_s0 - m_sz * m_sz;
    if (fabs(det) < 1.e-9) det = 1.;
    m_tr = (m_srz * m_s0  - m_sr  * m_sz) / det;
    m_r0 = (m_sr  * m_sz2 - m_srz * m_sz) / det;
    m_trErr2 = m_s0 / det;
    m_r0Err2 = m_sz2 / det;
  }

}

//============================================================================
/// Add a hit to the track. Update fit parameters
//============================================================================
void PrVLTrack::removeRHit(PrVLHit* hit) {

  // Remove the hit.
  PrVLHits::iterator it = std::find(m_rHits.begin(), m_rHits.end(), hit);
  if (m_rHits.end() == it) return;
  m_rHits.erase(it);
  if (hit->nUsed() > 0) --m_nbUsedRHits;
  const double z = hit->z();
  const double w = hit->weight();
  const double r = hit->r();
  m_s0 -= w;
  m_sr -= w * r;
  m_sz  -= w * z;
  m_srz -= w * r * z;
  m_sz2 -= w * z * z;
  if (m_rHits.size() > 2) {
    double det = m_sz2 * m_s0 - m_sz * m_sz;
    if (fabs(det) < 1.e-9) det = 1.;
    m_tr = (m_srz * m_s0  - m_sr  * m_sz) / det;
    m_r0 = (m_sr  * m_sz2 - m_srz * m_sz) / det;
    m_trErr2 = m_s0 / det;
    m_r0Err2 = m_sz2 / det;
  }
  
}

//============================================================================
/// Return the (interpolated) R sensor zone
//============================================================================
unsigned int PrVLTrack::rZone(double z, bool& right) {

  PrVLHits::const_iterator it0 = m_rHits.begin();
  PrVLHits::const_iterator it1 = it0 + 1;
  PrVLHits::const_iterator it;
  if (m_rHits.front()->z() > m_rHits.back()->z()) {
    for (it = it1 + 1; it != m_rHits.end(); ++it) {
      if ((*it1)->z() < z) break;
      it0 = it1;
      it1 = it;
    }
  } else {
    for (it = it1 + 1; it != m_rHits.end(); ++it) {
      if ((*it1)->z() > z) break;
      it0 = it1;
      it1 = it;
    }
  }
  const double zRatio = (z - (*it0)->z()) / ((*it1)->z() - (*it0)->z());
  if (zRatio < 0.5) {
    right = (*it0)->right();
    return (*it0)->zone();
  }
  right = (*it1)->right();
  return (*it1)->zone();

}

//============================================================================
/// Calculated the interpolated radius
//============================================================================
double PrVLTrack::rInterpolated(double z) {
  
  PrVLHits::const_iterator it0 = m_rHits.begin();
  PrVLHits::const_iterator it1 = it0 + 1;
  PrVLHits::const_iterator it;
  if (m_rHits.front()->z() > m_rHits.back()->z()) {
    for (it = it1 + 1; it != m_rHits.end(); ++it) {
      if ((*it1)->z() < z) break;
      it0 = it1;
      it1 = it;
    }
  } else {
    for (it = it1 + 1; it != m_rHits.end(); ++it) {
      if ((*it1)->z() > z) break;
      it0 = it1;
      it1 = it;
    }
  }
  double zRatio = (z - (*it0)->z()) / ((*it1)->z( ) - (*it0)->z());
  return (zRatio * (*it1)->r() + (1. - zRatio) * (*it0)->r());

}

//=========================================================================
// Initialize a track from an existing one
//=========================================================================
void PrVLTrack::setPhiClusters(PrVLTrack& track,
                               double x0, double tx, double y0, double ty,
                               PrVLHit* h1, PrVLHit* h2, PrVLHit* h3) {

  m_rHits.clear();
  PrVLHits::const_iterator ith;
  for (ith = track.rHits().begin(); ith != track.rHits().end(); ++ith) {
    (*ith)->setStartingPoint(x0 + tx * (*ith)->z(), 
                             y0 + ty * (*ith)->z());
    m_rHits.push_back(*ith);
  }
  m_rzone = track.rZone();
  m_missedSensors = track.missedSensors();
  m_backward = track.backward();

  m_phiHits.clear();
  m_phiHits.push_back(h1);
  m_phiHits.push_back(h2);
  m_phiHits.push_back(h3);
  std::sort(m_phiHits.begin(), m_phiHits.end(), PrVLHit::DecreasingByZ());
  fitTrack();
  
}

//============================================================================
/// Initialize a track from 4 clusters
//============================================================================
void PrVLTrack::setPhiClusters(PrVLHit* r1, int zone, 
                               PrVLHit* h1, PrVLHit* h2, PrVLHit* h3) {
                             
  m_rHits.clear();
  m_rHits.push_back(r1);
  m_rzone = zone;
  m_missedSensors = 0;
  m_backward = false;
  m_phiHits.clear();
  m_phiHits.push_back(h1);
  m_phiHits.push_back(h2);
  m_phiHits.push_back(h3);
  fitTrack();
  
}

//=========================================================================
/// Fit the track, compute m_x0, m_tx, m_y0, m_ty.
//=========================================================================
void PrVLTrack::fitTrack() {

  m_sa2   = 0.; m_sa2z  = 0.; m_sa2z2 = 0.;
  m_sab   = 0.; m_sabz  = 0.; m_sabz2 = 0.;
  m_sb2   = 0.; m_sb2z  = 0.; m_sb2z2 = 0.;
  m_sac   = 0.; m_sacz  = 0.;
  m_sbc   = 0.; m_sbcz  = 0.;
  m_qFactor = -1.;
  
  PrVLHits::iterator ith;
  for (ith = m_rHits.begin(); ith != m_rHits.end(); ++ith) {
    addToFit(*ith);
  }
  for (ith = m_phiHits.begin(); ith != m_phiHits.end(); ++ith) {
    addToFit(*ith);
  }
  solve();
  
}

//=========================================================================
/// Solve the system of 4 equations to obtain the track parameters
//=========================================================================
void PrVLTrack::solve() {

  const double a1 = m_sa2z * m_sa2z - m_sa2 * m_sa2z2;
  const double a2 = m_sa2z * m_sab  - m_sa2 * m_sabz;
  const double a3 = m_sa2z * m_sabz - m_sa2 * m_sabz2;
  const double ac = m_sa2z * m_sac  - m_sa2 * m_sacz;
  
  const double b1 = m_sab * m_sa2z - m_sa2 * m_sabz;
  const double b2 = m_sab * m_sab  - m_sa2 * m_sb2;
  const double b3 = m_sab * m_sabz - m_sa2 * m_sb2z;
  const double bc = m_sab * m_sac  - m_sa2 * m_sbc;
  
  const double c1 = m_sabz * m_sa2z - m_sa2 * m_sabz2;
  const double c2 = m_sabz * m_sab  - m_sa2 * m_sb2z;
  const double c3 = m_sabz * m_sabz - m_sa2 * m_sb2z2;
  const double cc = m_sabz * m_sac  - m_sa2 * m_sbcz;
  
  const double d1 = b1 * a2 - a1 * b2;
  const double d2 = b1 * a3 - a1 * b3;
  const double dc = b1 * ac - a1 * bc;
  
  const double e1 = c1 * a2 - a1 * c2;
  const double e2 = c1 * a3 - a1 * c3;
  const double ec = c1 * ac - a1 * cc;
  
  double det = e1 * d2 - d1 * e2;
  if (0 != det) det = 1. / det;
  m_ty = (d1 * ec - e1 * dc) * det;
  m_y0 = (e2 * dc - d2 * ec) * det;
  m_tx = - (ac + a2 * m_y0 + a3 * m_ty) / a1;
  m_x0 = - (m_sac + m_sa2z * m_tx + m_sab * m_y0 + m_sabz * m_ty) / m_sa2;

}

//=========================================================================
// Remove hits highest Chi2 in R and Phi until OK
//=========================================================================
bool PrVLTrack::removeWorstRAndPhi(double maxChi2, unsigned int minExpected) {

  PrVLHits::iterator ith;
  bool worstIsPhi = true;
  updateRParameters();
  double highest = 2 * maxChi2;
  while (m_phiHits.size() + m_rHits.size() >= minExpected) {
    highest = -1.;
    PrVLHits::iterator worst = m_phiHits.end();
    for (ith = m_phiHits.begin(); ith != m_phiHits.end(); ++ith) {
      const double chi2 = (*ith)->chi2(m_x0 + (*ith)->z() * m_tx, 
                                       m_y0 + (*ith)->z() * m_ty);
      if (highest < chi2) {
        highest = chi2;
        worst = ith;
        worstIsPhi = true;
      }
    }
    for (ith = m_rHits.begin(); ith != m_rHits.end(); ++ith) {
      const double chi2 = (*ith)->chi2(m_x0 + (*ith)->z() * m_tx, 
                                       m_y0 + (*ith)->z() * m_ty);
      if (highest < chi2) {
        highest = chi2;
        worst = ith;
        worstIsPhi = false;
      }
    }
    // Exit if there is no bad hit.
    if (highest < maxChi2) break;
    subtractToFit(*worst);
    if (worstIsPhi) {
      m_phiHits.erase(worst);
      if (m_phiHits.size() < 3) break;
    } else {
      m_rHits.erase(worst);
      if (m_rHits.size() < 3) break;
    }
    updateRParameters();
  }
  return highest < maxChi2;
  
}

//============================================================================
/// Add another cluster
//============================================================================
bool PrVLTrack::addPhiCluster(PrVLHit* hit, double maxChi2) {

  // Avoid re-adding the same hit;
  if (std::find(m_phiHits.begin(), m_phiHits.end(), hit) != m_phiHits.end()) {
    return false;
  }

  if (chi2(hit) < maxChi2) {
    m_phiHits.push_back(hit);
    addToFit(hit);
    solve();
    return true;
  }
  return false;

}

//============================================================================
/// Add a Phi cluster to the track
//============================================================================
void PrVLTrack::addPhiHit(PrVLHit* hit) {

  m_phiHits.push_back(hit);
  addToFit(hit);
  solve();
  
}

//============================================================================
/// Remove a Phi cluster from the track
//============================================================================
void PrVLTrack::removePhiHit(PrVLHit* hit) {

  // Remove the hit.
  PrVLHits::iterator it = std::find(m_phiHits.begin(), m_phiHits.end(), hit);
  if (m_phiHits.end() == it) return;
  subtractToFit(hit);
  m_phiHits.erase(it);
  solve();

}

//=========================================================================
/// Add the closest R cluster of a list of hits
//=========================================================================
bool PrVLTrack::addBestRCluster(PrVLHits& hits, const double z, 
                                double maxChi2) {

  const double x = xAtZ(z);
  const double y = yAtZ(z);
  const double rPred = sqrt(x * x + y * y);
  PrVLHit* best = 0;
  PrVLHits::iterator ith;
  for (ith = hits.begin(); ith != hits.end(); ++ith) {
    const double dist = fabs(rPred - (*ith)->r());
    double chi2 = (*ith)->weight() * dist * dist;
    if (chi2 < maxChi2) {
      maxChi2 = chi2;
      best = *ith;
    }
  }
  if (!best) return false;                  
  m_rHits.push_back(best);
  best->setStartingPoint(m_x0 + best->z() * m_tx, 
                         m_y0 + best->z() * m_ty);
  addToFit(best);
  solve();
  updateRParameters();
  return true;
  
}

//=========================================================================
/// Return the covariance matrix of the last fit at the specified z
//=========================================================================
Gaudi::TrackSymMatrix PrVLTrack::covariance(double z) {

  Gaudi::TrackSymMatrix cov;
  cov(0, 0) = m_sa2;
  cov(1, 0) = m_sab;
  cov(1, 1) = m_sb2;

  cov(2, 0) = m_sa2z - z * m_sa2;
  cov(2, 1) = m_sabz - z * m_sab;
  cov(3, 0) = cov(2, 1);
  cov(3, 1) = m_sb2z - z * m_sb2;

  cov(2, 2) = m_sa2z2 - 2 * z * m_sa2z + z * z * m_sa2;
  cov(3, 2) = m_sabz2 - 2 * z * m_sabz + z * z * m_sab;
  cov(3, 3) = m_sb2z2 - 2 * z * m_sb2z + z * z * m_sb2;

  cov(4, 4) = 1.;

  if (!cov.InvertChol()) {
    std::cerr << "Choleski inversion failed" << endmsg;
  }
  return cov;
  
}
