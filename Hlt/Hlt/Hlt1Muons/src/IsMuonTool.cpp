/** Implementation of IsMuonTool.
 *
 * @author Kevin Dungs
 * @date 2015-01-06
 */
#include "IsMuonTool.h"

#include <string>
#include <vector>

#include "Event/Track.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "MuonID/ICommonMuonTool.h"
#include "MuonID/ICommonMuonTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"

DECLARE_TOOL_FACTORY(IsMuonTool)

/** C'tor
 * Also declare interface so tool can be obtained in Gaudi via tool<>().
 */
IsMuonTool::IsMuonTool(const std::string& type, const std::string& name,
                       const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITracksFromTrack>(this);
}

/** Load the CommonMuonTool.
 */
StatusCode IsMuonTool::initialize() {
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
StatusCode IsMuonTool::tracksFromTrack(const LHCb::Track& track,
                                       std::vector<LHCb::Track*>& tracks) {
  if (!m_muonTool->preSelection(track)) {
    return StatusCode::SUCCESS;
  }
  const auto extrapolation = m_muonTool->extrapolateTrack(track);
  if (!m_muonTool->inAcceptance(extrapolation)) {
    return StatusCode::SUCCESS;
  }
  CommonConstMuonHits hits;
  std::array<unsigned, ICommonMuonTool::nStations> occupancies;
  std::tie(hits, occupancies) =
      m_muonTool->hitsAndOccupancies(track, extrapolation);
  if (m_muonTool->isMuon(occupancies, track.p())) {
    // Add found hits to track
    LHCb::Track* output = track.clone();
    tracks.push_back(output);
    for (const auto& hit : hits) {
      output->addToLhcbIDs(LHCb::LHCbID{hit->tile()});
    }
  }
  return StatusCode::SUCCESS;
}
