#ifndef HCDELAYSCAN_H
#define HCDELAYSCAN_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// ROOT
#include "TH2D.h"
#include "TProfile.h"

// Local
#include "HCMonitorBase.h"

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

 private:
  /// TES location of digits.
  std::string m_digitLocation;

  unsigned int m_minStep;
  unsigned int m_maxStep;

  /// Profile histograms (ADC vs. step) for each channel and slot.
  std::vector<std::vector<std::vector<TProfile*> > > m_adcs;
  /// ADC distributions for each channel, slot, and step.
  std::vector<std::vector<std::vector<std::vector<TH1D* > > > > m_adcsPerStep;
  std::vector<std::vector<TH2D*> > m_scanResults;
  std::vector<std::vector<TH2D*> > m_scanOffset;

  unsigned int m_bxCentral;
  unsigned int m_bxNext;
  unsigned int m_bxPrev;
};

#endif
