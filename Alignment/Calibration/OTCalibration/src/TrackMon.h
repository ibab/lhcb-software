#ifndef TRACK_MON
#define TRACK_MON

#include <GaudiAlg/GaudiTupleAlg.h>

#include <OTDet/DeOTDetector.h>

#include <GaudiKernel/ToolHandle.h>

#include <TrackInterfaces/ITrackFitter.h>
#include <TrackInterfaces/ITrackProjector.h>
#include <TrackInterfaces/ITrackExtrapolator.h>
#include <OTDAQ/IOTRawBankDecoder.h>

#include <GaudiKernel/IDetDataSvc.h>
#include <DetDesc/Condition.h>

namespace LHCb { class Track; }

class TrackMon: public GaudiTupleAlg
{
private:
  static const unsigned int MODE_MODULE = 0;
  static const unsigned int MODE_LAYER = 1;
  static const unsigned int MODE_STATION = 2;
  static const unsigned int MODE_FRAME = 3;
private:
  DeOTDetector* detector;

  ToolHandle<ITrackFitter> fitter;
  ToolHandle<ITrackProjector> projector;
  ToolHandle<IOTRawBankDecoder> decoder;

  std::string trackLocation;

  std::vector<double> modulet0s;

  /**
   * Max track Chi^2 (default 16).
   */
  double maxTrackChi2;

  double minNDF;
  double minP;
  int trackType;

  IDetDataSvc *detDataSvc;
  Condition *condReadoutGate;
  double readoutGateT1;
  double readoutGateT2;
  double readoutGateT3;
public:
  TrackMon(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();
private:
  void fillUnbiasedResiduals(const LHCb::Track* track, int mode, std::vector<double>& tdrift, std::vector<double>& dist, std::vector<double>& timeResidual, std::vector<double>& distResidual, std::vector<double>& chi2, std::vector<double>& err2, std::vector<double>& errmes2);
  StatusCode condReadoutGateUpdate();
};

#endif // TRACK_MON
