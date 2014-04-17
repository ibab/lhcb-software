#ifndef VPDEPOSITCREATOR_H
#define VPDEPOSITCREATOR_H 1

// Activate filling of debugging histograms while the algorithm runs
// #define DEBUG_HISTO 

// STL
#include <string>
// Gaudi
#ifdef DEBUG_HISTO
#include "GaudiAlg/GaudiTupleAlg.h"
#else
#include "GaudiAlg/GaudiAlgorithm.h"
#endif
#include "GaudiKernel/RndmGenerators.h"

// LHCb
// Event/MCEvent
#include "Event/MCVPDigit.h"

// Local
#include "VPRadiationDamageTool.h"

class DeVP;
class MCHit;
class VPChannelID;

/** @class VPDepositCreator VPDepositCreator.h VPDigitisation/VPDepositCreator.h
 *
 *  @author Marcin Kucharczyk
 *  @date   20/09/09
 */

#ifdef DEBUG_HISTO
class VPDepositCreator : public GaudiTupleAlg {
#else
class VPDepositCreator : public GaudiAlgorithm {
#endif

public:
  /// Standard constructor
  VPDepositCreator(const std::string& name,ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPDepositCreator();

  /// Algorithm initialization
  virtual StatusCode initialize();
  /// Algorithm execution
  virtual StatusCode execute();       

private:
  /// Convert G4 energy deposit to charge deposits on pixels
  void createDeposits(LHCb::MCHit* hit);
  /// Draw random number from 1/q^2 distribution
  double randomTail(const double qmin, const double qmax);

  /// Locations of input and output containers
  std::string m_hitLocation;
  std::string m_digitLocation;

  /// Detector element
  DeVP* m_det;
  /// Radiation damage tool
  VPRadiationDamageTool* m_radDamageTool;

  /// Vector of MC digits
  LHCb::MCVPDigits* m_digits;

  /// Distance between points on hit trajectory
  double m_stepSize;
  /// Max. number of points on hit trajectory
  unsigned int m_nMaxSteps;

  /// Number of electrons per micron (MPV) 
  double m_chargeUniform;
  /// Conversion factor between energy deposit and number of electrons
  double m_eVPerElectron;
  /// Lower limit of 1/q^2 distribution
  double m_minChargeTail;

  double m_temperature;
  double m_biasVoltage;
  double m_diffusionCoefficient; 

  /// Flag to simulate radiation damage or not
  bool m_irradiated;
  /// Integrated luminosity in fb-1
  double m_dataTaken;

  Rndm::Numbers m_gauss;
  Rndm::Numbers m_uniform;

  bool m_debug; 
  
};
#endif
