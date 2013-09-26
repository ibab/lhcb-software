#ifndef VPDIGITCREATOR_H 
#define VPDIGITCREATOR_H 1

// Activate filling of debugging histograms while the algorithm	runs
// #define DEBUG_HISTO 

// STL 
#include <string>
#include <map>

// Gaudi
#ifdef DEBUG_HISTO
#include "GaudiAlg/GaudiTupleAlg.h"
#else
#include "GaudiAlg/GaudiAlgorithm.h"
#endif
#include "GaudiKernel/RndmGenerators.h"

// LHCb
// Event/DigiEvent
#include "Event/VPDigit.h"

/** @class VPDigitCreator VPDigitCreator.h
 *  
 *
 *  @author Thomas Britton
 *  @date   2010-07-07
 */

#ifdef DEBUG_HISTO
class VPDigitCreator : public GaudiTupleAlg {
#else
class VPDigitCreator : public GaudiAlgorithm {
#endif

public: 
  /// Standard constructor
  VPDigitCreator(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPDigitCreator();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  std::string m_inputLocation;
  std::string m_outputLocation;

  // Bunch crossing spacing in ns
  double m_bunchCrossingSpacing;  
  // Discrimination threshold in number of electrons
  double m_threshold;
    
  // Calibration constants used during tuning of the front end. X-threshold) electrons = Y ToT
  double m_chargeTuning;
  double m_totTuning;
  double m_discharge;
  
  double m_clockPhase;
  double m_samplePeriod;

   // Number of bits to use for ToT
  unsigned int m_nBits;
  // Max ToT value
  unsigned int m_maxToT;

  // Gaussian random number generator for noise
  Rndm::Numbers m_gaussDist;
  // Noise in electrons (0 = don't simulate)
  double m_ElectronicNoise;

  // Option to use pixel dead time
  bool m_deadTime;
  // List of dead pixels and time until recovery
  std::map<const LHCb::VPChannelID, double> m_deadPixels; 

  bool m_debug;

  double timeOverThreshold(double charge);
  
};
#endif // VPDIGITCREATOR_H
