#ifndef HC_CLOCK_SCAN_H
#define HC_CLOCK_SCAN_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include "TH2D.h"
#include "TProfile.h"

// Local
#include "HCMonitorBase.h"

/** @class HCClockScan HCClockScan.h
 *
 *  Algorithm for Herschel clock scan analysis.
 *
 */

class HCClockScan : public HCMonitorBase {
 public:
  /// Standard constructor
  HCClockScan(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCClockScan();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution
  virtual StatusCode finalize();    ///< Algorithm finalization

 private:
  /// TES locations of digits.
  std::vector<std::string> m_digitLocations;

  int m_minStep;
  int m_maxStep;
  int m_VFEClkPitch;
  int m_ADCClkPitch;

  // Namings
  std::vector<std::string> m_stations;
  std::vector<std::string> m_quadrants;
  std::vector<std::string> m_slots;

  /// Quadrants ADCs per channels and tae slot
  std::vector<std::vector<std::vector<TProfile*>>> m_adcs;
  std::vector<std::vector<std::vector<std::vector<TH1D*>>>> m_adcsPerStep;
  std::vector<std::vector<TH2D*>> m_scanResults;
  std::vector<std::vector<TH2D*>> m_scanOffset;

  int m_stepCounter;
  int m_stepEvtCounter;
  float m_centralThreshold;
};

#endif
