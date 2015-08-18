// LHCb
// Event/MCEvent
#include "Event/MCVPDigit.h"
// Event/DigiEvent
#include "Event/VPDigit.h"
// Kernel/LHCbKernel
#include "Kernel/VPConstants.h"
#include "Kernel/VPDataFunctor.h"

// Local
#include "VPDigitCreator.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPDigitCreator
//
// 2010-07-07 : Thomas Britton
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPDigitCreator)

//=============================================================================
// Standard constructor
//=============================================================================
VPDigitCreator::VPDigitCreator(const std::string& name, 
                               ISvcLocator* pSvcLocator) :
    GaudiTupleAlg(name, pSvcLocator),
    m_nNoisy(0) {

  // Where to pick up the MCVPDigits
  declareProperty("InputLocation",  m_inputLocation =  LHCb::MCVPDigitLocation::Default);
  // Where to put the VPDigits
  declareProperty("OutputLocation", m_outputLocation = LHCb::VPDigitLocation::Default);

  // Discrimination threshold in electrons
  declareProperty("ChargeThreshold", m_threshold = 1000.0);  
  // Threshold dispersion in electrons
  declareProperty("ElectronicNoise", m_electronicNoise = 130.0); 

  // Simulate pixel dead time or not
  declareProperty("DeadTime", m_deadTime = false);  
  // Bunch spacing in ns 
  declareProperty("BunchCrossingSpacing", m_bunchCrossingSpacing = 25.0);
  // Discharge rate in electrons per ns (used in dead time simulation)
  declareProperty("DischargeRate", m_dischargeRate = 200.0); 

  // Simulate masked pixels or not
  declareProperty("MaskedPixels", m_maskedPixels = false);
  // Simulate noisy pixels or not
  declareProperty("NoisyPixels", m_noisyPixels = false);
  // Fraction of masked pixels
  declareProperty("FractionMasked", m_fractionMasked = 0.0);
  // Fraction of noisy pixels
  declareProperty("FractionNoisy", m_fractionNoisy = 0.0);

}

//=============================================================================
// Destructor
//=============================================================================
VPDigitCreator::~VPDigitCreator() {}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VPDigitCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;  

  // Calculate discharge per bunch-crossing.
  m_dischargeRate *= m_bunchCrossingSpacing;

  // Initialize the random number generators.
  sc = m_gauss.initialize(randSvc(), Rndm::Gauss(0., 1.));
  if (!sc) {
    error() << "Cannot initialize Gaussian random number generator." << endmsg;
    return sc;
  }
  sc = m_uniform.initialize(randSvc(), Rndm::Flat(0., 1.));
  if (!sc) {
    error() << "Cannot initialize uniform random number generator." << endmsg;
    return sc;
  }
  if (m_noisyPixels) {
    // Calculate the total number of noisy pixels. 
    m_nNoisy = int(m_fractionNoisy * VP::NSensors * VP::NPixelsPerSensor);
    if (m_nNoisy < 1) m_noisyPixels = false;
  }
  setHistoTopDir("VP/");
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VPDigitCreator::execute() {
  
  if (m_deadTime) {
    // Update the list of currently inactive pixels.
    std::map<const VPChannelID, double>::iterator itp;
    for (itp = m_deadPixels.begin(); itp != m_deadPixels.end(); ++itp) {
      m_deadPixels[itp->first] -= m_dischargeRate;
      if (msgLevel(MSG::DEBUG)) {
        debug() << (itp->first).channelID() << " " << itp->second << endmsg;
      }
      if (m_deadPixels[itp->first] < m_threshold) {
        std::map<const VPChannelID, double>::iterator here = itp++;
        m_deadPixels.erase(here);
      }
    }
  }

  // Pick up the MC digits.
  const LHCb::MCVPDigits* mcdigits = getIfExists<LHCb::MCVPDigits>(m_inputLocation);
  if (!mcdigits) {
    error() << "No digits in " << m_inputLocation << endmsg;
    return StatusCode::FAILURE;
  }

  // Create a container for the digits and transfer ownership to the TES. 
  LHCb::VPDigits* digits = new LHCb::VPDigits();
  put(digits, m_outputLocation); 

  // Loop over the MC digits.
  LHCb::MCVPDigits::const_iterator it;
  for (it = mcdigits->begin(); it != mcdigits->end(); ++it) {
    // Sum up the collected charge from all deposits.
    const std::vector<double>& deposits = (*it)->deposits();
    const double charge = std::accumulate(deposits.begin(), deposits.end(), 0.);
    // Check if the collected charge exceeds the threshold.
    if (charge < m_threshold + m_electronicNoise * m_gauss()) continue;
    const LHCb::VPChannelID channel = (*it)->channelID();
    if (m_deadTime) {
      // Check if the pixel is already over threshold.
      if (m_deadPixels.count(channel) == 1) {
        m_deadPixels[channel] += charge;
        continue;
      } else {
        // Add the pixel to the list of dead pixels.
        m_deadPixels[channel] = charge;
      } 
    }
    if (m_maskedPixels) {
      // Draw a random number to choose if this pixel is masked.
      if (m_uniform() < m_fractionMasked) continue;
    }
    // Create a new digit.
    digits->insert(new LHCb::VPDigit(), channel);
  }

  if (m_noisyPixels) {
    // Add additional digits without a corresponding MC digit.
    for (unsigned int i = 0; i < m_nNoisy; ++i) {
      // Sample the sensor, chip, column and row of the noise pixel.
      const int sensor = int(floor(m_uniform() * VP::NSensors));
      const int chip = int(floor(m_uniform() * VP::NChipsPerSensor));
      const int col = int(floor(m_uniform() * VP::NColumns));
      const int row = int(floor(m_uniform() * VP::NRows));
      LHCb::VPChannelID channel(sensor, chip, col, row);
      // Make sure the channel has not yet been added to the container.
      if (digits->object(channel)) continue;
      digits->insert(new LHCb::VPDigit(), channel);
    }
  }

  // Sort the digits by channel ID.
  std::stable_sort(digits->begin(), digits->end(),
                   VPDataFunctor::Less_by_Channel<const LHCb::VPDigit*>());
  return StatusCode::SUCCESS;

}

