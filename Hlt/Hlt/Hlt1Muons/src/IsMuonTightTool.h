#ifndef ISMUONTIGHTTOOL_H
#define ISMUONTIGHTTOOL_H

#include <string>
#include <vector>

#include "Event/Track.h"
#include "GaudiAlg/GaudiTool.h"
#include "MuonID/ICommonMuonTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"

/** Tool for identifying muons in the HLT. Makes use of the CommonMuonTool.
 *
 * @author Francesco Dettori, Kevin Dungs
 * @date 2016-02-23
 */
class IsMuonTightTool : public GaudiTool, virtual public ITracksFromTrack {
 public:
  IsMuonTightTool(const std::string&, const std::string&, const IInterface*);
  ~IsMuonTightTool() override = default;
  StatusCode initialize() override;
  StatusCode tracksFromTrack(const LHCb::Track&,
                             std::vector<LHCb::Track*>&) const override;

 private:
  ICommonMuonTool* m_muonTool;
};

#endif  // ISMUONTIGHTTOOL_H
