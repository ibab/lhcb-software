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
PrVLTrack::PrVLTrack(  ) :
    m_s0(0.),
    m_sr(0.),
    m_sz(0.),
    m_srz(0.),
    m_sz2(0.),
    m_zone(0),
    m_backward(false),
    m_r0(-999.), m_tr(-999.),
    m_r0Err2(999.), m_trErr2(999.),
    m_nbUsedRHits(0),
    m_missedSensors(-1),
    m_valid(true),
    m_x0(0.), m_y0(0.),
    m_tx(0.), m_ty(0.),
    m_qFactor(0.),
    m_sa2(0.), m_sa2z(0.), m_sa2z2(0.),
    m_sab(0.), m_sabz(0.), m_sabz2(0.),
    m_sb2(0.), m_sb2z(0.), m_sb2z2(0.),
    m_sac(0.), m_sacz(0.), 
    m_sbc(0.), m_sbcz(0.) {
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
  const double r = hit->rGlobal();
  m_s0 += w;
  m_sr += w * r;
  m_sz += w * z;
  m_srz += w * r * z;
  m_sz2 += w * z * z;
  if (m_rHits.size() > 2) {
    double den = m_sz2 * m_s0 - m_sz * m_sz;
    if (fabs(den) < 1.e-9) den = 1.;
    m_tr = (m_srz * m_s0  - m_sr  * m_sz) / den;
    m_r0 = (m_sr  * m_sz2 - m_srz * m_sz) / den;
    m_trErr2 = m_s0 / den;
    m_r0Err2 = m_sz2 / den;
  }

}

//============================================================================
/// Add a hit to the track. Update fit parameters
//============================================================================
void PrVLTrack::removeRHit(PrVLHit* hit) {

  // Remove the hit.
  m_rHits.erase(std::remove(m_rHits.begin(), m_rHits.end(), hit), m_rHits.end()); 
  if (0 != hit->nUsed()) --m_nbUsedRHits;
  const double z = hit->z();
  const double w = hit->weight();
  const double r = hit->rGlobal();
  m_s0 -= w;
  m_sr -= w * r;
  m_sz  -= w * z;
  m_srz -= w * r * z;
  m_sz2 -= w * z * z;
  if (m_rHits.size() > 2) {
    double den = (m_sz2 * m_s0 - m_sz * m_sz);
    if (fabs(den) < 1.e-9) den = 1.;
    m_tr = (m_srz * m_s0  - m_sr  * m_sz) / den;
    m_r0 = (m_sr  * m_sz2 - m_srz * m_sz) / den;
    m_trErr2 = m_s0 / den;
    m_r0Err2 = m_sz2 / den;
  }
  
}

//============================================================================
/// Return the interpolated radius
//============================================================================
double PrVLTrack::rInterpolated(double z) {

  PrVLHits::const_iterator itF, itN, itNN;
  if (m_rHits.front()->z() > m_rHits.back()->z()) {
    itN = m_rHits.begin();
    itF = itN + 1;
    for (itNN = itF + 1; m_rHits.end() != itNN; ++itNN) {
      if ((*itF)->z() < z) break;
      itN = itF;
      itF = itNN;
    }
  } else {
    itN = m_rHits.begin();
    itF = itN + 1;
    for (itNN = itF + 1; m_rHits.end() != itNN; ++itNN) {
      if ((*itF)->z() > z) break;
      itN = itF;
      itF = itNN;
    }
  }
  double zRatio = (z - (*itN)->z()) / ((*itF)->z( ) - (*itN)->z());
  return (zRatio * (*itF)->rGlobal() + (1. - zRatio) * (*itN)->rGlobal());

}

//=========================================================================
/// Initialize a track from an existing one
//=========================================================================
void PrVLTrack::setPhiClusters(PrVLTrack& track,
                               double cosPhi, double sinPhi,
                               PrVLHits::const_iterator it1, 
                               PrVLHits::const_iterator it2) {
                             
  m_rHits.clear();
  PrVLHits::const_iterator ith;
  for (ith = track.rHits().begin(); ith != track.rHits().end(); ++ith) {
    m_rHits.push_back(*ith);
  }
  m_zone = track.zone();
  m_missedSensors = track.missedSensors();
  m_backward = track.backward();  

  m_phiHits.clear();
  m_qFactor = -1.;
  double sumGlobal = 0.; 
  for (ith = it1; ith != it2; ++ith) {
    sumGlobal += (*ith)->dGlobal();
    m_phiHits.push_back(*ith);
  }
  std::sort(m_phiHits.begin(), m_phiHits.end(), PrVLHit::DecreasingByZ());

  const double sinDPhi = sumGlobal / m_phiHits.size();
  const double cosDPhi = sqrt(1. - sinDPhi * sinDPhi); 
  const double xStart = 1000. * (cosPhi * cosDPhi - sinPhi * sinDPhi);
  const double yStart = 1000. * (cosPhi * sinDPhi + sinPhi * cosDPhi);
  for (ith = m_rHits.begin(); ith != m_rHits.end(); ++ith) {
    (*ith)->setStartingPoint(xStart, yStart);
  }
  fitTrack();

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
  m_zone = track.zone();
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
  m_zone = zone;
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
  
  double den = e1 * d2 - d1 * e2;
  if (0 != den) den = 1. / den;
  m_ty = (d1 * ec - e1 * dc) * den;
  m_y0 = (e2 * dc - d2 * ec) * den;
  m_tx = - (ac + a2 * m_y0 + a3 * m_ty) / a1;
  m_x0 = - (m_sac + m_sa2z * m_tx + m_sab * m_y0 + m_sabz * m_ty) / m_sa2;

}

//============================================================================
/// Fit, remove highest Chi2 in phi until OK or less than minExpected
//============================================================================
bool PrVLTrack::removeWorstMultiple(double maxChi2, unsigned int minExpected) {

  PrVLHits::iterator ith;
  int nbDiffSensors = 0;
  double highest = 1000.;
  while (m_phiHits.size() >= minExpected) {
    highest = -1.;
    PrVLHits::iterator worst = m_phiHits.end();
    unsigned int prevSensor = 999;
    for (ith = m_phiHits.begin(); ith != m_phiHits.end(); ++ith) {
      unsigned int nextSensor = 999;
      unsigned int mySensor = (*ith)->sensor();
      if (ith != m_phiHits.end() - 1) {
        nextSensor = (*(ith + 1))->sensor();
      }
      if (mySensor == prevSensor || mySensor == nextSensor) {
        const double c2 = chi2(*ith);
        if (c2 > highest) {
          highest = c2;
          worst = ith;
        }
      }
      if (mySensor != prevSensor) ++nbDiffSensors;
      prevSensor = mySensor;
    }
    if (nbDiffSensors < 3) return false;
    if (highest < 0.) break;
    subtractToFit(*worst);
    m_phiHits.erase(worst);
  }
  solve();

  // Now filter the rest, i.e. sensors with a single hit
  while (m_phiHits.size() >= minExpected) {
    highest = -1.;
    PrVLHits::iterator worst = m_phiHits.end();
    for (ith = m_phiHits.begin(); ith != m_phiHits.end(); ++ith) {
      const double chi2 = (*ith)->chi2(m_x0 + (*ith)->z() * m_tx, 
                                       m_y0 + (*ith)->z() * m_ty);
      if (chi2 > highest) {
        highest = chi2;
        worst = ith;
      }
    }
    if (highest < maxChi2) break;
    if (m_phiHits.size() < minExpected) break;
    subtractToFit(*worst);
    m_phiHits.erase(worst);
    solve();
  }
  return highest < maxChi2;
  
}

//=========================================================================
//  Fit, remove highest Chi2 in R and phi until OK
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
void PrVLTrack::addPhiCluster(PrVLHit* hit) {

  m_phiHits.push_back(hit);
  addToFit(hit);
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
    const double dist = fabs(rPred - (*ith)->rLocal());
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
