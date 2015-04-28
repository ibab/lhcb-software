#ifndef MUONIDALGLITE_H
#define MUONIDALGLITE_H

#include <string>

#include "Event/MuonPID.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "MuonID/ICommonMuonTool.h"
#include "DLLMuonTool.h"

/** @class MuonIDAlgLite MuonIDAlgLite.h
 * A lightweight version of MuonIDAlg that uses the CommonMuonTool.
 * At this point makeMuonTrack is not fully implemented and no DLL is calulated,
 *yet.
 *
 * @author Kevin Dungs
 * @date 2015-01-06
 */
class MuonIDAlgLite final : public GaudiAlgorithm {
 
  std::map< LHCb::MuonPID*, CommonConstMuonHits > m_muonMap;

 public:
  MuonIDAlgLite(const std::string &name, ISvcLocator *pSvcLocator);
  ~MuonIDAlgLite() override = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  // Helper functions
  bool isGoodOfflineTrack(const LHCb::Track &) const;
  /// Ignore MuonID info from conditions database.
  bool m_OverrideDB;
  // Use or not uncrossed logical channels in the non-pad detector areas
  bool m_use_uncrossed;
  double m_PreSelMomentum;
  // Momentum ranges: different treatement of M4/M5 in each
  std::vector<double> m_MomentumCuts; // vector of momentum ranges
  //Create container with all muonTracks (even if not in acceptance or !IsMuon)
  bool m_DoAllMuonTracks;
  //want to find quality?
  bool m_FindQuality;  
  double m_foifactor; 

  int m_nMax;// number of steps

  int m_dllFlag;

  LHCb::Track makeMuonTrack(const LHCb::MuonPID &) const;
  // Members
  ICommonMuonTool *muonTool_;
  DLLMuonTool *DLLTool_;
  bool useTTrack_;
  std::string tesPathInputTracks_, tesPathOutputMuonPid_,
      tesPathOutputMuonTracks_;
};

#endif  // MUONIDALGLITE_H
