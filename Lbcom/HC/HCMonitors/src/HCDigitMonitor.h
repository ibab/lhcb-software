#ifndef HC_DIGIT_MONITOR_H
#define HC_DIGIT_MONITOR_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class HCDigitMonitor HCDigitMonitor.h
 *
 *  Monitoring algorithm for Herschel digits.
 *
 */

class HCDigitMonitor : public GaudiHistoAlg {
 public:
  /// Standard constructor
  HCDigitMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCDigitMonitor();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  /// TES location of digits.
  std::string m_digitLocation;

  /// Number of B-side crate
  unsigned int m_crateB;
  /// Number of F-side crate
  unsigned int m_crateF;

  /// Channel mappings
  std::vector<unsigned int> m_channelsB0;
  std::vector<unsigned int> m_channelsB1;
  std::vector<unsigned int> m_channelsB2;
  std::vector<unsigned int> m_channelsF1;
  std::vector<unsigned int> m_channelsF2;

  /// Flags whether a channel is mapped to a PMT or not.
  std::vector<bool> m_mappedB;
  std::vector<bool> m_mappedF; 

  /// Station numbers for each channel.
  std::vector<unsigned int> m_stationB;
  std::vector<unsigned int> m_stationF;
  /// Quadrant numbers for each channel.
  std::vector<unsigned int> m_quadrantB;
  std::vector<unsigned int> m_quadrantF;

  /// Parameters for ADC distribution histograms
  Gaudi::Histo1DDef m_parAdc;

  /// ADC distributions of all channels
  std::vector<AIDA::IHistogram1D*> m_hAdcB;
  std::vector<AIDA::IHistogram1D*> m_hAdcF;

  /// ADC distributions of all quadrants
  std::vector<AIDA::IHistogram1D*> m_hAdcB0;
  std::vector<AIDA::IHistogram1D*> m_hAdcB1;
  std::vector<AIDA::IHistogram1D*> m_hAdcB2;
  std::vector<AIDA::IHistogram1D*> m_hAdcF1;
  std::vector<AIDA::IHistogram1D*> m_hAdcF2;

  /// ADC sum distributions
  AIDA::IHistogram1D* m_hAdcSumB0;
  AIDA::IHistogram1D* m_hAdcSumB1;
  AIDA::IHistogram1D* m_hAdcSumB2;
  AIDA::IHistogram1D* m_hAdcSumF1;
  AIDA::IHistogram1D* m_hAdcSumF2;

  /// Mean ADC vs. channel number
  AIDA::IProfile1D* m_hAdcVsChannelB;
  AIDA::IProfile1D* m_hAdcVsChannelF;

  /// Mean ADC vs. quadrant
  AIDA::IProfile1D* m_hAdcVsQuadrantB0;
  AIDA::IProfile1D* m_hAdcVsQuadrantB1;
  AIDA::IProfile1D* m_hAdcVsQuadrantB2;
  AIDA::IProfile1D* m_hAdcVsQuadrantF1;
  AIDA::IProfile1D* m_hAdcVsQuadrantF2;
 
};

#endif
