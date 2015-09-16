/** Implementation of CommonMuonTool
 *
 * 2015-01-06: Kevin Dungs
 */
#include "CommonMuonTool.h"

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "DetDesc/Condition.h"
#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/DeMuonDetector.h"
#include "vdt/exp.h"

#include "MuonID/CommonMuonHitManager.h"

DECLARE_COMPONENT(CommonMuonTool)

/** C'tor
 * Also declares an interface to that the tool can be obtained in Gaudi via
 * tool<...>(...)
 */
CommonMuonTool::CommonMuonTool(const std::string& type, const std::string& name,
                               const IInterface* parent)
    : base_class(type, name, parent), m_foiFactor{1.0} {
  declareInterface<ICommonMuonTool>(this);
}

/** Initialises the tool.
 * Loads helpers and extracts detector information and variables from the
 * CondDB.
 */
auto CommonMuonTool::initialize() -> StatusCode {
  auto sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  m_hitManager = tool<CommonMuonHitManager>("CommonMuonHitManager");
  m_det = getDet<DeMuonDetector>(DeMuonLocation::Default);

  // Load station information from detector description
  for (unsigned s = 0; s != nStations; ++s) {
    m_regionInner[s] = std::make_pair(m_det->getInnerX(s), m_det->getInnerY(s));
    m_regionOuter[s] = std::make_pair(m_det->getOuterX(s), m_det->getOuterY(s));
    m_stationZ[s] = m_det->getStationZ(s);
  }

  // Prepare to load information from conditions data base
  Condition* condFoiFactor = nullptr;
  Condition* condPreSelMomentum = nullptr;
  Condition* condFoiParametersX = nullptr;
  Condition* condFoiParametersY = nullptr;
  Condition* condMomentumCuts = nullptr;
  registerCondition<CommonMuonTool>("Conditions/ParticleID/Muon/FOIfactor",
                                    condFoiFactor);
  registerCondition<CommonMuonTool>("Conditions/ParticleID/Muon/PreSelMomentum",
                                    condPreSelMomentum);
  registerCondition<CommonMuonTool>("Conditions/ParticleID/Muon/XFOIParameters",
                                    condFoiParametersX);
  registerCondition<CommonMuonTool>("Conditions/ParticleID/Muon/YFOIParameters",
                                    condFoiParametersY);
  registerCondition<CommonMuonTool>("Conditions/ParticleID/Muon/MomentumCuts",
                                    condMomentumCuts);
  // Update conditions
  sc = runUpdate();
  if (sc.isFailure()) {
    Error("Could not update conditions from the CondDB.");
    return sc;
  }
  // Extract parameters
  m_foiFactor = condFoiFactor->param<double>("FOIfactor");
  m_preSelMomentum = condPreSelMomentum->param<double>("PreSelMomentum");
  m_foiParamX[0] = condFoiParametersX->paramVect<double>("XFOIParameters1");
  m_foiParamX[1] = condFoiParametersX->paramVect<double>("XFOIParameters2");
  m_foiParamX[2] = condFoiParametersX->paramVect<double>("XFOIParameters3");
  m_foiParamY[0] = condFoiParametersY->paramVect<double>("YFOIParameters1");
  m_foiParamY[1] = condFoiParametersY->paramVect<double>("YFOIParameters2");
  m_foiParamY[2] = condFoiParametersY->paramVect<double>("YFOIParameters3");
  m_momentumCuts = condMomentumCuts->paramVect<double>("MomentumCuts");

  return sc;
}

/** Project a given track into the muon stations.
 * Returns a std::array that contains the coordinates for each station.
 */
auto CommonMuonTool::extrapolateTrack(const LHCb::Track& track) const
    -> ICommonMuonTool::MuonTrackExtrapolation {
  MuonTrackExtrapolation extrapolation;
  const auto& state = track.closestState(9450.0);  // state closest to M1

  // Project the state into the muon stations
  for (unsigned station = 0; station != nStations; ++station) {
    // x(z') = x(z) + (dx/dz * (z' - z))
    extrapolation[station] = {
        state.x() + state.tx() * (m_stationZ[station] - state.z()),
        state.y() + state.ty() * (m_stationZ[station] - state.z())};
  }

  return extrapolation;
}

/** Require minimum momentum of track.
 */
auto CommonMuonTool::preSelection(const LHCb::Track& track) const noexcept
    -> bool {
  return track.p() > m_preSelMomentum;
}

/** Check whether track extrapolation is within detector acceptance.
 */
auto CommonMuonTool::inAcceptance(
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation) const noexcept
    -> bool {
  using xy_t = std::pair<double, double>;
  auto abs_lt = [](const xy_t& p1, const xy_t& p2) {
    return fabs(p1.first) < p2.first && fabs(p1.second) < p2.second;
  };

  // Outer acceptance
  if (!abs_lt(extrapolation[0], m_regionOuter[0]) ||
      !abs_lt(extrapolation[nStations - 1], m_regionOuter[nStations - 1])) {
    // Outside M1 - M5 region
    return false;
  }

  // Inner acceptance
  if (abs_lt(extrapolation[0], m_regionInner[0]) ||
      abs_lt(extrapolation[nStations - 1], m_regionInner[nStations - 1])) {
    // Inside M1 - M5 chamber hole
    return false;
  }

  return true;
}

/** Extracts hits within the FoI for a given track and its extrapolation. Also
 * gives the occupancies of the muon stations. That information is used in
 * isMuon.
 */
auto CommonMuonTool::hitsAndOccupancies(
    const LHCb::Track& track,
    const ICommonMuonTool::MuonTrackExtrapolation& extrapolation) const
    -> std::tuple<CommonConstMuonHits, ICommonMuonTool::MuonTrackOccupancies> {
  using xy_t = std::pair<double, double>;

  /* Define an inline callable that makes it easier to check whether a hit is
   * within a given window. */
  class IsInWindow {
   public:
    // Constructor takes parameters by value and then moves them into place.
    IsInWindow(xy_t center, xy_t foi0, xy_t foi1, xy_t foi2, xy_t foi3,
               double sf)
        : center_{std::move(center)},
          foi_{{std::move(foi0), std::move(foi1), std::move(foi2),
                std::move(foi3)}} {
      // To my mind this should be an std::transform, but I would like Gerhard
      // to comment on that first.
      std::for_each(std::begin(foi_), std::end(foi_), [&](xy_t& p) {
        p.first *= sf;
        p.second *= sf;
      });
    }
    bool operator()(const CommonMuonHit& hit) {
      auto region = hit.tile().region();
      assert(region < 4);
      return (fabs(hit.x() - center_.first) < hit.dx() * foi_[region].first) &&
             (fabs(hit.y() - center_.second) < hit.dy() * foi_[region].second);
    }

   private:
    xy_t center_;
    std::array<xy_t, 4> foi_;
  };

  auto makeIsInWindow = [&](unsigned station) {
    auto p = track.p();
    return IsInWindow{extrapolation[station], foi(station, 0, p),
                      foi(station, 1, p),     foi(station, 2, p),
                      foi(station, 3, p),     m_foiFactor};
  };

  CommonConstMuonHits hits;
  MuonTrackOccupancies occupancies;
  occupancies[0] = 0;

  // Start from 1 because M1 does not matter for IsMuon
  for (unsigned s = 1; s != nStations; ++s) {
    auto predicate = makeIsInWindow(s);
    const auto& station = m_hitManager->station(s);
    if (msgLevel(MSG::DEBUG))
      debug() << "max number of regions = " << station.nRegions() << endmsg;
    for (unsigned r = 0; r != station.nRegions(); ++r) {
      if (msgLevel(MSG::DEBUG)) debug() << "REGION = " << r << endmsg;
      auto hr = m_hitManager->hits(-m_regionOuter[s].first, s, r);
      std::for_each(
          std::begin(hr), std::end(hr), [&](const CommonMuonHit& hit) {
            if (msgLevel(MSG::DEBUG))
              debug() << "Hit st = " << s << ", reg = " << r
                      << ", position X = " << hit.x()
                      << ", position Y = " << hit.y() << endmsg;
            if (predicate(hit)) {
              if (msgLevel(MSG::DEBUG))
                debug() << "###Hit in FOI. st = " << s << ", reg = " << r
                        << ", position X = " << hit.x()
                        << ", position Y = " << hit.y() << endmsg;
              hits.push_back(&hit);
            }
          });
    }
    occupancies[s] = hits.size();
  }
  // Turn cumulative occupancies into occupancies per station
  std::adjacent_difference(std::begin(occupancies), std::end(occupancies),
                           std::begin(occupancies));

  return std::make_tuple(hits, occupancies);
}

/** Given a container of hits that are sorted by station this function extracts
 * all of the hits that are crossed and calculates the corresponding
 * occupancies.
 * The stations,regions M1RX, M4R1, M5R1 are made out by pads, and hence
 * they behave as crossed hits always. These regions yield mapInRegion == 1,
 * the others yield mapInRegion == 2.
 * The hits can be separated as:
 * 1) uncrossed == 0 && mapInRegion == 1 (crossed hit)
 * 2) uncrossed == 0 && mapInRegion == 2 (crossed hit)
 * 3) uncrossed == 1 && mapInRegion == 1 (crossed hit, due to mapInRegion == 1)
 * 4) uncrossed == 1 && mapInRegion == 2 (uncross hit)
 * For the crossed hits we want to select 1), 2) and 3)
 */
auto CommonMuonTool::extractCrossed(const CommonConstMuonHits& hits) const
    noexcept
    -> std::tuple<CommonConstMuonHits, ICommonMuonTool::MuonTrackOccupancies> {
  auto res = CommonConstMuonHits{};
  res.reserve(hits.size());
  std::copy_if(std::begin(hits), std::end(hits), std::back_inserter(res),
               [&](const CommonMuonHit* hit) {
                 return !hit->uncrossed() ||
                        m_det->mapInRegion(hit->tile().station(),
                                           hit->tile().region()) == 1;
               });
  res.shrink_to_fit();
  // Recalculate occupancies - makes use of the fact that hits are ordered by
  // station
  MuonTrackOccupancies occupancies;
  occupancies[0] = 0;
  auto ppl = std::begin(res);
  auto ppr = std::begin(res);
  for (auto s = 1u; s != nStations; ++s) {
    ppr = std::partition_point(
        ppl, std::end(res),
        [s](const CommonMuonHit* hit) { return hit->tile().station() == s; });
    occupancies[s] = std::distance(ppl, ppr);
    ppl = ppr;
  }
  return std::make_tuple(res, occupancies);
}

/** Checks 'isMuon' given the occupancies corresponding to a track and the
 * track's momentum. The requirement differs in bins of p.
 */
auto CommonMuonTool::isMuon(
    const ICommonMuonTool::MuonTrackOccupancies& occupancies, double p) const
    noexcept -> bool {
  const double pr1 = m_preSelMomentum, pr2 = m_momentumCuts[0],
               pr3 = m_momentumCuts[1];
  auto has = [&](unsigned station) { return occupancies[station] != 0; };
  const bool has12 = has(1) && has(2);
  bool id = false;

  if (p < pr1) {
    id = false;
  } else if (p < pr2) {
    id = has12;
  } else if (p < pr3) {
    id = has12 && (has(3) || has(4));
  } else {
    id = has12 && has(3) && has(4);
  }

  return id;
}

/** Checks 'isMuonLoose' given the occupancies corresponding to a track and the
 * track's momentum. The requirement differs in bins of p.
 *
 * The magic numbers 3.5 GeV and 4.2 GeV are the results of study by Helder in
 * Aug 2011.
 */
auto CommonMuonTool::isMuonLoose(
    const ICommonMuonTool::MuonTrackOccupancies& occupancies, double p) const
    noexcept -> bool {
  const double pr1 = m_preSelMomentum, pr2 = m_momentumCuts[0], magic1 = 3500,
               magic2 = 4200;
  auto has = [&](unsigned station) { return occupancies[station] != 0; };
  const bool has3 = has(3) && p > magic1, has4 = has(4) && p > magic2;
  bool id = false;

  if (p < pr1) {
    id = false;
  } else if (p < pr2) {
    // Require at least two hits in M2, M3, (M4 if p > magic1).
    auto v = std::vector<bool>{has(1), has(2), has3};
    id = std::count(std::begin(v), std::end(v), true) > 1;
  } else {
    // Require at least three hits in M2, M3, (M4 if p > magic1), (M5 if p >
    // magic2).
    auto v = std::vector<bool>{has(1), has(2), has3, has4};
    id = std::count(std::begin(v), std::end(v), true) > 2;
  }

  return id;
}

auto CommonMuonTool::foi(int station, int region, double p) const noexcept
    -> std::pair<double, double> {
  return i_foi(station, region, p);
}

/** Helper function that returns the x, y coordinates of the FoI for a
 * given station and region and a track's momentum.
 */
auto CommonMuonTool::i_foi(int station, int region, double p) const noexcept
    -> std::pair<double, double> {
  auto i = station * nRegions + region;
  if (p < 1000000) {
    const auto pGev = p / Gaudi::Units::GeV;
    auto ellipticalFoiWindow = [&](const decltype(m_foiParamX)& fp) {
      return fp[0][i] + fp[1][i] * vdt::fast_exp(-fp[2][i] * pGev);
    };
    return {ellipticalFoiWindow(m_foiParamX), ellipticalFoiWindow(m_foiParamY)};
  } else {
    return {m_foiParamX[0][i], m_foiParamY[0][i]};
  }
}
