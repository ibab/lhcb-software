#ifndef __OTModuleClbrAlg_h 
#define __OTModuleClbrAlg_h 1

#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackProjector.h"
#include "OTDet/DeOTDetector.h"

#include "Event/OTMeasurement.h"
#include "Event/FitNode.h"
#include "Event/State.h"

#include <string>

/** @class OTModuleClbrAlg OTModuleClbrAlg.h
 *
 *  This class implements algorithms for module T0's and TR-relation
 *  calibration.
 *
 *  The granularity is per module. If we will need granularity per straw
 *  this class either will be updated or new algorithm will be written.
 *
 *  @author Alexandr Kozlinskiy
 *  @date   2009-10-20
 */
class OTModuleClbrAlg: public GaudiHistoAlg
{
private:
  std::vector<double> confHistDriftTime;
  std::vector<double> confHistDriftTimeResidual;
  std::vector<double> confHistDriftTimeVsDist;
  std::vector<double> confHistModuleDT0;
  std::vector<double> confHistModuleT0;

  AIDA::IHistogram1D* histModuleDriftTime[3][4][4][9];
  AIDA::IHistogram1D* histModuleDriftTimeResidual[3][4][4][9];
  AIDA::IHistogram2D* histModuleDriftTimeVsDist[3][4][4][9];

  AIDA::IHistogram2D* histDriftTimeVsDist;

  AIDA::IHistogram1D* histModuleDT0VsModuleN;
  AIDA::IHistogram1D* histModuleT0VsModuleN;

  AIDA::IHistogram1D* histModuleDT0;
  AIDA::IHistogram1D* histModuleT0;

  double t0s[3][4][4][9];
  double trA, trB, trC;

  DeOTDetector* detector;

  std::string trackLocation;

  ToolHandle<ITrackProjector> projector;

  double maxTrackChi2;
  double maxUnbiasedChi2;

  bool useOutliers;

  bool writeXMLs;
  bool readXMLs;

  std::vector<std::string> stationNames;
  std::vector<std::string> layerNames;
  std::vector<std::string> quarterNames;
  std::vector<std::string> moduleNames;
public:
  OTModuleClbrAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  void fillHists(const LHCb::OTMeasurement& measurement, const LHCb::FitNode& node, const LHCb::State& state);

  StatusCode fitT0s();
  StatusCode fitTR();
  StatusCode fitTRMinuit();

  StatusCode writeCondXMLs();
  StatusCode readCondXMLs();
};

DECLARE_ALGORITHM_FACTORY(OTModuleClbrAlg);

#endif // __OTModuleClbrAlg_h
