#ifndef PATPIXELTRACK_H 
#define PATPIXELTRACK_H 1

// LHCb
#include "Event/StateVector.h"
// Local 
#include "PatPixelHit.h"

namespace LHCb {
  class State;
}

/** @class PatPixelTrack PatPixelTrack.h
 *  Working tracks, used inside the PatPixelTracking
 *
 *  @author Olivier Callot
 *  @date   2012-01-06
 */

class PatPixelTrack {

public: 
  /// Standard constructor
  PatPixelTrack(); 
  /// Destructor
  virtual ~PatPixelTrack() {}
  /// Start a seed track from the two given hits
  void set(PatPixelHit* h1, PatPixelHit* h2);
  // Set(clear) "a-backward-track" flag
  void setBackward(const bool flag) {m_backward = flag;}
  bool backward() const {return m_backward;}

  // Return the list of hits on this track.
  PatPixelHits& hits() {return m_hits;}
  // Add a given hit to this track
  void addHit(PatPixelHit* hit);                                
  // Remove a given hit from this track
  void removeHit(PatPixelHit* hit);
  // Mark the hits of this track as being associated
  void tagUsedHits() {
    PatPixelHits::iterator ith;
    for (ith = m_hits.begin(); m_hits.end() != ith; ++ith) {
      (*ith)->setUsed(true); 
    }
  }
  // Count unassociated hits for this track
  int nbUnused() {
    unsigned int nn = 0;
    PatPixelHits::iterator ith;
    for (ith = m_hits.begin(); m_hits.end() != ith; ++ith) {
      if (!(*ith)->isUsed()) ++nn;
    }
    return nn; 
  }

  // Check that for a 3-hit track, there are actually 3 different sensors used.
  bool all3SensorsAreDifferent() const { 
    if (m_hits[0]->module() == m_hits[1]->module()) return false;
    if (m_hits[0]->module() == m_hits[2]->module()) return false;
    if (m_hits[1]->module() == m_hits[2]->module()) return false;
    return true; 
  }

  int firstModule() const {return m_hits.front()->module();}
  int lastModule()  const {return m_hits.back()->module();}

  /// Chi2 / degrees-of-freedom of straight-line fit
  double chi2() {
    double ch = 0.;
    int nDoF = -4;
    PatPixelHits::const_iterator ith;
    for (ith = m_hits.begin(); m_hits.end() != ith; ++ith) {
      ch += chi2(*ith);
      nDoF += 2;
    }
    return ch / nDoF;
  }
  /// Chi2 constribution from a given hit
  double chi2(PatPixelHit* hit) {
    return hit->chi2(m_x0 + m_tx * hit->z(), m_y0 + m_ty * hit->z()); 
  }
  /// Position at given z from straight-line fit
  double xAtZ(const double z) const {
    return m_x0 + m_tx * z;
  }
  double yAtZ(const double z) const {
    return m_y0 + m_ty * z;
  } 

  /// Create a track state vector: position and the slopes: dx/dz and dy/dz
  LHCb::StateVector state(double z) {
    LHCb::StateVector temp;
    temp.setX(m_x0 + z * m_tx);
    temp.setY(m_y0 + z * m_ty);
    temp.setZ(z);
    temp.setTx(m_tx);
    temp.setTy(m_ty);
    temp.setQOverP(0.); // Q/P is Charge/Momentum ratio
    return temp; 
  }

  // Calculate the z-pos. where the track passes closest to the beam
  double zBeam() const {
    return -(m_x0 * m_tx + m_y0 * m_ty) / (m_tx * m_tx + m_ty * m_ty); 
  }

  Gaudi::TrackSymMatrix covariance(double z);

  /// Fit with a K-filter with scattering. Return the chi2
  double fitKalman(LHCb::State& state, int direction, double noisePerLayer) const;

  // Number of hits assigned to the track
  int size(void) const {return m_hits.size();} 

private:
  /// Backward or forward track
  bool m_backward;     
  /// List of pointers to hits
  PatPixelHits m_hits;        
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

  /// Calculate the fit from the x/y-slope sums
  void solve();
};

typedef std::vector<PatPixelTrack> PatPixelTracks; // vector of tracks

#endif // PATPIXELTRACK_H
