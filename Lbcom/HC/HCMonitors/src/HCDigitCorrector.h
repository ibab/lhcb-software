#pragma once

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// Local
#include "HCMonitorBase.h"

class Condition;

/** @class HCDigitCorrector.h
 *
 * Apply common mode suppression to raw ADC values.
 *
 */

class HCDigitCorrector : public HCMonitorBase {
 public:
  /// Standard constructor
  HCDigitCorrector(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCDigitCorrector();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  std::string m_inputLocation;
  std::string m_outputLocation;

  Condition* m_condCM;

  /// Calibration constants for each channel and parity
  std::vector<std::vector<std::vector<float> > > m_tantheta;
  std::vector<std::vector<std::vector<float> > > m_x0;
  std::vector<std::vector<std::vector<float> > > m_y0;

  // Input parameters  
  std::vector<float> m_thetaConfig;
  std::vector<float> m_x0Config;
  std::vector<float> m_y0Config;

  StatusCode cacheParameters();
  StatusCode setParameters();
 
};
