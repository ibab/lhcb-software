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

  /// Generator for Gaussian random variates
  Rndm::Numbers m_gauss;
  /// Generator for uniform random variates
  Rndm::Numbers m_uniform;

  /// Number of photons per MeV of deposited energy
  double m_photonsPerMeV;
  /// Average fraction of collected photons
  double m_collectionEfficiency;
  /// Quantum efficiency
  double m_quantumEfficiency;
  /// PMT gain
  double m_gain;
  /// ADC conversion factor
  double m_electronsPerADC;
  /// Max. number of ADC counts
  unsigned int m_maxADC;

  /// Nominal z-positions of the stations
  std::vector<double> m_z;
  /// Number of stations
  unsigned int m_nStations;
  /// Number of front-end channels.
  unsigned int m_nChannels;

  /// Flag to activate monitoring histograms or not
  bool m_monitoring;
  
};
#endif
