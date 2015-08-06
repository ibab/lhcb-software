#ifndef HC_CLOCK_SCAN_H
#define HC_CLOCK_SCAN_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Local
#include "HCMonitorBase.h"

/** @class HCClockScan HCClockScan.h
 *
 *  Algorithm for Herschel clock scan analysis.
 *
 */

class HCClockScan : public HCMonitorBase {
 public:
  /// Optimisation schemes
  enum Scheme {
    AbsoluteMax, ///< Use max. ADC value
    RelativeMax  ///< Use ratio between max. and second max. ADC value
  };

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

  /// Quadrants ADCs per channel and TAE slot
  std::vector<std::vector<std::vector<AIDA::IProfile1D*> > > m_adcs;
  std::vector<std::vector<std::vector<std::vector<AIDA::IHistogram1D*> > > > m_adcsPerStep;
  std::vector<std::vector<AIDA::IHistogram2D*> > m_results;
  std::vector<std::vector<AIDA::IHistogram2D*> > m_offsets;
  std::vector<AIDA::IHistogram2D*> m_resultsStation;

  int m_stepCounter;
  /// Flag for optimisation scheme
  bool m_relativeMax;
};

#endif
