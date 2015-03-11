#ifndef COMMONMUONTOOL_H_
#define COMMONMUONTOOL_H_

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/DeMuonDetector.h"

#include "MuonID/CommonMuonHitManager.h"

static const InterfaceID IID_CommonMuonTool("CommonMuonTool", 1, 0);

/** @class CommonMuonTool CommonMuonTool.h
 * A tool that provides functionality for all steps in muon identification that
 *are the same in HLT and offline.
 *
 * It is designed to have no state associated with the event. The only members
 *are related to conditions (such as detector information or FoI).
 *
 * @author Kevin Dungs
 * @date 2015-01-06
 */
class CommonMuonTool final : public GaudiTool {
 public:
  static const InterfaceID& interfaceID() { return IID_CommonMuonTool; }
  static const unsigned nStations = 5, nRegions = 4;

  using MuonTrackExtrapolation =
      std::array<std::pair<double, double>, nStations>;
  using MuonTrackOccupancies = std::array<unsigned, nStations>;

  CommonMuonTool(const std::string& type, const std::string& name,
                 const IInterface* parent);
  ~CommonMuonTool() override = default;

  auto initialize() -> StatusCode override;
  auto extrapolateTrack(const LHCb::Track&) const -> MuonTrackExtrapolation;
  auto preSelection(const LHCb::Track&, const MuonTrackExtrapolation&) const
      noexcept -> bool;
  auto hitsAndOccupancies(const LHCb::Track&,
                          const MuonTrackExtrapolation&) const
      -> std::tuple<CommonConstMuonHits, MuonTrackOccupancies>;
  auto extractCrossed(const CommonConstMuonHits&) const noexcept
      -> std::tuple<CommonConstMuonHits, MuonTrackOccupancies>;
  auto isMuon(const MuonTrackOccupancies&, double) const noexcept -> bool;
  auto isMuonLoose(const MuonTrackOccupancies&, double) const noexcept -> bool;
  auto foi(int, int, double) const noexcept -> std::pair<double, double>;

 private:
  CommonMuonHitManager* m_hitManager;  // non-owning
  DeMuonDetector* m_det;               // non-owning
  double m_foiFactor, m_preSelMomentum;
  std::array<double, nStations> m_stationZ;
  std::array<std::pair<double, double>, nStations * nRegions> m_padSize;
  std::array<std::pair<double, double>, nStations> m_regionInner, m_regionOuter;
  std::array<std::vector<double>, 3> m_foiParamX, m_foiParamY;
  std::vector<double> m_momentumCuts;
};

#endif  // COMMONMUONTOOL_H_
