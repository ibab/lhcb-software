// $Id: PatFwdTrackCandidate.h,v 1.6 2009-01-21 09:06:17 cattanem Exp $
#ifndef PATFWDTRACKCANDIDATE_H
#define PATFWDTRACKCANDIDATE_H 1

#include <algorithm>

// Include files
#include "Event/Track.h"
#include "PatKernel/PatForwardHit.h"

/** @class PatFwdTrackCandidate PatFwdTrackCandidate.h
 *  Local track candidate. Basically, the Velo track, list of PatTCoord
 *  and T-station track parameters
 *
 *  @author Olivier Callot
 *  @date   2005-07-06 Initial version
 *  @date   2007-08-20 Update for A-Team framework
 */

class PatFwdTrackCandidate final {
public:
  PatFwdTrackCandidate(const LHCb::Track *tr, PatFwdHits coords )
      : m_track{tr}, m_coords{std::move(coords)} {
    const LHCb::State* state = m_track->stateAt(LHCb::State::EndVelo);
    m_x0 = state->x();
    m_y0 = state->y();
    m_z0 = state->z();
    m_tx = state->tx();
    m_ty = state->ty();

    m_tx2 = m_tx * m_tx;
    m_ty2 = m_ty * m_ty;
    m_qOverP = state->qOverP();
  }
  PatFwdTrackCandidate(const LHCb::Track *tr ) : PatFwdTrackCandidate(tr,PatFwdHits{}) { m_coords.reserve(32); }
  template <typename I> 
  PatFwdTrackCandidate(const LHCb::Track *tr, I first, I last) : PatFwdTrackCandidate(tr,PatFwdHits{first,last}) {}

  void cleanCoords() {
    m_coords.erase(
        std::remove_if(std::begin(m_coords), std::end(m_coords),
                       [](const PatFwdHit *h) { return !h->isSelected(); }),
        std::end(m_coords));
  }

  ~PatFwdTrackCandidate() {};

  const LHCb::Track *track() const { return m_track; }
  double xStraight(double z) const { return m_x0 + m_tx * (z - m_z0); }
  double yStraight(double z) const { return m_y0 + m_ty * (z - m_z0); }
  double qOverP() const { return m_qOverP; }

  double slX() const { return m_tx; }
  double slY() const { return m_ty; }
  double slX2() const { return m_tx2; }
  double slY2() const { return m_ty2; }

  double sinTrack() const { return sqrt(1. - 1. / (1. + slX2() + slY2())); }

  PatFwdHits::const_iterator coordBegin()  const{ return m_coords.begin(); }
  PatFwdHits::const_iterator coordEnd() const { return m_coords.end(); }

  PatFwdHits::iterator coordBegin() { return m_coords.begin(); }
  PatFwdHits::iterator coordEnd() { return m_coords.end(); }

  PatFwdHits::iterator begin() { return m_coords.begin(); }
  PatFwdHits::iterator end() { return m_coords.end(); }
 
  PatFwdHits::const_iterator begin() const { return m_coords.begin(); }
  PatFwdHits::const_iterator end() const { return m_coords.end(); }

  void addCoord(PatFwdHit *coord) { m_coords.push_back(coord); }
  void addCoords(const PatFwdHits &coords) {
    addCoords(std::begin(coords), std::end(coords));
  }
  template <typename I> void addCoords(I first, I last) {
    m_coords.reserve(m_coords.size() + std::distance(first,last));
    m_coords.insert(m_coords.end(), first, last );

  }
  const PatFwdHits &coords() const { return m_coords; }
  PatFwdHits &coords() { return m_coords; }

  void setParameters(double ax, double bx, double cx, double dx, double ay,
                     double by) {
    m_x[0] = ax;
    m_x[1] = bx;
    m_x[2] = cx;
    m_x[3] = dx;

    m_y[0] = ay;
    m_y[1] = by;

    m_cosAfter = 1. / sqrt(1. + m_x[1] * m_x[1]);
    m_fitted = true;
  }

  bool fitted() const { return m_fitted; }

  void updateParameters(double dax, double dbx, double dcx, double ddx = 0.,
                        double day = 0., double dby = 0.) {
    m_x[0] += dax;
    m_x[1] += dbx;
    m_x[2] += dcx;
    m_x[3] += ddx;

    m_y[0] += day;
    m_y[1] += dby;

    m_cosAfter = 1. / sqrt(1. + m_x[1] * m_x[1]);
  }

  double x(double dz) const {
    return m_x[0] + dz * (m_x[1] + dz * (m_x[2] + dz * m_x[3]));
  }
  double y(double dz) const { return m_y[0] + dz * m_y[1]; }

  double xSlope(double dz) const {
    return m_x[1] + dz * (2 * m_x[2] + 3 * dz * m_x[3]);
  }

  double ySlope(double) const { return m_y[1]; }

  double xMagnet(double dz) const { return m_x[0] + dz * m_x[1]; }

  double cosAfter() const { return m_cosAfter; }

  double dSlope() const { return m_x[1] - m_tx; }

  int setSelectedCoords() {
    int nbSelected = 0;
    for (auto *hit : m_coords) {
      auto ignored = hit->isIgnored();
      hit->setSelected(!ignored);
      if (!ignored) ++nbSelected;
    }
    return nbSelected;
  }

  void setQuality(double quality) { m_quality = quality; }
  double quality() const { return m_quality; }

  void setChi2PerDoF(double chi2) { m_chi2PerDoF = chi2; }
  double chi2PerDoF() const { return m_chi2PerDoF; }

  void setNDoF(int nDoF) { m_nDoF = nDoF; }
  int nDoF() const { return m_nDoF; }

  double bx() const { return m_x[1]; }

  int nbIT() const { return m_nb[0]; }
  int nbOT() const { return m_nb[1]; }
  void setNbIT(int nb) { m_nb[0] = nb; }
  void setNbOT(int nb) { m_nb[1] = nb; }

  void setCand1stQuality(double cand1stquality) {
    m_candquality[0] = cand1stquality;
  }
  double cand1stquality() const { return m_candquality[0]; }

  void setCand2ndQuality(double cand2ndquality) {
    m_candquality[1] = cand2ndquality;
  }
  double cand2ndquality() const { return m_candquality[1]; }

private:
  const LHCb::Track *m_track;
  double m_x0;
  double m_y0;
  double m_z0;
  double m_tx;
  double m_ty;

  double m_qOverP;
  double m_tx2;
  double m_ty2;

  PatFwdHits m_coords;

  std::array<double,4>  m_x{{ 0., 0., 0., 0. }};
  std::array<double,2>  m_y{{ 0., 0. }};

  double m_cosAfter{ 1. };
  double m_quality{ 0. };
  std::array<double,2> m_candquality{{0.,0.}};
  std::array<int,2> m_nb{{ 0, 0 }}; // IT, OT

  double m_chi2PerDoF{ 0 };
  int m_nDoF{ 0 };
  bool m_fitted{ false };
};
#endif // PATFWDTRACKCANDIDATE_H
