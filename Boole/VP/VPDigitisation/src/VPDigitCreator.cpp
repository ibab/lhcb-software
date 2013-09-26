//STL
#include <map>

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb
// Event/MCEvent
#include "Event/MCVPDigit.h"
// Event/DigiEvent
#include "Event/VPDigit.h"

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

  declareProperty("InputLocation",  m_inputLocation =  LHCb::MCVPDigitLocation::MCVPDigitLocation);
  declareProperty("OutputLocation", m_outputLocation = LHCb::VPDigitLocation::VPDigitLocation);

  // Discrimination threshold in electrons
  declareProperty("ChargeThreshold", m_threshold = 1000.0);  
  // Tuning of ToT gain
  declareProperty("TunedCharge", m_chargeTuning = 15000.); 
  declareProperty("TunedTOT", m_totTuning = 10.);
  // Number of bits for ToT
  declareProperty("NumberOfBits", m_nBits = 4); 
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

  // Calculate maximal ToT value (saturation)
  if (m_nBits > 0) {
     m_maxToT = (2 << (m_nBits - 1)) - 1;
  } else {
     m_maxToT = 1; 
  }
  // Calculate discharge rate in ns / electron
  m_discharge = (m_totTuning * m_samplePeriod) / (m_chargeTuning - m_threshold); 
  
  if (m_ElectronicNoise > 0.1) {    
    sc = m_gaussDist.initialize(randSvc(), Rndm::Gauss(0.0, 1.0));
    if (!sc) {
      error() << "Random number initialisation failure" << endmsg;
      return sc;
    }
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VPDigitCreator::execute() {
  
  if (m_deadTime) {
    // Update the list of currently inactive pixels
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
  
  LHCb::VPDigits* digits = new LHCb::VPDigits();
  const LHCb::MCVPDigits* mcdigits = getIfExists<LHCb::MCVPDigits>(m_inputLocation);
  if (!mcdigits) {
    error() << "No digits in " << m_inputLocation << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::MCVPDigits::const_iterator it;
  for (it = mcdigits->begin(); it != mcdigits->end(); ++it) {
    // Sum up the charge from all deposits.
    SmartRefVector<LHCb::MCVPDeposit> deposits = (*it)->mcDeposit();
    double charge = 0.;
    SmartRefVector<LHCb::MCVPDeposit>::const_iterator itd;
    for (itd = deposits.begin(); itd != deposits.end(); ++itd) {
      charge += (*itd)->depositedCharge(); 
    }
    // Add electronics noise.
    if (m_ElectronicNoise > 0.1) {
      charge += m_ElectronicNoise * m_gaussDist();
      if (charge < 1.) charge = 1.;
    }
    const LHCb::VPChannelID channel = (*it)->channelID();
    // Calculate time over threshold.
    double tot = timeOverThreshold(charge);
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
    // Calculate ToT counts.
    unsigned int adc = ceil((tot - m_clockPhase) / m_samplePeriod);
    if (adc > m_maxToT) adc = m_maxToT;
    // Create new digit.
    LHCb::VPDigit* digit = new LHCb::VPDigit(adc); 
    digits->insert(digit, channel);
  }
  put(digits, m_outputLocation); 
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Calculate the time (ns) that the pixel stays over threshold, 
/// counting from the LHC clock onwards
//=============================================================================
double VPDigitCreator::timeOverThreshold(const double charge) {

  double timeOverTh = m_clockPhase + (charge - m_threshold) * m_discharge;
  if (timeOverTh < 0.) timeOverTh = 0.;
  return timeOverTh;

}

