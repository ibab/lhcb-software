#ifndef HCDELAYSCAN_H
#define HCDELAYSCAN_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include "HCMonitorBase.h"
#include "TH2D.h"
#include "TProfile.h"
// Gaudi
#include "GaudiKernel/IEventTimeDecoder.h"

/** @class HCDigitMonitor HCDigitMonitor.h
 *
 *  Monitoring algorithm for Herschel digits.
 *
 */

class HCDelayScan : public HCMonitorBase {
 public:
  /// Standard constructor
  HCDelayScan(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCDelayScan();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution
  virtual StatusCode finalize();  ///< Algorithm finalization

  /// A class to smooth the ADC
  unsigned int randomizeADC( unsigned int adc ) ;

 private:
  /// TES location of digits.
  std::string m_digitLocation;

  /// ODIN
  IEventTimeDecoder* m_odin;

  /// Number of B-side crate
  unsigned int m_crateB;
  /// Number of F-side crate
  unsigned int m_crateF;

  int m_minStep;
  int m_maxStep;
  int m_VFEClkPitch;
  int m_ADCClkPitch;

  std::vector<int> m_BxRange;

  // Namings
  std::vector<std::string> m_stations;
  std::vector<std::string> m_quadrants;
  std::vector<std::string> m_slots;

  /// Parameters for ADC distribution histograms
  Gaudi::Histo1DDef m_parAdc;

  /// Quadrants ADCs per channels and tae slot
  std::map< std::string , std::vector<TProfile*>> m_adcs;
  std::map< std::string , std::vector<std::vector<TH1D*>>>  m_adcsPerStep;
  std::map< std::string , std::vector<TProfile*>> m_adcsCor;
  std::map< std::string , std::vector<std::vector<TH1D*>>>  m_adcsPerStepCor;
  std::vector<std::vector<TH2D*>> m_scanResults;
  std::vector<std::vector<TH2D*>> m_scanOffset;

  int m_BxCentral;
  int m_BxNext;
  int m_BxPrev;


};

#endif
