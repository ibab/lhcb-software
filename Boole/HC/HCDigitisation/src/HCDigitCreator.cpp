// STL
#include <vector>
#include <map>

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// LHCb
// Event/MCEvent
#include "Event/MCHit.h"
// Event/DAQEvent
#include "Event/RawEvent.h"

// Local
#include "HCDigitCreator.h"

DECLARE_ALGORITHM_FACTORY(HCDigitCreator)

//=============================================================================
// Constructor
//=============================================================================
HCDigitCreator::HCDigitCreator(const std::string& name, 
                               ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator) {

  declareProperty("HitLocation", m_hitLocation = LHCb::MCHitLocation::HC);
  declareProperty("RawEventLocation", 
                  m_rawEventLocation = LHCb::RawEventLocation::Default);

  declareProperty("PhotonsPerMeV", m_photonsPerMeV = 8000.);
  declareProperty("CollectionEfficiency", m_collectionEfficiency = 0.04);
  declareProperty("QuantumEfficiency", m_quantumEfficiency = 0.15);
  declareProperty("Gain", m_gain = 1.e3);
  declareProperty("ElectronsPerADC", m_electronsPerADC = 2.5e4); 
  declareProperty("MaxADC", m_maxADC = 4095);

  declareProperty("Monitoring", m_monitoring = false);

}

//=============================================================================
// Destructor
//=============================================================================
HCDigitCreator::~HCDigitCreator() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCDigitCreator::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()) return sc;

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

  setHistoTopDir("HC/");
  return StatusCode::SUCCESS;

}

//=========================================================================
// Main execution
//=========================================================================
StatusCode HCDigitCreator::execute() {

  // Pick up the MC hits.
  const LHCb::MCHits* hits = getIfExists<LHCb::MCHits>(m_hitLocation);
  if (!hits) {
    error() << "No hits in " << m_hitLocation << endmsg;
    return StatusCode::FAILURE;
  }

  // Check if the raw event exists.
  LHCb::RawEvent* rawEvent = getIfExists<LHCb::RawEvent>(m_rawEventLocation);
  if (NULL == rawEvent) {
    // Create the raw event.
    rawEvent = new LHCb::RawEvent();
    put(rawEvent, m_rawEventLocation);
  }

  // Loop over the hits and calculate the front-end signal.
  LHCb::MCHits::const_iterator ith;
  std::map<unsigned int, double> signals;
  for (ith = hits->begin(); ith != hits->end(); ++ith) {
    // TODO: use detector element to get station, quadrant and channel.
    // Determine the station number.
    const double z = (*ith)->midPoint().z() / Gaudi::Units::m;
    unsigned int station = 0;
    const double tol = 0.2;
    if (fabs(z + 7.5) < tol) {
      station = 0;
    } else if (fabs(z + 19.7) < tol) {
      station = 1;
    } else if (fabs(z + 114.) < tol) {
      station = 2;
    } else if (fabs(z - 20.) < tol) {
      station = 3;
    } else if (fabs(z - 114.) < tol) {
      station = 4;
    } else {
      warning() << "No station at " << z << endmsg;
      continue;
    } 
    // Determine the quadrant.
    unsigned int quadrant = 0;
    const double x = (*ith)->midPoint().x();
    const double y = (*ith)->midPoint().y();
    if (x < 0.) {
      if (y < 0.) {
        quadrant = 0;
      } else {
        quadrant = 1;
      }
    } else {
      if (y < 0.) {
        quadrant = 2;
      } else {
        quadrant = 3;
      }
    } 
    // Calculate the channel number.
    const unsigned int channel = 4 * station + quadrant;
    const double edep = (*ith)->energy() / Gaudi::Units::MeV;
    // Calculate the number of photons and photo-electrons.
    const double nPhotons = edep * m_photonsPerMeV * m_collectionEfficiency;
    const double nPhotoElectrons = nPhotons * m_quantumEfficiency;
    signals[channel] += nPhotoElectrons * m_gain;
    if (m_monitoring) {
      plot(edep, "Edep", "Deposited energy [MeV]", 0., 10., 50);
      plot(nPhotoElectrons, "NPEs", "Photoelectrons / hit", 0., 1000., 100); 
    }
  }
  std::map<unsigned int, double>::const_iterator itm;
  std::vector<unsigned int> words;
  for (itm = signals.begin(); itm != signals.end(); ++itm) {
    const unsigned int channel = (*itm).first;
    // Convert the signal to ADC counts.
    unsigned int adc = static_cast<unsigned int>(floor((*itm).second / m_electronsPerADC));
    if (adc > m_maxADC) adc = m_maxADC;
    const unsigned int word  = (channel << 16) + (adc & 0xFFFF); 
    words.push_back(word);
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Channel: " << channel << ", ADC: " << adc << endmsg;
    }
  }
  // Get the size of the raw bank.
  const unsigned int nBytes = sizeof(unsigned int) * words.size();
  const unsigned int version = 1;
  // Add a new raw bank and pass memory ownership to the raw event.
  LHCb::RawBank* bank = rawEvent->createBank(0, LHCb::RawBank::HC, version, 
                                             nBytes, &(words[0]));
  rawEvent->adoptBank(bank, true);
  return StatusCode::SUCCESS;

}

