#ifndef PRVL_TRACK_H 
#define PRVL_TRACK_H 1

#include "PrVLHit.h"
#include "Event/StateVector.h"

/** @class PrVLTrack PrVLTrack.h
 * 
 *  Internal representation of a track in PrVLTracking
 *
 */

class PrVLTrack {
public: 
  /// Constructor
  PrVLTrack();
  /// Destructor 
  virtual ~PrVLTrack() {} 

  bool valid() const {return m_valid;}
  void setValid(const bool flag) {m_valid = flag;}
  bool backward() const {return m_backward;}
  void setBackward(const bool flag) {m_backward = flag;} 
  unsigned int rZone() const {return m_rzone;}
  void setRZone(const unsigned int zone) {m_rzone = zone;}
  bool overlap() const {return m_overlap;}
  void setOverlap(const bool flag) {m_overlap = flag;}
  int missedSensors() const {return m_missedSensors;}
  void setMissedSensors(const int nMiss) {m_missedSensors = nMiss;}

  PrVLHits& rHits() {return m_rHits;}
  PrVLHits& phiHits() {return m_phiHits;}
  unsigned int nbRHits() const {return m_rHits.size();}
  unsigned int nbPhiHits() const {return m_phiHits.size();}

  double rPred(const double z) const {
    return m_r0 + z * m_tr;
  }
  double rErr2(double z) const { 
    const double dz = z - m_sz / m_s0; 
    return m_trErr2 * (1. + dz * dz); 
  }
  double rChi2() const {
    double chi2 = 0.;
    PrVLHits::const_iterator it;
    for (it = m_rHits.begin(); it != m_rHits.end(); ++it) {
      const double d = m_r0 + (*it)->z() * m_tr - (*it)->r();
      chi2 += (*it)->weight() * d * d;
    }
    return chi2 / (m_rHits.size() - 2.);
  }
  unsigned int rZone(double z, bool& right);
  double rInterpolated(double z);

  void addRHit(PrVLHit* hit);
  void removeRHit(PrVLHit* hit);
  void addPhiHit(PrVLHit* hit);
  void removePhiHit(PrVLHit* hit);

  unsigned int nbUsedRHits() const {return m_nbUsedRHits;}
  void tagUsedRHits() {
    PrVLHits::iterator it;
    for (it = m_rHits.begin(); it != m_rHits.end(); ++it) {
      (*it)->setUsed();
    }
  }
  void untagUsedRHits() {
    PrVLHits::iterator it;
    for (it = m_rHits.begin(); it != m_rHits.end(); ++it) {
      (*it)->unsetUsed();
    }
  }
  unsigned int nbUsedPhiHits() const {
    unsigned int nUsed = 0;
    PrVLHits::const_iterator it;
    for (it = m_phiHits.begin(); it != m_phiHits.end(); ++it) {
      if ((*it)->nUsed() > 0) ++nUsed;
    }
    return nUsed;
  }
  unsigned int nbUnusedPhiHits() const {
    unsigned int nUnused = 0;
    PrVLHits::const_iterator it;
    for (it = m_phiHits.begin(); it != m_phiHits.end(); ++it) {
      if (0 == (*it)->nUsed()) ++nUnused;
    }
    return nUnused;
  }
  void tagUsedPhiHits() {
    PrVLHits::iterator it;
    for (it = m_phiHits.begin(); it != m_phiHits.end(); ++it) {
      (*it)->setUsed();
    }
  }

  void setPhiClusters(PrVLTrack& track,
                      double x0, double tx, double y0, double ty,
                      PrVLHit* h1, PrVLHit* h2, PrVLHit* h3);
  void setPhiClusters(PrVLHit* r1, int zone, 
                      PrVLHit* h1, PrVLHit* h2, PrVLHit* h3);

  bool addPhiCluster(PrVLHit* hit, double maxChi2);
  void addPhiClusters(PrVLHits& hits) {
    PrVLHits::const_iterator it;
    for (it = hits.begin(); it != hits.end(); ++it) {
      m_phiHits.push_back(*it);
    }
    fitTrack();
  }

  void fitTrack();

  bool removeWorstRAndPhi(double maxChi2, unsigned int minPhi);

  void solve();

  double distance(PrVLHit* hit) {
    return hit->distance(m_x0 + hit->z() * m_tx, 
                         m_y0 + hit->z() * m_ty); 
  }
 
  double chi2(PrVLHit* hit) { 
    return hit->chi2(m_x0 + hit->z() * m_tx, 
                     m_y0 + hit->z() * m_ty); 
  }

  double xAtHit(PrVLHit* hit) {return m_x0 + m_tx * hit->z();}
  double yAtHit(PrVLHit* hit) {return m_y0 + m_ty * hit->z();}
  double xAtZ(const double z) {return m_x0 + m_tx * z;}
  double yAtZ(const double z) {return m_y0 + m_ty * z;}
  double rAtZ(const double z) {
    const double x = m_x0 + m_tx * z;
    const double y = m_y0 + m_ty * z;
    return sqrt(x * x + y * y);
  }
 
  bool addBestRCluster(PrVLHits& hits, const double z, double maxChi2); 

  void updateRParameters() {
    PrVLHits::iterator it;
    for (it = m_rHits.begin(); it != m_rHits.end(); ++it) {
      (*it)->setStartingPoint(xAtHit(*it), yAtHit(*it));
    }
    fitTrack();
  }

  void updatePhiWeights() {
    PrVLHits::iterator it;
    for (it = m_phiHits.begin(); it != m_phiHits.end(); ++it) {
      (*it)->setWeight(xAtHit(*it), yAtHit(*it));
    }
    fitTrack();
  }

  void addToFit(PrVLHit* hit) {
    m_qFactor = -1.;
    const double a = hit->a();
    const double b = hit->b();
    const double c = hit->c();
    const double z = hit->z();
    const double w = hit->weight();
    m_sa2   += w * a * a;
    m_sa2z  += w * a * a * z;
    m_sa2z2 += w * a * a * z * z;
    m_sab   += w * a * b;
    m_sabz  += w * a * b * z;
    m_sabz2 += w * a * b * z * z;
    m_sb2   += w * b * b;
    m_sb2z  += w * b * b * z;
    m_sb2z2 += w * b * b * z * z;
    m_sac   += w * a * c;
    m_sacz  += w * a * c * z;
    m_sbc   += w * b * c;
    m_sbcz  += w * b * c * z;
  }
  
  void subtractToFit(PrVLHit* hit) {
    m_qFactor = -1.;
    const double a = hit->a();
    const double b = hit->b();
    const double c = hit->c();
    const double z = hit->z();
    const double w = hit->weight();
    m_sa2   -= w * a * a;
    m_sa2z  -= w * a * a * z;
    m_sa2z2 -= w * a * a * z * z;
    m_sab   -= w * a * b;
    m_sabz  -= w * a * b * z;
    m_sabz2 -= w * a * b * z * z;
    m_sb2   -= w * b * b;
    m_sb2z  -= w * b * b * z;
    m_sb2z2 -= w * b * b * z * z;
    m_sac   -= w * a * c;
    m_sacz  -= w * a * c * z;
    m_sbc   -= w * b * c;
    m_sbcz  -= w * b * c * z;
  }

  double qFactor() {
    if (m_qFactor < 0.) {
      m_qFactor = 0.;
      PrVLHits::const_iterator it;
      for (it = m_rHits.begin(); it != m_rHits.end(); ++it) {
        m_qFactor += chi2(*it);
      }
      int nbUsed = 0;
      for (it = m_phiHits.begin(); it != m_phiHits.end(); ++it) {
        m_qFactor += chi2(*it);
        nbUsed += (*it)->nUsed();
      }
      m_qFactor /= (m_rHits.size() + m_phiHits.size() - 4.);
      m_qFactor += 2. * nbUsed / double(m_phiHits.size());
    }
    return m_qFactor;
  }

  LHCb::StateVector state(double z) {
    LHCb::StateVector temp;
    temp.setX(m_x0 + z * m_tx);
    temp.setY(m_y0 + z * m_ty);
    temp.setZ(z);
    temp.setTx(m_tx);
    temp.setTy(m_ty);
    temp.setQOverP(0.);
    return temp;
  }

  double zBeam() { 
    return -(m_x0 * m_tx + m_y0 * m_ty) / (m_tx * m_tx + m_ty * m_ty);
  }

  Gaudi::TrackSymMatrix covariance(double z);

  struct DecreasingByRLength {
    bool operator() (const PrVLTrack& lhs, const PrVLTrack& rhs) const {
      if (lhs.nbRHits() == rhs.nbRHits()) {
        return lhs.rChi2() < rhs.rChi2();
      } 
      return lhs.nbRHits() > rhs.nbRHits(); 
    }
  };

  struct DecreasingByPhiLength {
    bool operator() (const PrVLTrack& lhs, const PrVLTrack& rhs) const {
      if (lhs.nbPhiHits() == rhs.nbPhiHits()) {
        return lhs.m_qFactor < rhs.m_qFactor;
      }
      return lhs.nbPhiHits() > rhs.nbPhiHits(); 
    }
  };

private:
  unsigned int m_rzone;
  double m_overlap;
  bool m_backward;  
  double m_s0;
  double m_sr;
  double m_sz;
  double m_srz;
  double m_sz2;
  double m_r0;
  double m_tr;
  double m_r0Err2;
  double m_trErr2;

  int m_nbUsedRHits;
  PrVLHits m_rHits;
  int m_missedSensors;
  bool m_valid;

  PrVLHits m_phiHits;
  double m_x0;
  double m_y0;
  double m_tx;
  double m_ty;

  double m_qFactor;
  
  // Fit cumulative values
  double m_sa2;
  double m_sa2z;
  double m_sa2z2;
  double m_sab;
  double m_sabz;
  double m_sabz2;
  double m_sb2;
  double m_sb2z;
  double m_sb2z2;
  double m_sac;
  double m_sacz;
  double m_sbc;
  double m_sbcz;
};

typedef std::vector<PrVLTrack> PrVLTracks;

#endif
