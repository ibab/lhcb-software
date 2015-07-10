#ifndef ICOMMONMUONTOOL_H_
#define ICOMMONMUONTOOL_H_

#include "GaudiKernel/IAlgTool.h"
#include "MuonID/CommonMuonHit.h"

static const InterfaceID IID_ICommonMuonTool("ICommonMuonTool", 1, 0);

class ICommonMuonTool : virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID() { return IID_ICommonMuonTool; }
  static const unsigned nStations = 5, nRegions = 4;

  using MuonTrackExtrapolation =
      std::array<std::pair<double, double>, nStations>;
  using MuonTrackOccupancies = std::array<unsigned, nStations>;

  virtual auto extrapolateTrack(const LHCb::Track&) const
      -> MuonTrackExtrapolation = 0;
  virtual auto preSelection(const LHCb::Track&) const noexcept -> bool = 0;
  virtual auto inAcceptance(const MuonTrackExtrapolation&) const noexcept
      -> bool = 0;
  virtual auto hitsAndOccupancies(const LHCb::Track&,
                                  const MuonTrackExtrapolation&) const
      -> std::tuple<CommonConstMuonHits, MuonTrackOccupancies> = 0;
  virtual auto extractCrossed(const CommonConstMuonHits&) const noexcept
      -> std::tuple<CommonConstMuonHits, MuonTrackOccupancies> = 0;
  virtual auto isMuon(const MuonTrackOccupancies&, double) const noexcept
      -> bool = 0;
  virtual auto isMuonLoose(const MuonTrackOccupancies&, double) const noexcept
      -> bool = 0;
  virtual auto foi(int, int, double) const noexcept
      -> std::pair<double, double> = 0;
};

#endif  // ICOMMONMUONTOOL_H_
