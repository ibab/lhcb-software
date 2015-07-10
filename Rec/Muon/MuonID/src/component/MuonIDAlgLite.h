#ifndef MUONIDALGLITE_H
#define MUONIDALGLITE_H

#include <string>

#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "MuonID/ICommonMuonTool.h"
#include "DLLMuonTool.h"
#include "MakeMuonTool.h"

/** @class MuonIDAlgLite MuonIDAlgLite.h
 * A lightweight version of MuonIDAlg that uses the CommonMuonTool.
 * At this point makeMuonTrack is not fully implemented and no DLL is calulated,
 *yet.
 *
 * @author Kevin Dungs
 * @date 2015-01-06
 */
class MuonIDAlgLite final : public GaudiAlgorithm {

 public:
  MuonIDAlgLite(const std::string &name, ISvcLocator *pSvcLocator);
  ~MuonIDAlgLite() override = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  // Helper functions
  bool isGoodOfflineTrack(const LHCb::Track &) const;
  int m_dllFlag;

  std::vector<CommonConstMuonHits> m_muonVector; 
  LHCb::Track makeMuonTrack(const LHCb::MuonPID &) const;
  // Members
  ICommonMuonTool *muonTool_;
  DLLMuonTool *DLLTool_;
  MakeMuonTool *makeMuonTool_;
  bool useTTrack_;
  std::string tesPathInputTracks_, tesPathOutputMuonPid_,
      tesPathOutputMuonTracks_;
};

#endif  // MUONIDALGLITE_H
