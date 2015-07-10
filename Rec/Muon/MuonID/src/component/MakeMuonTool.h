#ifndef MakeMuonTool_H
#define MakeMuonTool_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "MuonDet/MuonBasicGeometry.h"
#include "Event/MuonPID.h"
#include "Event/Track.h"

#include "MuonDet/DeMuonDetector.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

#include "MuonID/CommonMuonHitManager.h"
#include "MuonID/ICommonMuonTool.h"

static const InterfaceID IID_MakeMuonTool("MakeMuonTool", 1, 0);

/** @class MakeMuonTool MakeMuonTool.h
 */
class MakeMuonTool final : public GaudiTool {
 public:
  static const InterfaceID& interfaceID() { return IID_MakeMuonTool; }
  /// Standard constructor
  MakeMuonTool(const std::string& type, const std::string& name,
               const IInterface* parent);

  virtual ~MakeMuonTool() = default;  ///< Destructor

  virtual StatusCode initialize();

  StatusCode muonCandidate(
      const LHCb::Track&, LHCb::Track&,
      const ICommonMuonTool::MuonTrackExtrapolation&,
      const std::vector<LHCb::LHCbID>&);

  virtual StatusCode muonTrackFit(LHCb::Track&);

  StatusCode makeStates(const LHCb::Track&);

  void addLHCbIDsToMuTrack(LHCb::Track&, const std::vector<LHCb::LHCbID>&, 
                           const ICommonMuonTool::MuonTrackExtrapolation&);

  // make muon track function
  LHCb::Track* makeMuonTrack(LHCb::MuonPID*, CommonConstMuonHits &,     
                             const ICommonMuonTool::MuonTrackExtrapolation&);

 protected:
  ITrackFitter* m_fitter;
  ITrackExtrapolator* m_extrapolator;
  IMeasurementProvider* m_measProvider;
  ICommonMuonTool *muonTool_;

  DeMuonDetector* m_mudet;

  bool m_FindQuality;

  unsigned m_NStation;

  LHCb::State* m_mySeedState;

  std::vector<double> m_zstations;

  std::vector<LHCb::State> m_states;

};

#endif  // MUIDTOOL_H
