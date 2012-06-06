#ifndef VELOLITE_DIGIT_CREATOR_H
#define VELOLITE_DIGIT_CREATOR_H 1

#include <cmath>
// STL
#include <string>
// GSL
#include "gsl/gsl_sf_erf.h"
#include "Kernel/FPEGuard.h"
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Point3DTypes.h"
// Event/MCEvent
#include "Event/MCVeloLiteDigit.h"

class DeVeloLite;
class MCHit;
class VeloLiteChannelID;
class ISiAmplifierResponse;
class IStripNoiseTool;

/** @class VeloLiteDigitCreator VeloLiteDigitCreator.h 
 *
 * 
 */

class VeloLiteDigitCreator : public GaudiAlgorithm {

public:
  /// Constructor
  VeloLiteDigitCreator(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VeloLiteDigitCreator();

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution
  virtual StatusCode finalize();   ///< Algorithm finalise

private:

  /// Simulate charge deposition between entry and exit of MCHit
  void simulateSignal(LHCb::MCHits* hits, double timeOffset);
  /// Simulate capacitive coupling
  void simulateCoupling();
  /// Add noise to the strips
  void simulateNoise();
  /// Simulate random dead strips
  void simulateInefficiency();
  /// Digitisation
  void digitise();
  /// Remove any digits below threshold
  void cleanup(); 
  /// Put the digits on the TES
  StatusCode storeDigits(); 

  /// Calculate the number of points to deposit charge 
  /// along the trajectory of the hit
  int numberOfPoints(LHCb::MCHit* hit);
  /// Spread the charge over the points to simulate
  void chargePerPoint(LHCb::MCHit* hit,
                      std::vector<double>& points, double timeOffset);
  /// Add delta ray charges if required
  void deltaRayCharge(double total, double equal, double tol, 
                      std::vector<double>& points);
  /// Allow the deposited charge points to diffuse to the strips
  void diffusion(LHCb::MCHit* hit, std::vector<double>& points);
  /// Sample noise of a strip
  double noiseValue(double noiseSigma);
  /// Sample noise to add to an otherwise empty strip
  double noiseValueTail(double noiseSigma, double threshold);
  /// Add charge to a digit with link to MCHit 
  void fill(LHCb::MCVeloLiteDigit* digit, LHCb::MCHit* hit, double charge);
  /// Add charge to a digit without link to MCHit
  void fill(LHCb::MCVeloLiteDigit* digit, double charge) {
    fill(digit, 0, charge);
  }
  /// Find the strip with a given key.
  LHCb::MCVeloLiteDigit* findOrInsert(LHCb::VeloLiteChannelID& channel);
  /// Find the strip with the previous key (or create a new one).
  LHCb::MCVeloLiteDigit* findOrInsertPrevStrip(LHCb::MCVeloLiteDigits::iterator it, 
                                               bool& valid, bool& create);
  /// Find the strip with the next key (or create a new one).
  LHCb::MCVeloLiteDigit* findOrInsertNextStrip(LHCb::MCVeloLiteDigits::iterator it, 
                                               bool& valid, bool& create);

  /// Get the effective charge fraction for this hit 
  /// compared to an in time main bunch hit
  double chargeTimeFactor(double tof, double bunchOffset, double z);

  /// gsl_erf_sf_q protected against floating point exceptions 
  double safe_gsl_sf_erf_Q(double const &arg) const {
    if (fabs(arg) > 10.) return (arg > 0. ? 0. : 1.);
    // Turn off the inexact FPE
    FPE::Guard reducedFPE(FPE::Guard::mask("Inexact"), true);
    return gsl_sf_erf_Q(arg);
  }
  /// Poisson distributions protected against floating point exceptions
  int poissonDistR() {
    // Turn off FPEs
    FPE::Guard allFPE(FPE::Guard::mask("AllExcept"), true);
    return int(m_poissonDistR());
  }
  int poissonDistPhi() {
    // Turn off FPEs
    FPE::Guard allFPE(FPE::Guard::mask("AllExcept"), true);
    return int(m_poissonDistPhi());
  }

  /// Names and time offsets of input containers
  std::vector<std::string> m_inputContainers;
  std::vector<double> m_inputTimeOffsets; 
  /// Names of output containers
  std::vector<std::string> m_outputContainers;
  /// Name of MCHit container to make links to
  std::string m_MCHitContainerToLinkName; 
  /// MCHit container to make links to
  ObjectContainerBase* m_MCHitContainerToLink; 
  /// Pointer to detector element 
  DeVeloLite* m_det; 
  LHCb::MCVeloLiteDigits* m_digits; 
  LHCb::MCVeloLiteDigits* m_digits_coupling; 

  /// Flag to simulate unequal charge along track
  bool m_inhomogeneousCharge; 
  /// Flag to simulate capacitive coupling
  bool m_coupling;            
  /// Flag to simulate noise in sensors of main Velo
  bool m_noiseSim;
  /// Flag to simulate noise in pile-up sensors            
  bool m_simNoisePileUp;      
  /// Fraction of strips to randomly kill
  double m_stripInefficiency; 

  /// Gaussian random numbers
  Rndm::Numbers m_gaussDist;
  /// Uniform random numbers
  Rndm::Numbers m_uniformDist;
  /// Poisson random numbers 
  /// (mean corresponds to average number of noise strips in a sensor)
  Rndm::Numbers m_poissonDistR;
  Rndm::Numbers m_poissonDistPhi;
  /// Random numbers from 1/E^2
  double ran_inv_E2(double emin, double emax); 
  /// Random numbers from a gaussian tail
  double ran_gaussian_tail(const double a, const double sigma);

  /// Threshold charge in electrons
  double m_threshold;

  /// Temperature in eV (used for evaluating the expected diffusion width)
  double m_kT;
  /// Voltage applied to (over)depleted sensor
  double m_biasVoltage;
  /// Amount of energy required to make an electron-hole pair in silicon
  double m_eVPerElectron;

  /// Number of points to distribute charge on per strip
  int m_simulationPointsPerStrip;
  /// Use same charge along strip at each seeding centre
  double m_chargeUniform;
  double m_deltaRayMinEnergy;
  /// Fraction of charge shared due to capacitive coupling
  double m_capacitiveCoupling;
  /// Average noise per strip (to estimate number of noisy strips)
  double m_averageStripNoiseR;
  double m_averageStripNoisePhi;
  /// Base size of charge diffusion 
  double m_baseDiffuseSigma;    

  /// Peaking time of the amplifier response function
  double m_pulseShapePeakTime;  
  /// Time offset of sampling with respect to peaking time
  double m_offPeakSamplingTime; 

  /// Scale factor for noise
  double m_scaleNoise;          
  /// Scale factor for Gaussian flucuations in charge at points
  double m_scaleFluctuations;   

  /// Conversion from ADC to electrons
  double m_electronsPerADC;     
  /// Maximum possible ADC value
  int m_maxADCOutput;            
  /// Minimum possible ADC value
  int m_minADCOutput; 

  /// Tool for emulating amplifier response
  ISiAmplifierResponse* m_SiTimeTool;
  /// Tool for estimating strip noise
  IStripNoiseTool* m_StripNoiseTool;
 
  /// Number of digits created
  unsigned int m_nDigitsTotal;
  /// Number of digits removed due to strip inefficiency 
  unsigned int m_nDigitsKilled;

  /// Output level
  bool m_debug; 
  bool m_verbose;
  
};
#endif
