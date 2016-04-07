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

  /// Flag whether to book and fill the ADC vs. BX 2D histograms.
  bool m_adcVsBx; 
  /// Flag whether to fill the ADC distribution histograms for all channels.
  bool m_allChannels;

  /// ADC distributions of all channels
  std::vector<std::vector<AIDA::IHistogram1D*> > m_hAdcChannel;

  /// ADC distributions of all quadrants
  std::vector<std::vector<AIDA::IHistogram1D*> > m_hAdcQuadrant;
  /// ADC distributions of all quadrants for empty-empty crossings
  std::vector<std::vector<AIDA::IHistogram1D*> > m_hAdcQuadrantNoBeam;
  /// ADC distributions of all quadrants for beam-beam crossings
  std::vector<std::vector<AIDA::IHistogram1D*> > m_hAdcQuadrantBeam;

  /// ADC distributions as function of BX ID.
  std::vector<AIDA::IHistogram2D*> m_hAdcVsBx;

  /// ADC sum distributions
  std::vector<std::vector<AIDA::IHistogram1D*> > m_hAdcSum;
  /// ADC sum distributions for empty-empty crossings
  std::vector<std::vector<AIDA::IHistogram1D*> > m_hAdcSumNoBeam;
  /// ADC sum distributions for beam-beam crossings
  std::vector<std::vector<AIDA::IHistogram1D*> > m_hAdcSumBeam;

  /// Mean ADC vs. channel number
  std::vector<std::vector<AIDA::IProfile1D*> > m_hAdcVsChannel;

  /// Mean ADC vs. quadrant
  std::vector<std::vector<AIDA::IProfile1D*> > m_hAdcVsQuadrant;
  /// Mean ADC vs. quadrant for empty-empty crossings
  std::vector<std::vector<AIDA::IProfile1D*> > m_hAdcVsQuadrantNoBeam;
  /// Mean ADC vs. quadrant for beam-beam crossings
  std::vector<std::vector<AIDA::IProfile1D*> > m_hAdcVsQuadrantBeam;

};

#endif
