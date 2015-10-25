#ifndef HCDIGITCREATOR_H
#define HCDIGITCREATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class HCDigitCreator HCDigitCreator.h HCDigitisation/HCDigitCreator.h
 *
 *  This algorithm simulates the detector and front-end response 
 *  of the Herschel forward shower counters and writes out raw banks.
 *
 */

class HCDigitCreator : public GaudiTupleAlg {

public:
  /// Standard constructor
  HCDigitCreator(const std::string& name,ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCDigitCreator();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:
  /// Location of input container
  std::string m_hitLocation;
  /// Location of output container
  std::string m_rawEventLocation;

  /// Number of B-side crate
  unsigned int m_crateB;
  /// Number of F-side crate
  unsigned int m_crateF;

  /// Channel mappings
  std::vector<int> m_channelsB0;
  std::vector<int> m_channelsB1;
  std::vector<int> m_channelsB2;
  std::vector<int> m_channelsF1;
  std::vector<int> m_channelsF2;

  /// Channel numbers for each quadrant
  std::vector<std::vector<unsigned int> > m_channels;

  /// Nominal z-positions of the backward stations
  std::vector<double> m_zB;
  /// Nominal z-positions of the forward stations
  std::vector<double> m_zF;

  /// Number of photons per MeV of deposited energy
  double m_photonsPerMeV;
  /// Average fraction of collected photons
  double m_collectionEfficiency;
  /// Quantum efficiency
  double m_quantumEfficiency;
  /// Overall conversion factor between deposited energy and photo electrons
  double m_pePerMeV;
  /// PMT gain
  double m_gain;
  /// ADC conversion factor
  double m_electronsPerADC;
  /// Max. number of ADC counts
  unsigned int m_maxADC;
  /// Trigger threshold in ADC counts
  unsigned int m_threshold;
 
  /// Flag to activate monitoring histograms or not
  bool m_monitoring;

  /// Generator for Gaussian random variates
  Rndm::Numbers m_gauss;
  /// Generator for uniform random variates
  Rndm::Numbers m_uniform;

  /// Set up the channel map for one station.
  bool mapChannels(const std::vector<int>& channels,
                   const unsigned int station, const bool bwd);
  /// Raw bank encoding (PRS/SPD format) 
  void encode(const unsigned int card,
              const std::vector<unsigned int>& adcs,
              std::vector<unsigned int>& words);
};
#endif
