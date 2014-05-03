#ifndef VPDIGITCREATOR_H 
#define VPDIGITCREATOR_H 1

// STL 
#include <map>

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/RndmGenerators.h"

// LHCb
// Event/DigiEvent
#include "Event/VPDigit.h"

/** @class VPDigitCreator VPDigitCreator.h
 *  Using MCVPDigits as input, this algorithm simulates the response of the 
 *  VeloPix ASIC and produces a VPDigit for each MCVPDigit above threshold. 
 *
 *  @author Thomas Britton
 *  @date   2010-07-07
 */

class VPDigitCreator : public GaudiTupleAlg {

public: 
  /// Standard constructor
  VPDigitCreator(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPDigitCreator();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// Location of input container (MCVPDigits)
  std::string m_inputLocation;
  /// Location of output container (VPDigits)
  std::string m_outputLocation;

  /// Discrimination threshold in number of electrons
  double m_threshold;
  /// Noise in number of electrons
  double m_electronicNoise;

  /// Option to simulate pixel dead time
  bool m_deadTime;
  /// Discharge rate in electrons per ns 
  double m_dischargeRate;
  /// Bunch crossing spacing in ns
  double m_bunchCrossingSpacing;  
  /// List of dead pixels and remaining charge
  std::map<const LHCb::VPChannelID, double> m_deadPixels; 

  /// Option to simulate masked pixels
  bool m_maskedPixels;
  /// Option to simulate noisy pixels
  bool m_noisyPixels;
  /// Fraction of masked pixels
  double m_fractionMasked;
  /// Fraction of noisy pixels
  double m_fractionNoisy;
  /// Average number of noisy pixels
  unsigned int m_nNoisy;

  /// Random number generators
  Rndm::Numbers m_gauss;
  Rndm::Numbers m_uniform;

};
#endif // VPDIGITCREATOR_H
