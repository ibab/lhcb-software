#pragma once

#include "AIDA/IHistogram1D.h"

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IEventTimeDecoder.h"
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

  /// Randomise a given ADC value or add a correction term. 
  double fadc(const unsigned int adc);
  /// Scale histograms (in case of variable binning).
  void scale(AIDA::IHistogram1D* h);
  /// Establish the station and quadrant numbers for each channel.
  void mapChannels(const std::vector<unsigned int>& channels,
                   const bool bwd, const unsigned int station);
};

