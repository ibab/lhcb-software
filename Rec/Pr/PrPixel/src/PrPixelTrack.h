#ifndef PRPIXELTRACK_H
#define PRPIXELTRACK_H 1

// LHCb
#include "Event/StateVector.h"
// Local
#include "PrPixelHit.h"

namespace LHCb {
class State;
}

/** @class PrPixelTrack PrPixelTrack.h
 *  Working tracks, used inside the PrPixelTracking
 *
 *  @author Olivier Callot
 *  @date   2012-01-06
 */

class PrPixelTrack {

 public:
  /// Standard constructor
  PrPixelTrack();
  /// Destructor
  virtual ~PrPixelTrack() {}
  /// Start a seed track from the two given hits
  void set(PrPixelHit *h1, PrPixelHit *h2) {
    m_hits.clear();
    m_hits.push_back(h1);
    m_hits.push_back(h2);
  }

  /// Return the list of hits on this track.
  PrPixelHits &hits() { return m_hits; }
  /// Add a given hit to this track
  void addHit(PrPixelHit *hit) { m_hits.push_back(hit); }

  // Mark the hits of this track as being associated
  void tagUsedHits() {
    for (auto it = m_hits.begin(), end = m_hits.end(); it != end; ++it) {
      (*it)->setUsed(true);
    }
  }
  // Count unassociated hits for this track
  unsigned int nbUnused() const {
    unsigned int nn = 0;
    for (auto it = m_hits.cbegin(), end = m_hits.cend(); it != end; ++it) {
      if (!(*it)->isUsed()) ++nn;
    }
    return nn;
  }

  /// Chi2 / degrees-of-freedom of straight-line fit
  double chi2() const {
    double ch = 0.;
    int nDoF = -4;
    for (auto it = m_hits.cbegin(), end = m_hits.cend(); it != end; ++it) {
      ch += chi2(*it);
      nDoF += 2;
    }
    return ch / nDoF;
  }
  /// Chi2 constribution from a given hit
  double chi2(PrPixelHit *hit) const {
    return hit->chi2(m_x0 + m_tx * hit->z(), m_y0 + m_ty * hit->z());
  }
  /// Position at given z from straight-line fit
  double xAtZ(const double z) const { return m_x0 + m_tx * z; }
  double yAtZ(const double z) const { return m_y0 + m_ty * z; }

  /// Create a track state vector: position and the slopes: dx/dz and dy/dz
  LHCb::StateVector state(const double z) const {
    LHCb::StateVector temp;
    temp.setX(m_x0 + z * m_tx);
    temp.setY(m_y0 + z * m_ty);
    temp.setZ(z);
    temp.setTx(m_tx);
    temp.setTy(m_ty);
    temp.setQOverP(0.);  // Q/P is Charge/Momentum ratio
    return temp;
  }

  // Calculate the z-pos. where the track passes closest to the beam
  double zBeam() const {
    return -(m_x0 * m_tx + m_y0 * m_ty) / (m_tx * m_tx + m_ty * m_ty);
  }

  Gaudi::TrackSymMatrix covariance(const double z) const;

  /// Perform a straight-line fit.
  void fit();
 
  /// Fit with a K-filter with scattering. Return the chi2
  double fitKalman(LHCb::State &state, int direction,
                   double noisePerLayer) const;

  // Number of hits assigned to the track
  unsigned int size(void) const { return m_hits.size(); }

 private:
  /// List of pointers to hits
  PrPixelHits m_hits;
  /// Straight-line fit parameters
  double m_x0;
  double m_tx;
  double m_y0;
  double m_ty;
  /// Sums for the x-slope fit
  double m_s0;
  double m_sx;
  double m_sz;
  double m_sxz;
  double m_sz2;
  /// Sums for the y-slope fit
  double m_u0;
  double m_uy;
  double m_uz;
  double m_uyz;
  double m_uz2;

};

typedef std::vector<PrPixelTrack> PrPixelTracks;  // vector of tracks

#endif  // PRPIXELTRACK_H
