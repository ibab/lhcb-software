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

  /// ADC distributions as function of BX ID.
  std::vector<AIDA::IHistogram2D*> m_hAdcVsBx;

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


  /// ADC distributions of all channels
  std::vector<AIDA::IHistogram1D*> m_hAdcBCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcFCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcEvenBCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcEvenFCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcOddBCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcOddFCor;

  /// ADC distributions of all quadrants
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantEvenCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantOddCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantNoBeamCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantBeamCor;

  /// ADC distributions as function of BX ID.
  std::vector<AIDA::IHistogram2D*> m_hAdcVsBxCor;

  /// ADC sum distributions
  std::vector<AIDA::IHistogram1D*> m_hAdcSumCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumEvenCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumOddCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumNoBeamCor;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumBeamCor;

  /// Mean ADC vs. channel number
  AIDA::IProfile1D* m_hAdcVsChannelBCor;
  AIDA::IProfile1D* m_hAdcVsChannelFCor;
  AIDA::IProfile1D* m_hAdcVsChannelEvenBCor;
  AIDA::IProfile1D* m_hAdcVsChannelEvenFCor;
  AIDA::IProfile1D* m_hAdcVsChannelOddBCor;
  AIDA::IProfile1D* m_hAdcVsChannelOddFCor;

  /// Mean ADC vs. quadrant
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantCor;
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantEvenCor;
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantOddCor;
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantNoBeamCor;
  std::vector<AIDA::IProfile1D*> m_hAdcVsQuadrantBeamCor;

};

#endif
