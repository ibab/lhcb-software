#ifndef HC_DIGIT_MONITOR_H
#define HC_DIGIT_MONITOR_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Local
#include "HCMonitorBase.h"

/** @class HCDigitMonitor HCDigitMonitor.h
 *
 *  Monitoring algorithm for Herschel digits.
 *
 */

class HCDigitMonitor : public HCMonitorBase {
 public:
  /// Standard constructor
  HCDigitMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCDigitMonitor();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution
  virtual StatusCode finalize();    ///< Algorithm finalization

 private:
  /// TES location of digits
  std::string m_digitLocation;

  /// ADC distributions of all channels
  std::vector<AIDA::IHistogram1D*> m_hAdcB;
  std::vector<AIDA::IHistogram1D*> m_hAdcF;
  std::vector<AIDA::IHistogram1D*> m_hAdcEvenB;
  std::vector<AIDA::IHistogram1D*> m_hAdcEvenF;
  std::vector<AIDA::IHistogram1D*> m_hAdcOddB;
  std::vector<AIDA::IHistogram1D*> m_hAdcOddF;

  /// ADC distributions of all quadrants
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrant;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantEven;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantOdd;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantNoBeam;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantBeam;

  /// ADC sum distributions
  std::vector<AIDA::IHistogram1D*> m_hAdcSum;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumEven;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumOdd;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumNoBeam;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumBeam;

  /// Mean ADC vs. channel number
  AIDA::IProfile1D* m_hAdcVsChannelB;
  AIDA::IProfile1D* m_hAdcVsChannelF;
  AIDA::IProfile1D* m_hAdcVsChannelEvenB;
  AIDA::IProfile1D* m_hAdcVsChannelEvenF;
  AIDA::IProfile1D* m_hAdcVsChannelOddB;
  AIDA::IProfile1D* m_hAdcVsChannelOddF;

  /// Mean ADC vs. quadrant
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrant;
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantEven;
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantOdd;
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantNoBeam;
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantBeam;

};

#endif
