#pragma once

#include "AIDA/IHistogram1D.h"

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class HCMonitorBase HCMonitorBase.h
 *
 *  Base class for Herschel monitoring algorithms.
 *
 */

class HCMonitorBase : public GaudiTupleAlg {
 public:
  /// Standard constructor
  HCMonitorBase(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCMonitorBase();

  virtual StatusCode initialize();  ///< Algorithm initialization

 protected:
  /// Number of B-side crate
  unsigned int m_crateB;
  /// Number of F-side crate
  unsigned int m_crateF;

  /// ADC binning
  std::vector<double> m_edges;

  /// Channel mappings
  std::vector<bool> m_maskedB0;
  std::vector<bool> m_maskedB1;
  std::vector<bool> m_maskedB2;
  std::vector<bool> m_maskedF1;
  std::vector<bool> m_maskedF2;

  std::vector<unsigned int> m_channelsB0;
  std::vector<unsigned int> m_channelsB1;
  std::vector<unsigned int> m_channelsB2;
  std::vector<unsigned int> m_channelsF1;
  std::vector<unsigned int> m_channelsF2;

  std::vector<unsigned int> m_sparesB0;
  std::vector<unsigned int> m_sparesB1;
  std::vector<unsigned int> m_sparesB2;
  std::vector<unsigned int> m_sparesF1;
  std::vector<unsigned int> m_sparesF2;

  /// Channel numbers for each quadrant
  std::vector<std::vector<unsigned int> > m_channels;
  /// Masked flags for each quadrant
  std::vector<std::vector<bool> > m_masked;
  /// Reference channel numbers for each quadrant
  std::vector<std::vector<unsigned int> > m_references;

  /// Random number generator
  Rndm::Numbers m_uniform;
  /// Flag whether to use variable binning in ADC histograms.
  bool m_variableBins;
  /// Flag whether to randomise the ADC values.
  bool m_randomiseAdc;

  /// Additional term added to the ADC to get the correct average.
  std::vector<double> m_adcCorrection;
  std::vector<unsigned int> m_adcStep;

  /// Smallest BX ID to be included in the plots.
  unsigned int m_bxMin;
  /// Highest BX ID to be included in the plots.
  unsigned int m_bxMax;

  /// Setup the mapping for a given station.
  bool mapChannels(const std::vector<unsigned int>& channels,
                   const std::vector<unsigned int>& refs,
                   const std::vector<bool>& masked,
                   const unsigned int station, const bool bwd);
  /// Print the channel mapping for information.
  void printMapping() const;
  /// Randomise a given ADC value or add a correction term.
  double fadc(const int adc);
  /// Scale histograms (in case of variable binning).
  void scale(AIDA::IHistogram1D* h);
};
