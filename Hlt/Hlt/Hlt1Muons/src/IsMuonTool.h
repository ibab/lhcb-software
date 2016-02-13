#ifndef ISMUONTOOL_H
#define ISMUONTOOL_H

#include <string>
#include <vector>

#include "Event/Track.h"
#include "GaudiAlg/GaudiTool.h"
#include "MuonID/ICommonMuonTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"

/** Tool for identifying muons in the HLT. Makes use of the CommonMuonTool.
 *
 * @author Kevin Dungs
 * @date 2015-01-06
 */
class IsMuonTool : public GaudiTool, virtual public ITracksFromTrack {
 public:
  IsMuonTool(const std::string&, const std::string&, const IInterface*);
  ~IsMuonTool() override = default;
  StatusCode initialize() override;
  StatusCode tracksFromTrack(const LHCb::Track&,
                             std::vector<LHCb::Track*>&) const override;

 private:
  ICommonMuonTool* m_muonTool;
};

#endif  // ISMUONTOOL_H
