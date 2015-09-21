// $Id: PatFwdTrackCandidate.h,v 1.6 2009-01-21 09:06:17 cattanem Exp $
#ifndef PATFWDTRACKCANDIDATE_H
#define PATFWDTRACKCANDIDATE_H 1

#include <algorithm>

// Include files
#include "Event/Track.h"
#include "vdt/sqrt.h"
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
  PatFwdHits m_coords;
  std::array<double,6> m_state; // x0,y0,z0, tx,ty, Q/P
  std::array<double,6> m_params{ { 0., 0., 0., 0., 0., 0. } };
  double m_quality{ 0. };
  std::array<double, 2> m_candquality{ { 0., 0. } };
  std::array<int, 2> m_nb{ { 0, 0 } }; // IT, OT
  std::pair<double, int> m_chi2PerDof{ 0., 0 };
  const LHCb::Track *m_track{ nullptr };
  bool  m_fitted{ false };
  float m_scstddev{0.f};

public:
  template <typename C>
  PatFwdTrackCandidate(const LHCb::Track *tr, C&& coords)
    : m_coords{ std::forward<C>(coords) }, m_track{ tr }  {
    const LHCb::State *state = m_track->stateAt(LHCb::State::EndVelo);
    assert(state != nullptr);
    m_state = { { state->x(),  state->y(), state->z(),
                  state->tx(), state->ty(),
                  state->qOverP() } };
  }

  PatFwdTrackCandidate(const LHCb::Track *tr)
    : PatFwdTrackCandidate(tr, PatFwdHits{}) {
    m_coords.reserve(32);
  }

  template <typename I>
  PatFwdTrackCandidate(const LHCb::Track *tr, I&& first, I&& last)
    : PatFwdTrackCandidate(
        tr, PatFwdHits{ std::forward<I>(first), std::forward<I>(last) })
  { //TODO: partition m_coords by 'selected' 'not selected' 'ignored'
  }

  PatFwdTrackCandidate() = delete;
  PatFwdTrackCandidate(const PatFwdTrackCandidate &) = default;
  PatFwdTrackCandidate(PatFwdTrackCandidate &&) = default;

  ~PatFwdTrackCandidate() = default;

  PatFwdTrackCandidate &operator=(const PatFwdTrackCandidate &) = default;
  PatFwdTrackCandidate &operator=(PatFwdTrackCandidate &&) = default;

  void cleanCoords() {
    m_coords.erase(
          std::remove_if(std::begin(m_coords), std::end(m_coords),
                         [](const PatFwdHit *h) { return !h->isSelected(); }),
          std::end(m_coords));
  }

  const LHCb::Track *track() const { return m_track; }

  double qOverP() const { return m_state[5]; }

  double slX() const { return m_state[3]; }
  double slY() const { return m_state[4]; }
  double slX2() const { return m_state[3] * m_state[3]; }
  double slY2() const { return m_state[4] * m_state[4]; }

  double sinTrack() const { return sqrt(1. - 1. / (1. + slX2() + slY2())); }

  PatFwdHits::const_iterator coordBegin() const { return std::begin(m_coords); }
  PatFwdHits::const_iterator coordEnd() const { return std::end(m_coords); }

  PatFwdHits::iterator coordBegin() { return std::begin(m_coords); }
  PatFwdHits::iterator coordEnd() { return std::end(m_coords); }

  PatFwdHits::iterator begin() { return std::begin(m_coords); }
  PatFwdHits::iterator end() { return std::end(m_coords); }

  PatFwdHits::const_iterator begin() const { return std::begin(m_coords); }
  PatFwdHits::const_iterator end() const { return std::end(m_coords); }

  PatFwdHits::const_reverse_iterator crbegin() const { return m_coords.crbegin(); }
  PatFwdHits::const_reverse_iterator crend() const { return m_coords.crend(); }

  void addCoord(PatFwdHit *coord) { m_coords.push_back(coord); }
  void addCoords(const PatFwdHits &coords) {
    addCoords(std::begin(coords), std::end(coords));
  }
  template <typename I> void addCoords(I first, I last) {
    m_coords.reserve(m_coords.size() + std::distance(first, last));
    m_coords.insert(m_coords.end(), first, last);
  }
  const PatFwdHits &coords() const { return m_coords; }
  // PatFwdHits &coords() { return m_coords; }

  PatFwdTrackCandidate& setParameters(double ax, double bx, double cx, double dx,
                                      double ay, double by) {
    m_params = {{ ax, bx, cx, dx, ay, by }};
    m_fitted = true;
    return *this;
  }

  bool fitted() const { return m_fitted; }

  void updateParameters(double dax, double dbx, double dcx, double ddx = 0.,
                        double day = 0., double dby = 0.) {
    m_params[0] += dax;
    m_params[1] += dbx;
    m_params[2] += dcx;
    m_params[3] += ddx;
    m_params[4] += day;
    m_params[5] += dby;
  }

  double ySlope(double) const { return m_params[5]; }

  double cosAfter() const { return vdt::fast_isqrt( 1. + m_params[1] * m_params[1] ) ; }

  double dSlope() const { return m_params[1] - m_state[3]; }

  int setSelectedCoords() {
    int nbSelected = 0;
    for (auto *hit : m_coords) {
      auto ignored = hit->isIgnored();
      hit->setSelected(!ignored);
      if (!ignored) ++nbSelected;
    }
    return nbSelected;
  }

  PatFwdTrackCandidate setQuality(double quality) { m_quality = quality; return *this; }
  double quality() const { return m_quality; }

  PatFwdTrackCandidate& setChi2PerDoF(double chi2, int nDof) {
    m_chi2PerDof.first = chi2;
    m_chi2PerDof.second = nDof;
    return *this;
  }
  double chi2PerDoF() const { return m_chi2PerDof.first; }
  int nDoF() const { return m_chi2PerDof.second; }

  double bx() const { return m_params[1]; }

  PatFwdTrackCandidate& setNbIT(int nb) { m_nb[0] = nb; return *this; }
  PatFwdTrackCandidate& setNbOT(int nb) { m_nb[1] = nb; return *this; }

  int nbIT() const { return m_nb[0]; }
  int nbOT() const { return m_nb[1]; }

  PatFwdTrackCandidate& setCand1stQuality(double cand1stquality) {
    m_candquality[0] = cand1stquality;
    return *this;
  }
  PatFwdTrackCandidate& setCand2ndQuality(double cand2ndquality) {
    m_candquality[1] = cand2ndquality;
    return *this;
  }

  double cand1stquality() const { return m_candquality[0]; }
  double cand2ndquality() const { return m_candquality[1]; }

  template <typename T> decltype(auto) xStraight(T z) const{
    return m_state[0] + m_state[3] * (z - m_state[2]);
  }
  template <typename T> decltype(auto) yStraight(T z) const{
    return m_state[1] + m_state[4] * (z - m_state[2]);
  }
  template <typename T> decltype(auto) x(T dz) const{
    return m_params[0] + dz * (m_params[1] + dz * (m_params[2] + dz * m_params[3]));
  }
  template <typename T> decltype(auto) y(T dz) const{
    return  m_params[4] + dz * m_params[5];
  }
  template <typename T> decltype(auto) xSlope(T dz) const{
    return m_params[1] + dz * (2 * m_params[2] + 3 * dz * m_params[3]);
  }
  template <typename T> decltype(auto)  xMagnet(T dz) const{
    return m_params[0] + dz * m_params[1];
  }

};
#endif // PATFWDTRACKCANDIDATE_H
