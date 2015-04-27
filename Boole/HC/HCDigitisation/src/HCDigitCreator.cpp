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
    m_nStationsB(0), m_nStationsF(0) {

  declareProperty("HitLocation", m_hitLocation = LHCb::MCHitLocation::HC);
  declareProperty("RawEventLocation", 
                  m_rawEventLocation = LHCb::RawEventLocation::Default);

  declareProperty("PhotonsPerMeV", m_photonsPerMeV = 8000.);
  declareProperty("CollectionEfficiency", m_collectionEfficiency = 0.025);
  declareProperty("QuantumEfficiency", m_quantumEfficiency = 0.25);
  declareProperty("Gain", m_gain = 1.e3);
  declareProperty("ElectronsPerADC", m_electronsPerADC = 125000.);
  declareProperty("MaxADC", m_maxADC = 4095);
  declareProperty("Threshold", m_threshold = 2);

  declareProperty("ZPositionsB", m_zB = {-7500., -19700., -114000.});
  declareProperty("ZPositionsF", m_zF = {20000., 114000.});

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

  // Get the number of stations.
  m_nStationsB = m_zB.size();
  m_nStationsF = m_zF.size();
 
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
  constexpr unsigned int nChannelsPerBoard = 64;
  std::vector<double> signalsB(nChannelsPerBoard, 0.);
  std::vector<double> signalsF(nChannelsPerBoard, 0.);
  for (const LHCb::MCHit* hit : *hits) {
    // Determine the station number.
    const double z = hit->midPoint().z();
    unsigned int station = 999;
    if (z > 0.) {
      // Forward stations (Muon side)
      for (unsigned int i = 0; i < m_nStationsF; ++i) {
        const double dz = z - m_zF[i];
        if (fabs(dz) < 20.) {
          station = i + 1;
          break;
        }
      }
    } else {
      // Backward stations (VELO side)
      for (unsigned int i = 0; i < m_nStationsB; ++i) {
        const double dz = z - m_zB[i];
        if (fabs(dz) < 20.) {
          station = i;
          break;
        }
      }
    }
    if (station == 999) {
      warning() << "No station at z = " << z << " mm" << endmsg;
      continue;
    } 
    // Determine the quadrant.
    const double x = hit->midPoint().x();
    const double y = hit->midPoint().y();
    unsigned int quadrant = y < 0. ? 0 : 1;
    if (x > 0.) quadrant += 2;
    // Calculate the channel number (TODO: apply the correct mapping).
    const unsigned int channel = 4 * station + quadrant;
    const double edep = hit->energy() / Gaudi::Units::MeV;
    // Calculate the number of photons.
    const double signal = edep * m_photonsPerMeV * m_collectionEfficiency;
    if (z > 0.) {
      signalsF[channel] = signal;
    } else {
      signalsB[channel] = signal;
    }
    if (!m_monitoring) continue;
    const LHCb::MCParticle* particle = hit->mcParticle();
    if (!particle) continue;
    const double p = particle->p() / Gaudi::Units::MeV; 
    const std::string st = std::to_string(station);
    if (z > 0.) {
      plot(edep, "Edep/F" + st, "Deposited energy [MeV]", 0., 10., 50);
      plot(p, "Momentum/F" + st, "Momentum [MeV/c]", 0., 200., 100);
    } else {
      plot(edep, "Edep/B" + st, "Deposited energy [MeV]", 0., 10., 50);
      plot(p, "Momentum/B" + st, "Momentum [MeV/c]", 0., 200., 100);
    }
  }
  // Convert the signals to ADC counts.
  std::vector<unsigned int> adcsB(nChannelsPerBoard, 0);
  std::vector<unsigned int> adcsF(nChannelsPerBoard, 0);
  for (unsigned int i = 0; i < nChannelsPerBoard; ++i) {
    double nElectrons = signalsB[i] * m_quantumEfficiency * m_gain;
    auto adc = static_cast<unsigned int>(floor(nElectrons / m_electronsPerADC));
    if (adc > m_maxADC) adc = m_maxADC;
    adcsB[i] = adc;
    nElectrons = signalsF[i] * m_quantumEfficiency * m_gain;
    adc = static_cast<unsigned int>(floor(nElectrons / m_electronsPerADC));
    if (adc > m_maxADC) adc = m_maxADC;
    adcsF[i] = adc;
    if (msgLevel(MSG::DEBUG)) {
      debug() << format("Channel: %02u (B): ADC = %06i", i, adcsB[i]) << endmsg;
      debug() << format("Channel: %02u (F): ADC = %06i", i, adcsF[i]) << endmsg;
    }
  }
  // Encode the data.
  std::vector<unsigned int> words;
  encodeV3(0, adcsB, words);
  encodeV3(1, adcsF, words);
  // Add a new raw bank and pass memory ownership to the raw event.
  const unsigned int version = 3;
  rawEvent->addBank(0, LHCb::RawBank::HC, version, words); 
  return StatusCode::SUCCESS;
}

//=========================================================================
// Raw bank encoding (PRS/SPD format) 
//=========================================================================
void HCDigitCreator::encodeV3(const unsigned int card,
                              const std::vector<unsigned int>& adcs,
                              std::vector<unsigned int>& words) {

  // Add the header word.
  const unsigned int headerIndex = words.size();
  words.push_back(card << 14);
  unsigned int lenAdc = 0;
  unsigned int lenTrig = 0;
  // Add the trigger words. 
  const unsigned int nChannels = adcs.size();
  int offset = 0;
  unsigned int word = 0;
  for (unsigned int i = 0; i < nChannels; ++i) {
    int mask = 0;
    if (adcs[i] > m_threshold) mask |= 0x40;
    // if (spd) mask |= 0x80;
    if (0 == mask) continue;
    mask |= i;
    mask = mask << offset;
    word |= mask;
    offset += 8;
    if (32 == offset) {
      words.push_back(word);
      word = 0;
      offset = 0;
    }
    ++lenTrig;
  }
  if (0 != word) {
    words.push_back(word);
    word = 0;
  }
  // Add the ADC words.
  for (unsigned int i = 0; i < nChannels; ++i) {
    if (adcs[i] == 0) continue;
    const unsigned int adc = (adcs[i] & 0x3FF) | (i << 10);
    if (0 == word) {
      word = adc;
    } else {
      word |= (adc << 16);
      words.push_back(word);
      word = 0;
    }
    ++lenAdc;
  }
  if (0 != word) {
    words.push_back(word);
    word = 0;
  }
  // Add the lengths of trigger and ADC parts to the header. 
  words[headerIndex] |= (lenAdc << 7) + lenTrig;

}
