#ifndef OTCalibrationAlg_OTCalibrationAlg_H
#define OTCalibrationAlg_OTCalibrationAlg_H

#include <string>
#include <map>
#include "GaudiAlg/GaudiAlgorithm.h"
#include "ModuleAccumulator.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackProjector.h"

class TFile ;
class TH1 ;
class TH2F ;
class TProfile ;
class TNamed ;
class DeOTDetector ;
namespace LHCb {
  class Track ;
}
class IOTCalibrationIOTool ;

class OTCalibrationAlg : public GaudiAlgorithm
{
public:
  enum T0Granularity { T0Module, T0Otis, T0Channel } ;
  OTCalibrationAlg(const std::string& name,ISvcLocator* pSvcLocator) ;
  virtual ~OTCalibrationAlg() ;
  virtual StatusCode execute() ;
  virtual StatusCode initialize() ;
  virtual StatusCode finalize() ;

public:
  void registerhist(const std::string& path, TNamed* h1) ;
  double maxDistance() const { return m_maxDistance ; }
  double minTime()     const { return m_minTime ; }
  double maxTime()     const { return m_minTime + 75. ; }
  size_t numDistanceBins() const { return m_numDistanceBins ; }
private:
  void updateCalib() ;
  void updateRtCalib(double& globalt0, std::ostringstream& logmessage) ;
  void updateT0Calib(const double& globalt0, std::ostringstream& logmessage) ;
  void accumulate(const LHCb::Track& track) ;
private:
  // Tools etc
  ToolHandle<ITrackProjector> m_projector ;
  const IOTCalibrationIOTool* m_iotool ;
  DeOTDetector* m_otdet ;

  // jobOptions
  std::string m_trackContainer ; // Input Tracks container location
  std::string m_inputcalibfile ;
  std::string m_histofilename ;
  int m_numDistanceBins ;
  float m_maxDistance ;
  float m_maxDistanceError ;
  float m_minTime ;
  float m_maxResidual ;
  float m_maxTimeResidual ;
  float m_maxTrackChisquarePerDof ;
  bool  m_acceptOutliers ;
  bool  m_storeAllWires ;
  bool  m_calibrateRt ;
  bool  m_calibrateT0 ;
  bool  m_calibrateResolution ;
  bool  m_calibrateTimeResolution ;
  unsigned int m_minEntriesPerStraw ;
  int m_t0Granularity ;
  bool m_combineChannelsInOTIS ;
  int m_rtPolyOrder ;
  unsigned int m_numRtBins ;
  bool m_createStrawTuple ;

  // data
  typedef std::map<const DeOTModule*, OTCal::ModuleAccumulator> ModuleContainer ;
  ModuleContainer m_modules ;
  OTCal::StrawAccumulator m_averagestraw ;
  TFile* m_file ;
  unsigned int m_numProcessedTracks ;
  unsigned int m_numProcessedHits ;
  unsigned int m_numAcceptedHits ;
  size_t m_numAcceptedOutliers ;
  TH1* m_numVeloHitsH1 ;
  TH1* m_numITHitsH1 ;
  TH1* m_numOTHitsH1 ;
  TH1* m_chisqprobH1 ;
  TH1* m_chisqperdofH1 ;
  TH1* m_trkt0H1 ;
  TH1* m_timeresidH1 ;
  TH1* m_resScaleFacH1 ;
  TProfile* m_t0OtisPr ;
  TProfile* m_t0ModulePr ;
  TProfile* m_t0QuarterPr ;
  TProfile* m_distErrPr ;
  TH2F* m_ttodcalibh2 ;
} ;

#endif
