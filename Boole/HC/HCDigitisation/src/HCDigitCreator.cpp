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
    GaudiTupleAlg(name, pSvcLocator),
    m_nStations(0), m_nChannels(0) {

  declareProperty("HitLocation", m_hitLocation = LHCb::MCHitLocation::HC);
  declareProperty("RawEventLocation", 
                  m_rawEventLocation = LHCb::RawEventLocation::Default);

  declareProperty("PhotonsPerMeV", m_photonsPerMeV = 8000.);
  declareProperty("CollectionEfficiency", m_collectionEfficiency = 0.025);
  declareProperty("QuantumEfficiency", m_quantumEfficiency = 0.25);
  declareProperty("Gain", m_gain = 1.e3);
  declareProperty("ElectronsPerADC", m_electronsPerADC = 125000.);
  declareProperty("MaxADC", m_maxADC = 4095);

  declareProperty("ZPositions", 
                  m_z = {-7500., -19700., -114000., 20000., 114000.});

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

  // Get the number of stations and channels.
  m_nStations = m_z.size();
  m_nChannels = 4 * m_nStations;
 
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
  if (!rawEvent) {
    // Create the raw event.
    rawEvent = new LHCb::RawEvent();
    put(rawEvent, m_rawEventLocation);
  }

  // Loop over the hits and sum up the signals in each quadrant.
  std::vector<double> signals(m_nChannels, 0.);
  for (const LHCb::MCHit* hit : *hits) {
    // Determine the station number.
    const double z = hit->midPoint().z();
    unsigned int station = 999;
    for (unsigned int i = 0; i < m_nStations; ++i) {
      const double dz = z - m_z[i];
      if (fabs(dz) < 20.) {
        station = i;
        break;
      }
    }
    if (station >= m_nStations) {
      warning() << "No station at z = " << z << " mm" << endmsg;
      continue;
    } 
    // Determine the quadrant.
    unsigned int quadrant = 0;
    const double x = hit->midPoint().x();
    const double y = hit->midPoint().y();
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
    const double edep = hit->energy() / Gaudi::Units::MeV;
    // Calculate the number of photons.
    signals[channel] += edep * m_photonsPerMeV * m_collectionEfficiency; 
    if (!m_monitoring) continue;
    const LHCb::MCParticle* particle = hit->mcParticle();
    if (!particle) continue;
    const double p = particle->p() / Gaudi::Units::MeV; 
    plot(p, "Momentum", "Momentum [MeV/c]", 0., 200., 100);
    const std::string st = std::to_string(station);
    plot(edep, "Edep/S" + st, "Deposited energy [MeV]", 0., 10., 50);
  }
  std::vector<unsigned int> words(m_nChannels, 0);
  for (auto it = signals.cbegin(), end = signals.cend(); it != end; ++it) {
    const unsigned int channel = it - signals.cbegin();
    const double nElectrons = (*it) * m_quantumEfficiency * m_gain;
    // Convert the signal to ADC counts.
    auto adc = static_cast<unsigned int>(floor(nElectrons / m_electronsPerADC));
    if (adc > m_maxADC) adc = m_maxADC;
    words[channel] = (channel << 16) + (adc & 0xFFFF);
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Channel: " << channel << ", ADC: " << adc << endmsg;
    }
  }
  // Get the size of the raw bank.
  const unsigned int nBytes = sizeof(unsigned int) * m_nStations;
  const unsigned int version = 1;
  // Add a new raw bank and pass memory ownership to the raw event.
  LHCb::RawBank* bank = rawEvent->createBank(0, LHCb::RawBank::HC, version, 
                                             nBytes, &(words[0]));
  rawEvent->adoptBank(bank, true);
  return StatusCode::SUCCESS;

}

