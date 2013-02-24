// Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/SmartRefVector.h"
// Kernel/LHCbKernel
#include "Kernel/VLChannelID.h"
// Event/MCEvent
#include "Event/MCHit.h"
// Local
#include "VLDigitMonitor.h"

using namespace Gaudi::Units;

/** @file VLDigitMonitor.cpp
 *
 *  Implementation of class : VLDigitMonitor
 *
 */

DECLARE_ALGORITHM_FACTORY(VLDigitMonitor)

//=============================================================================
/// Standard constructor, initializes variables
//=============================================================================
VLDigitMonitor::VLDigitMonitor(const std::string& name, 
                               ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator),
    m_det(0),
    m_nDigits(0.),
    m_nDigits2(0.),
    m_nSignal(0),
    m_nNoise(0),
    m_nOther(0),
    m_nEvents(0) {

  declareProperty("Detailed",  m_detailed = false);

}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VLDigitMonitor::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  if (msgLevel(MSG::DEBUG)) debug() << " ==> initialize()" << endmsg;
  m_det = getDet<DeVL>(DeVLLocation::Default);
  setHistoTopDir("VL/");
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VLDigitMonitor::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> execute()" << endmsg;
  ++m_nEvents;
  m_digits = getIfExists<LHCb::MCVLDigits>(LHCb::MCVLDigitLocation::Default);
  if (!m_digits) {
    error() << "No digits at " << LHCb::MCVLDigitLocation::Default << endmsg;
    return StatusCode::FAILURE;
  }
  monitor();
  return StatusCode::SUCCESS;

};

//=============================================================================
///  Finalize
//=============================================================================
StatusCode VLDigitMonitor::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> finalize()" << endmsg;
  double err = 0.;
  if (0 != m_nEvents) {
    m_nDigits  /= m_nEvents;
    m_nDigits2 /= m_nEvents;
    err = sqrt((m_nDigits2 - (m_nDigits * m_nDigits)) / m_nEvents);
  }
  info() << "------------------------------------------------------" << endmsg;
  info() << "                   VLDigitMonitor                     " << endmsg;
  info() << "------------------------------------------------------" << endmsg;
  info() << " Number of digits / event: " << m_nDigits 
         << " +/- " << err << endmsg; 
  const double all = m_nSignal + m_nNoise + m_nOther;
  if (all > 0) {
    info().precision(4);
    info() << "   digits from signal:                      " 
           << (m_nSignal / all) * 100 << "%" << endmsg;
    info() << "   digits from noise:                       " 
           << (m_nNoise / all) * 100 << "%" << endmsg;
    info() << "   digits from other, coupling + spillover: " 
           << (m_nOther / all) * 100 << "%" << endmsg;
  } else {
    info() << " ==> No MCVLDigits found! " << endmsg;
  }
  info() << "------------------------------------------------------" << endmsg;
  return GaudiAlgorithm::finalize();

}

void VLDigitMonitor::monitor() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> monitor()" << endmsg;
  const double size = m_digits->size();
  m_nDigits += size;
  m_nDigits2 += size * size;
  plot(size, "nDigits", "Number of MCVLDigits / event", 0., 10000., 50);
  // Loop over the digits in the container.
  LHCb::MCVLDigits::iterator it;
  for (it = m_digits->begin(); it != m_digits->end(); ++it) {
    LHCb::MCVLDigit* digit = (*it);
    // Check if the digit is signal or noise dominated.
    bool signal = false, noise = false, other = false;
    if (fabs(digit->noise()) > 0. && (digit->signal() == 0.0)) {
      noise = true;
      ++m_nNoise;
    } else if (fabs(digit->signal()) > 0. && (digit->mcHits().size() != 0)) {
      signal = true;
      ++m_nSignal;
    } else if (fabs(digit->signal()) > 0. && (digit->mcHits().size() == 0)) {
      other = true;
      ++m_nOther;
    }
    plot(digit->charge(), "charge", 
         "Charge in Si strip (electrons)",
         -10000., 50000., 100);
    plot(digit->signal(), "signal", 
         "Signal deposited in Si strip (electrons)",
         0., 50000., 100);
    plot(digit->noise(), "noise", 
         "Noise added in Si strip (electrons)",
         -10000., 10000, 100);
    plot(digit->cmnoise(), "cmnoise",
         "Common-Mode noise added in Si strip (electrons)",
         -20000., 20000., 100);
    plot(digit->pedestal(), "pedestal",
         "Pedestal added in Si strip (electrons)",
         0., 50000., 100);
    plot(digit->adc(), "adc",
         "ADC counts", 0., 256., 256);
    plot2D(digit->sensor(), digit->strip(), "sensorAndStrip",
           "Sensor and strip number",
           0., 100., 0., 3000., 100, 50);
     
    if (signal) {
      plot(digit->charge(), "chargeSignal", 
           "Signal dominated - Charge in Si (electrons)",
           -10000., 50000., 100);
      plot(digit->signal(), "signalSignal", 
           "Signal dominated - Signal deposited in Si (electrons)",
           0., 50000., 100);
      plot(digit->noise(), "noiseSignal",
           "Signal dominated - Noise added in Si strip (electrons)",
           -10000., -10000., 100);
      plot2D(digit->sensor(), digit->strip(), "sensorAndStripSignal",
             "Signal dominated - sensor and strip number",
             0., 100., 0., 3000., 100, 50);
    }
    if (noise) {
      plot(digit->charge(), "chargeNoise", 
           "Noise dominated - Charge in Si strip (electrons)",
           -10000., 50000., 100);
      plot(digit->signal(), "signalNoise",
           "Noise dominated - Signal deposited in Si strip (electrons)",
           0., 50000., 100);
      plot(digit->noise(), "noiseNoise",
           "Noise dominated - Noise added in Si strip (electrons)",
           -10000., 10000., 100);
      plot2D(digit->sensor(), digit->strip(), "sensorAndStripNoise",
             "Noise dominated - sensor and strip number",
             0., 100., 0., 3000., 100, 50);
    }
    if (other) {
      plot(digit->charge(), "chargeOther",
           "Other dominated - Charge in Si strip (electrons)",
           -10000., 50000., 100);
      plot(digit->signal(), "signalOther",
           "Other dominated - Signal deposited in Si strip (electrons)",
           0., 50000., 100);
      plot(digit->noise(), "noiseOther",
           "Other dominated - Noise added in Si strip (electrons)",
           -10000., 10000., 100);
      plot2D(digit->sensor(), digit->strip(), "sensorAndStripOther",
             "Other dominated - sensor and strip number",
             0., 100., 0., 3000., 100, 50);
    }
    // Get the MCHits from which the MCVLDigit was made and their weights.
    SmartRefVector<LHCb::MCHit> mcHits = digit->mcHits();
    std::vector<double> mcHitsCharge = digit->mcHitsCharge();
    plot(mcHits.size(), "hitsPerDigit",
         "Number of MC hits / MCVLDigit",
         -0.5, 5.5, 6);
    double totalSignal = 0.;
    std::vector<double>::iterator itch = mcHitsCharge.begin();
    SmartRefVector<LHCb::MCHit>::iterator ith;
    for (ith = mcHits.begin(); ith != mcHits.end(); ++ith) {
      totalSignal += (*itch);
      plot((*itch), "mcHitSignal",
           "MC hit signal deposited in MCVLDigit",
           0., 50000., 100);
      ++itch;
    }
    plot(totalSignal, "mcTotal",
         "MC total signal deposited in MCVLDigit",
         0., 50000., 100);
    if (signal) {
      plot(totalSignal, "mcTotalSignal",
           "Signal dominated - MC total signal deposited in MCVLDigit",
           0., 50000., 100);
    }
    if (noise) {
      plot(totalSignal, "mcTotalNoise",
           "Noise dominated - MC total signal deposited in MCVLDigit",
           0., 50000., 100);
    }
    if (other) {
      plot(totalSignal, "mcTotalOther",
           "Other dominated - MC total signal deposited in MCVLDigit",
           0., 50000., 100);
    }
    if (!m_detailed) continue;
    LHCb::VLChannelID channel = digit->channelID();
    const DeVLSensor* sens = m_det->sensor(digit->channelID().sensor());
    double sensorZ = sens->z() / cm;
    if (sens->isR()) {
      const DeVLRSensor* rSens = dynamic_cast<const DeVLRSensor*>(sens);
      const double radius = rSens->rOfStrip(channel.strip());
      const unsigned int zone = rSens->zoneOfStrip(channel.strip());
      if (0 == zone) {
        plot2D(sensorZ, radius / cm, "rz0",
               "MCVLDigit R position vs. Z (cm), Zone 0",
               -20., 80., 0., 5., 1000, 50);
      } else if (1 == zone) {
        plot2D(sensorZ, radius / cm, "rz1",
               "MCVLDigit R position vs. Z (cm), Zone 1",
               -20., 80., 0., 5., 1000, 50);
      } else if (2 == zone) {
        plot2D(sensorZ, radius / cm, "rz2",
               "MCVLDigit R position vs. Z (cm), Zone 2",
               -20., 80., 0., 5., 1000, 50);
      } else if (3 == zone) {
        plot2D(sensorZ, radius / cm, "rz3",
               "MCVLDigit R position vs. Z (cm), Zone 3",
               -20., 80., 0., 5., 1000, 50);
      } else if (4 == zone) {
        plot2D(sensorZ, radius / cm, "rz4",
               "MCVLDigit R position vs. Z (cm), Zone 4",
               -20., 80., 0., 5., 1000, 50);
      }
    } else if (sens->isPhi()) {
      const unsigned int strip = channel.strip();
      const unsigned int zone = sens->zoneOfStrip(strip);
      const DeVLPhiSensor* phiSens = dynamic_cast<const DeVLPhiSensor*>(sens);
      const double radius = phiSens->rMin(zone);
      const double phi = phiSens->phiOfStrip(strip, 0., radius);
      if (0 == zone) {
        plot2D(sensorZ, phi / degree, "phiz0",
               "MCVLDigit Phi position vs. Z (cm), Zone 0",
               -20., 80., -180., 180., 1000, 60);
      } else if (1 == zone) {
        plot2D(sensorZ, phi / degree, "phiz1",
               "MCVLDigit Phi position vs. Z (cm), Zone 1",
               -20., 80., -180., 180., 1000, 60);
      } else if (2 == zone) {
        plot2D(sensorZ, phi / degree, "phiz2",
               "MCVLDigit Phi position vs. Z (cm), Zone 2",
               -20., 80., -180., 180., 1000, 60);
      }
    }
  }

}

