/** Implementation of IsMuonTightTool.
 *
 * @author Francesco Dettori, Kevin Dungs
 * @date 2016-02-23
 */
#include "IsMuonTightTool.h"

#include <string>
#include <vector>

#include "Event/Track.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "MuonID/ICommonMuonTool.h"
#include "MuonID/ICommonMuonTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"

DECLARE_TOOL_FACTORY(IsMuonTightTool)

/** Creatortor
 * Also declare interface so tool can be obtained in Gaudi via tool<>().
 */
IsMuonTightTool::IsMuonTightTool(const std::string& type, const std::string& name,
                       const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITracksFromTrack>(this);
}

/** Load the CommonMuonTool.
 */
StatusCode IsMuonTightTool::initialize() {
  auto sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  m_muonTool = tool<ICommonMuonTool>("CommonMuonTool");
  return sc;
}

/** Implement signature specified by the ITracksFromTrack interface.
 * For a given track perform all necessary steps for muon id on it. Save an
 * output track if isMuon is true.
 */
StatusCode IsMuonTightTool::tracksFromTrack(const LHCb::Track& track,
                                       std::vector<LHCb::Track*>& tracks) const {
  if (!m_muonTool->preSelection(track)) {
    return StatusCode::SUCCESS;
  }
  const auto extrapolation = m_muonTool->extrapolateTrack(track);
  if (!m_muonTool->inAcceptance(extrapolation)) {
    return StatusCode::SUCCESS;
  }
  CommonConstMuonHits hits, hitsTight;
  std::array<unsigned, ICommonMuonTool::nStations> occupancies, occupanciesTight;
  std::tie(hits, occupancies) =
    m_muonTool->hitsAndOccupancies(track, extrapolation);
  std::tie(hitsTight, occupanciesTight) = m_muonTool->extractCrossed(hits);
  // Require isMuonTight
  if (m_muonTool->isMuon(occupanciesTight, track.p())) {
    // Add found hits to track
    LHCb::Track* output = track.clone();
    tracks.push_back(output);
    for (const auto& hit : hitsTight) {
      output->addToLhcbIDs(LHCb::LHCbID{hit->tile()});
    }
  }
  return StatusCode::SUCCESS;
}
