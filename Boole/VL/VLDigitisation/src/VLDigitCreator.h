#ifndef VL_DIGIT_CREATOR_H
#define VL_DIGIT_CREATOR_H 1

// STL
#include <vector>
#include <string>
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
// Event/MCEvent
#include "Event/MCVLDigit.h"

class DeVL;
class ISiAmplifierResponse;
class IStripNoiseTool;

/** @class VLDigitCreator VLDigitCreator.h 
 *
 * Simulate response of sensor and front-end electronics to 
 * charge deposition due to MC hits. 
 */

class VLDigitCreator : public GaudiAlgorithm {

public:
  /// Constructor
  VLDigitCreator(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VLDigitCreator() {}

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution
  virtual StatusCode finalize();   ///< Algorithm finalise

private:

  /// Simulate charge deposition between entry and exit of MC hit
  void simulateSignal(LHCb::MCHits* hits, double timeOffset);
  /// Simulate capacitive coupling
  void simulateCoupling();
  /// Add noise to the strips
  void simulateNoise();
  /// Simulate signal loss due to dead strips
  void simulateInefficiency();
  /// Convert charge to ADC counts
  void digitise();
  /// Remove digits below threshold
  void cleanup(); 

  /// Allocate the deposited charge to points in the sensor
  bool deposit(LHCb::MCHit* hit,
               std::vector<double>& points, double timeOffset);
  /// Add delta ray charges if required
  void deltaRayCharge(double total, double equal, double tol, 
                      std::vector<double>& points);
  /// Allow the deposited charge points to diffuse to the strips
  void drift(LHCb::MCHit* hit, std::vector<double>& points);

  /// Add charge to a digit (possibly with link to MCHit) 
  void addSignal(LHCb::MCVLDigit* digit, LHCb::MCHit* hit, double charge);

  /// Find or create the digit with a given key.
  LHCb::MCVLDigit* getDigit(LHCb::VLChannelID& channel, const bool create);
  /// Find the strip with the previous key (or create a new one).
  LHCb::MCVLDigit* getDigitPrev(LHCb::MCVLDigits::iterator it, 
                                bool& valid, bool& create);
  /// Find the strip with the next key (or create a new one).
  LHCb::MCVLDigit* getDigitNext(LHCb::MCVLDigits::iterator it, 
                                bool& valid, bool& create);

  /// Complementary error function protected against floating point exceptions
  double erfcSafe(const double arg);
  /// Draw random numbers from an 1 / E^2 distribution
  double ranInvSquare(double emin, double emax); 
  /// Draw random numbers from a gaussian tail distribution
  double ranGaussianTail(const double a, const double sigma);

  /// Names and time offsets of input containers
  std::vector<std::string> m_hitLocations;
  std::vector<double> m_timeOffsets; 
  /// Name of MCHit container to make links to
  std::string m_MCHitContainerToLinkName; 
  /// MCHit container to make links to
  ObjectContainerBase* m_MCHitContainerToLink; 
  // Name of output container
  std::string m_digitLocation;

  /// Pointer to detector element 
  DeVL* m_det; 
  LHCb::MCVLDigits* m_digits; 
  LHCb::MCVLDigits* m_digitsCoupling; 

  /// Flag to simulate unequal charge along track
  bool m_inhomogeneousCharge; 
  /// Flag to simulate noise
  bool m_noise;
  /// Fraction of charge shared due to capacitive coupling
  double m_coupling;
  /// Fraction of strips to randomly kill
  double m_inefficiency; 

  /// Gaussian random numbers
  Rndm::Numbers m_gauss;
  /// Uniform random numbers
  Rndm::Numbers m_uniform;
  /// Poisson random numbers 
  /// (mean corresponds to average number of noise strips in a sensor)
  Rndm::Numbers m_poissonR;
  Rndm::Numbers m_poissonPhi;

  /// Threshold charge in electrons
  double m_threshold;

  /// Temperature in eV (used for evaluating the expected diffusion width)
  double m_kT;
  /// Voltage applied to (over)depleted sensor
  double m_biasVoltage;
  /// Amount of energy required to make an electron-hole pair in silicon
  double m_eVPerElectron;

  /// Number of points to distribute charge on per strip
  int m_pointsPerStrip;
  /// Use same charge along strip at each seeding centre
  double m_chargeUniform;
  double m_deltaRayMinEnergy;
  /// Base size of charge diffusion 
  double m_baseDiffuseSigma;    

  /// Peaking time of the amplifier response function
  double m_peakTime;  
  /// Time offset of sampling with respect to peaking time
  double m_offPeakSamplingTime; 

  /// Scale factor for noise
  double m_scaleNoise;          
  /// Scale factor for Gaussian flucuations in charge at points
  double m_scaleFluctuations;   

  /// Conversion factor between ADC counts and number of electrons
  double m_electronsPerAdc;     
  /// Range of ADC values
  int m_adcRange;            

  /// Tool for emulating amplifier response
  ISiAmplifierResponse* m_SiTimeTool;
  /// Tool for estimating strip noise
  IStripNoiseTool* m_StripNoiseTool;
 
  /// Output level
  bool m_debug; 
  bool m_verbose;
  
};
#endif
