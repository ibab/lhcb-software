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
    m_hits = {h1, h2};
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
  float chi2() const {
    float ch = 0.;
    int nDoF = -4;
    for (auto it = m_hits.cbegin(), end = m_hits.cend(); it != end; ++it) {
      ch += chi2(*it);
      nDoF += 2;
    }
    return ch / nDoF;
  }
  /// Chi2 constribution from a given hit
  float chi2(PrPixelHit *hit) const {
    return hit->chi2(m_x0 + m_tx * hit->z(), m_y0 + m_ty * hit->z());
  }
  /// Position at given z from straight-line fit
  float xAtZ(const float z) const { return m_x0 + m_tx * z; }
  float yAtZ(const float z) const { return m_y0 + m_ty * z; }

  /// Create a track state vector: position and the slopes: dx/dz and dy/dz
  LHCb::StateVector state(const float z) const {
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
  float zBeam() const {
    return -(m_x0 * m_tx + m_y0 * m_ty) / (m_tx * m_tx + m_ty * m_ty);
  }

  Gaudi::TrackSymMatrix covariance(const float z) const;

  /// Perform a straight-line fit.
  void fit();
 
  /// Fit with a K-filter with scattering. Return the chi2
  float fitKalman(LHCb::State &state, const int direction,
                  const float noisePerLayer) const;

  // Number of hits assigned to the track
  unsigned int size(void) const { return m_hits.size(); }

 private:
  /// List of pointers to hits
  PrPixelHits m_hits;
  /// Straight-line fit parameters
  float m_tx;
  float m_ty;
  float m_x0;
  float m_y0;

  /// Sums for the x-slope fit
  __m128 v_sx, v_sxz, v_s0;
  /// Constants
  __m128 v_compValue, v_1s, v_2s, v_sign_mask;
};

typedef std::vector<PrPixelTrack> PrPixelTracks;  // vector of tracks

#endif  // PRPIXELTRACK_H
