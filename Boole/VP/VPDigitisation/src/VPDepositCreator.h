#ifndef VPDEPOSITCREATOR_H
#define VPDEPOSITCREATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/RndmGenerators.h"

// LHCb
// Event/MCEvent
#include "Event/MCVPDigit.h"

// Local
#include "VPRadiationDamageTool.h"

class DeVP;
class MCHit;

/** @class VPDepositCreator VPDepositCreator.h VPDigitisation/VPDepositCreator.h
 *  Using MCHits as input, this algorithm simulates the spatial ionisation 
 *  pattern as well as the drift and diffusion of charge carriers in the sensor
 *  and produces an MCVPDigit for each pixel with a charge deposit.
 *
 *  @author Marcin Kucharczyk
 *  @date   20/09/09
 */

class VPDepositCreator : public GaudiTupleAlg {

public:
  /// Standard constructor
  VPDepositCreator(const std::string& name,ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPDepositCreator();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:
  /// Convert G4 energy deposit to charge deposits on pixels
  void createDeposits(LHCb::MCHit* hit);
  /// Draw random number from 1/q^2 distribution
  double randomTail(const double qmin, const double qmax);

  /// Location of input container (MCHits)
  std::string m_hitLocation;
  /// Location of output container (MCVPDigits)
  std::string m_digitLocation;

  /// Detector element
  DeVP* m_det;
  /// Radiation damage tool
  VPRadiationDamageTool* m_radDamageTool;

  /// Container for MCVPDigits
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

  /// Temperature of the sensor [K]
  double m_temperature;
  /// Applied reverse bias voltage [V] 
  double m_biasVoltage;
  /// Diffusion coefficient
  double m_diffusionCoefficient; 

  /// Flag to simulate radiation damage or not
  bool m_irradiated;
  /// Integrated luminosity in fb-1
  double m_dataTaken;

  Rndm::Numbers m_gauss;
  Rndm::Numbers m_uniform;

  /// Flag to activate monitoring histograms or not
  bool m_monitoring;
  
};
#endif
