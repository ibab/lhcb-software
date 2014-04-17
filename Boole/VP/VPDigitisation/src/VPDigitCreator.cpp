// STL
#include <map>

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb
// Event/MCEvent
#include "Event/MCVPDigit.h"
// Event/DigiEvent
#include "Event/VPDigit.h"
// Kernel/LHCbKernel
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
/// Standard constructor
//=============================================================================
VPDigitCreator::VPDigitCreator(const std::string& name, 
                               ISvcLocator* pSvcLocator) :
#ifdef DEBUG_HISTO
    GaudiTupleAlg(name, pSvcLocator),
#else
    GaudiAlgorithm(name, pSvcLocator),
#endif
    m_debug(false) {

  declareProperty("InputLocation",  m_inputLocation =  LHCb::MCVPDigitLocation::Default);
  declareProperty("OutputLocation", m_outputLocation = LHCb::VPDigitLocation::Default);

  // Discrimination threshold in electrons
  declareProperty("ChargeThreshold", m_threshold = 1000.0);  
  // Tuning of ToT gain
  declareProperty("TunedCharge", m_chargeTuning = 15000.); 
  declareProperty("TunedTOT", m_totTuning = 10.);
  // Bunch spacing in ns 
  declareProperty("BunchCrossingSpacing", m_bunchCrossingSpacing = 25.0);
  // Sampling phase in ns w. r. t. LHC clock (tuned to largest timewalk)
  declareProperty("ClockPhase", m_clockPhase = 12.0); 
  // Sampling period of the ToT counter in ns
  declareProperty("SamplingPeriod", m_samplePeriod = 25.0);
  // Electronic noise in electrons
  declareProperty("ElectronicNoise", m_ElectronicNoise = 100.0);  
  // Include pixel dead time in the simulation or not
  declareProperty("DeadTime", m_deadTime = false);  

}

//=============================================================================
/// Destructor
//=============================================================================
VPDigitCreator::~VPDigitCreator() {}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VPDigitCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;  
  m_debug = msgLevel(MSG::DEBUG);
  if (m_debug) debug() << "==> Initialize" << endmsg;

  // Calculate discharge rate in ns / electron
  m_discharge = (m_totTuning * m_samplePeriod) / (m_chargeTuning - m_threshold); 
  
  if (m_ElectronicNoise > 0.1) {    
    sc = m_gaussDist.initialize(randSvc(), Rndm::Gauss(0.0, 1.0));
    if (!sc) {
      error() << "Random number initialisation failure" << endmsg;
      return sc;
    }
  }
#ifdef DEBUG_HISTO
  setHistoTopDir("VP/");
#endif
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
      m_deadPixels[itp->first] -= m_bunchCrossingSpacing;
      if (m_debug) {
        debug() << (itp->first).channelID() << " " << itp->second << endmsg;
      }
      if (m_deadPixels[itp->first] <= 0) {
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
    // Sum up the charge from all deposits.
    const std::vector<double>& deposits = (*it)->deposits();
    double charge = std::accumulate(deposits.begin(), deposits.end(), 0.);
    // Add electronics noise.
    if (m_ElectronicNoise > 0.1) {
      charge += m_ElectronicNoise * m_gaussDist();
      if (charge < 1.) charge = 1.;
    }
    const LHCb::VPChannelID channel = (*it)->channelID();
    // Calculate time over threshold.
    const double tot = timeOverThreshold(charge);
    // Check if pixel is already over threshold.
    if (m_deadTime) {
      if (m_deadPixels.count(channel) == 1) {
        m_deadPixels[channel] += tot;
        continue;
      } 
    }
    // Skip hits below threshold.
    if (charge < m_threshold) continue;
    // Update dead time for this pixel.
    if (m_deadTime) m_deadPixels[channel] = tot;
    // Create new digit.
    LHCb::VPDigit* digit = new LHCb::VPDigit(); 
    digits->insert(digit, channel);
  }

  // Sort the digits by channel ID.
  std::stable_sort(digits->begin(), digits->end(),
                   VPDataFunctor::Less_by_Channel<const LHCb::VPDigit*>());
  return StatusCode::SUCCESS;

}

