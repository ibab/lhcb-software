#ifndef HC_DIGIT_MONITOR_H
#define HC_DIGIT_MONITOR_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/IEventTimeDecoder.h"

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
  virtual StatusCode finalize();    ///< Algorithm finalization

 private:
  /// TES location of digits.
  std::string m_digitLocation;

  /// ODIN
  IEventTimeDecoder* m_odin;

  /// Number of B-side crate
  unsigned int m_crateB;
  /// Number of F-side crate
  unsigned int m_crateF;

  /// ADC binning
  std::vector<double> m_edges;

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
  std::vector<AIDA::IHistogram1D*> m_hAdcEvenB;
  std::vector<AIDA::IHistogram1D*> m_hAdcEvenF;
  std::vector<AIDA::IHistogram1D*> m_hAdcOddB;
  std::vector<AIDA::IHistogram1D*> m_hAdcOddF;

  /// ADC distributions of all quadrants
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrant;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantEven;
  std::vector<AIDA::IHistogram1D*> m_hAdcQuadrantOdd;

  /// ADC sum distributions
  std::vector<AIDA::IHistogram1D*> m_hAdcSum;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumEven;
  std::vector<AIDA::IHistogram1D*> m_hAdcSumOdd;

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

  /// Additional term added to the ADC to get the correct average.
  std::vector<double> m_adcCorrection;

  void scale(AIDA::IHistogram1D* h);
  void mapChannels(const std::vector<unsigned int>& channels,
                   const bool bwd, const unsigned int station);
};

#endif
