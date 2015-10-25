// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// LHCb
// Event/MCEvent
#include "Event/MCHit.h"
// Event/DAQEvent
#include "Event/RawEvent.h"
#include "Event/GenCollision.h"

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

  declareProperty("CrateB", m_crateB = 0);
  declareProperty("CrateF", m_crateF = 1);

  declareProperty("ChannelsB0", m_channelsB0);
  declareProperty("ChannelsB1", m_channelsB1);
  declareProperty("ChannelsB2", m_channelsB2);
  declareProperty("ChannelsF1", m_channelsF1);
  declareProperty("ChannelsF2", m_channelsF2);

  declareProperty("ZPositionsB", m_zB = {-7500., -19700., -114000.});
  declareProperty("ZPositionsF", m_zF = {20000., 114000.});

  declareProperty("PhotonsPerMeV", m_photonsPerMeV = 8000.);
  declareProperty("CollectionEfficiency", m_collectionEfficiency = 0.025);
  declareProperty("QuantumEfficiency", m_quantumEfficiency = 0.25);
  declareProperty("Gain", m_gain = 1.e3);
  declareProperty("ElectronsPerADC", m_electronsPerADC = 5000.);
  declareProperty("MaxADC", m_maxADC = 1023);
  declareProperty("Threshold", m_threshold = 2);

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
  if (sc.isFailure()) {
    return Error("Cannot initialize Gaussian random number generator.", sc);
  }
  sc = m_uniform.initialize(randSvc(), Rndm::Flat(0., 1.));
  if (sc.isFailure()) {
    return Error("Cannot initialize uniform random number generator.", sc);
  }
  // Set up the channel maps.
  mapChannels(m_channelsB0, 0, true);
  mapChannels(m_channelsB1, 1, true);
  mapChannels(m_channelsB2, 2, true);
  mapChannels(m_channelsF1, 1, false);
  mapChannels(m_channelsF2, 2, false);

  // Compute the conversion factor between energy deposit and photo electrons.
  m_pePerMeV = m_photonsPerMeV * m_collectionEfficiency * m_quantumEfficiency;
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
    return Error("No hits in " + m_hitLocation);
  }

  // Check if the raw event exists.
  LHCb::RawEvent* rawEvent = getIfExists<LHCb::RawEvent>(m_rawEventLocation);
  if (!rawEvent) {
    // Create the raw event.
    rawEvent = new LHCb::RawEvent();
    put(rawEvent, m_rawEventLocation);
  }

  const LHCb::GenCollisions* collisions = getIfExists<LHCb::GenCollisions>(LHCb::GenCollisionLocation::Default);
  if (!collisions) {
    return Error("Cannot retrieve collisions");
  }
  const unsigned int firstCollisionType = (*collisions->begin())->processType();
  for (LHCb::GenCollisions::const_iterator it = collisions->begin(); it != collisions->end(); ++it) {
    const unsigned int collisionType = (*it)->processType();
    const bool hardQCD = (collisionType >= 110);
    const bool iselastic = (collisionType == 102);
    const bool issd = (collisionType == 103 || collisionType == 104);
    const bool isdd = (collisionType == 105);
    const bool isother = !hardQCD && !iselastic && !issd && !isdd;
    if (isother) info() << "Collision type " << collisionType << endmsg;
    counter("HardScat") += hardQCD; 
    counter("Elastic") += iselastic;
    counter("SD") += issd;
    counter("DD") += isdd;
    counter("Other") += isother;
  } 
  // Loop over the hits and sum up the signals in each quadrant.
  constexpr unsigned int nChannelsPerBoard = 64;
  std::vector<double> signalsB(nChannelsPerBoard, 0.);
  std::vector<double> signalsF(nChannelsPerBoard, 0.);
  const unsigned int nStationsB = 3;
  const unsigned int nStationsF = 2;
  std::vector<double> sumSignals(nStationsB + nStationsF, 0.);
  for (const LHCb::MCHit* hit : *hits) {
    // Determine the station number.
    const double z = hit->midPoint().z();
    unsigned int station = 999;
    if (z > 0.) {
      // Forward stations (Muon side)
      for (unsigned int i = 0; i < nStationsF; ++i) {
        const double dz = z - m_zF[i];
        if (fabs(dz) < 20.) {
          station = i + 1;
          break;
        }
      }
    } else {
      // Backward stations (VELO side)
      for (unsigned int i = 0; i < nStationsB; ++i) {
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
    unsigned int quadrant = 0;
    if (x < 0.) {
      quadrant = y > 0. ? 0 : 3;
    } else {
      quadrant = y > 0. ? 1 : 2;
    }
    // Calculate the channel number.
    const unsigned int channel = m_channels[station][quadrant];
    const double edep = hit->energy() / Gaudi::Units::MeV;
    // Calculate the number of photo-electrons.
    const double signal = edep * m_pePerMeV;
    if (z > 0.) {
      signalsF[channel] += signal;
      sumSignals[2 + station] += signal;
    } else {
      signalsB[channel] += signal;
      sumSignals[station] += signal;
    }
    if (!m_monitoring) continue;
    // Get the particle which produced this hit.
    const LHCb::MCParticle* particle = hit->mcParticle();
    if (!particle) continue;
    const double p = particle->p() / Gaudi::Units::MeV; 
    const std::string st = std::to_string(station);
    // Retrieve the mother particle.
    const LHCb::MCParticle* mother = particle;
    while (mother->originVertex() && 
           mother->originVertex()->type() != LHCb::MCVertex::ppCollision) {
      mother = mother->originVertex()->mother();
    }
    if (!mother) continue;
    const double pt = mother->pt() / Gaudi::Units::MeV;
    const double pseudoRapidity = mother->pseudoRapidity();
    if (z > 0.) {
      plot(edep, "Edep/F" + st, "Deposited energy [MeV]", 0., 10., 50);
      plot(p, "Momentum/F" + st, "Momentum [MeV/c]", 0., 200., 100);
      plot2D(pt, fabs(pseudoRapidity), "EtaPTF" + st, "Pseudorapidity vs. pT",
             0., 5000., 0., 11., 50, 50, edep);
      plot(pseudoRapidity, "EtaF" + st, "Pseudorapidity", -11., 11., 50, edep);
    } else {
      plot(edep, "Edep/B" + st, "Deposited energy [MeV]", 0., 10., 50);
      plot(p, "Momentum/B" + st, "Momentum [MeV/c]", 0., 200., 100);
      plot2D(pt, fabs(pseudoRapidity), "EtaPTB" + st, "Pseudorapidity vs. pT",
             0., 5000., 0., 11., 50, 50, edep);
      plot(pseudoRapidity, "EtaB" + st, "Pseudorapidity", -11., 11., 50, edep);
    }
  }
  if (m_monitoring) {
    for (unsigned int i = 0; i < 5; ++i) {
      const double low = 0.;
      const double high = 2.5e5;
      const unsigned int bins = 50; 
      const std::string st1 = i < 3 ? "B" + std::to_string(i) : 
                                      "F" + std::to_string(i - 2);
      plot(sumSignals[i], "Sum" + st1, low, high, bins);
    }
  }
  // Convert the signals to ADC counts.
  std::vector<unsigned int> adcsB(nChannelsPerBoard, 0);
  std::vector<unsigned int> adcsF(nChannelsPerBoard, 0);
  for (unsigned int i = 0; i < nChannelsPerBoard; ++i) {
    const double nElectronsB = signalsB[i] * m_gain;
    adcsB[i] = static_cast<int>(floor(nElectronsB / m_electronsPerADC));
    if (adcsB[i] > m_maxADC) adcsB[i] = m_maxADC;
    const double nElectronsF = signalsF[i] * m_gain;
    adcsF[i] = static_cast<int>(floor(nElectronsF / m_electronsPerADC));
    if (adcsF[i] > m_maxADC) adcsF[i] = m_maxADC;
    if (msgLevel(MSG::DEBUG)) {
      debug() << format("Channel: %02u (B): ADC = %06i", i, adcsB[i]) << endmsg;
      debug() << format("Channel: %02u (F): ADC = %06i", i, adcsF[i]) << endmsg;
    }
  }
  // Encode the data.
  std::vector<unsigned int> words;
  encode(m_crateB, adcsB, words);
  encode(m_crateF, adcsF, words);
  // Add a new raw bank and pass memory ownership to the raw event.
  const unsigned int version = 2;
  rawEvent->addBank(0, LHCb::RawBank::HC, version, words); 
  return StatusCode::SUCCESS;
}

//=========================================================================
// Raw bank encoding (PRS/SPD format) 
//=========================================================================
void HCDigitCreator::encode(const unsigned int card,
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

//=========================================================================
// Set up the channel map.
//=========================================================================
bool HCDigitCreator::mapChannels(const std::vector<int>& channels,
                                 const unsigned int station,
                                 const bool bwd) {

  const unsigned int offset = bwd ? 0 : 2;
  // Check if the input is valid.
  if (channels.size() != 4) {
    std::string s = bwd ? "B" : "F";
    s += std::to_string(station);
    error() << "Invalid channel map for station " << s << endmsg;
    m_channels[station + offset].assign(4, 0);
    return false;
  }

  const unsigned int crate = bwd ? m_crateB : m_crateF;
  m_channels[station + offset].resize(4);
  for (unsigned int i = 0; i < 4; ++i) {
    if (channels[i] < 0) {
      std::string s = bwd ? "B" : "F";
      s += std::to_string(station);
      error() << "Invalid channel number " << channels[i] 
              << " (station " << s << ")." << endmsg;
      continue;
    }
    m_channels[station + offset][i] = (crate << 6) | channels[i];
  }
  return true;
}
