
#include "Kernel/VPChannelID.h"

#include "VPSuperPixelBankEncoder.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPSuperPixelBankEncoder
//
// 2014-03-06 : Karol Hennessy, Kurt Rinnert
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPSuperPixelBankEncoder)

//=============================================================================
// Constructor
//=============================================================================
VPSuperPixelBankEncoder::VPSuperPixelBankEncoder(const std::string& name,
                                                 ISvcLocator* pSvcLocator)
    : GaudiAlgorithm(name, pSvcLocator),
      m_bankVersion(2),
      m_isDebug(false),
      m_isVerbose(false),
      m_evt(0),
      m_spBySensor(208, std::vector<unsigned int>()) {
  declareProperty("DigitLocation",
                  m_digitLocation = LHCb::VPDigitLocation::Default);
  declareProperty("RawEventLocation",
                  m_rawEventLocation = LHCb::RawEventLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
VPSuperPixelBankEncoder::~VPSuperPixelBankEncoder() { ; }

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPSuperPixelBankEncoder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if (m_isDebug) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPSuperPixelBankEncoder::execute() {

  if (m_isDebug) debug() << "==> Execute" << endmsg;
  ++m_evt;
  const VPDigits* digits = getIfExists<VPDigits>(m_digitLocation);
  if (NULL == digits) {
    return Error(" ==> There are no VPDigits in TES! ");
  }

  // Check if RawEvent exists
  RawEvent* rawEvent = getIfExists<RawEvent>(m_rawEventLocation);
  if (NULL == rawEvent) {
    // Create RawEvent
    rawEvent = new LHCb::RawEvent();
    put(rawEvent, m_rawEventLocation);
  }

  for (unsigned int sensor = 0; sensor < m_spBySensor.size(); ++sensor) {
    m_spBySensor[sensor].clear();
    // put header word as first element (but don't know content yet)
    m_spBySensor[sensor].push_back(0);
  }

  // Loop over digits create super pixel words and store them.
  // No assumption about the order of digits is made.
  LHCb::VPDigits::const_iterator itSeed;
  for (itSeed = digits->begin(); itSeed != digits->end(); ++itSeed) {
    const unsigned int chip = (*itSeed)->channelID().chip();
    const unsigned int row = (*itSeed)->channelID().row();
    const unsigned int col = (*itSeed)->channelID().col();
    const unsigned int sensor = (*itSeed)->channelID().sensor();
    const unsigned int sensorCol = col + 256 * chip;
    const unsigned int spCol = sensorCol / 2;
    const unsigned int spRow = row / 4;
    const unsigned int spAddr = ((spCol << 6) | spRow);
    const unsigned int spix = 1 << (((col % 2) * 4) + row % 4);

    bool found = false;

    for (unsigned int j = 0; j < m_spBySensor[sensor].size(); j++) {
      if ((m_spBySensor[sensor][j] >> 8) == spAddr) {
        m_spBySensor[sensor][j] |= spix;
        found = true;
        break;
      }
    }

    if (!found) {
      m_spBySensor[sensor].push_back((spAddr << 8) | spix);
    }

  }

  // set 'no neighbour' hint flags
  int dx[] = { -1, 0, 1, -1, 0, 1, -1, 1 };
  int dy[] = { -1, -1, -1, 1, 1, 1, 0, 0 };

  for (unsigned int sens = 0; sens < m_spBySensor.size(); ++sens) {

    if (m_spBySensor[sens].size() < 2) {
      continue;
    }  // empty bank

    // sort super pixels column major on each sensor
    std::sort(m_spBySensor[sens].begin() + 1, m_spBySensor[sens].end(),
              SPLowerThan());

    memset(m_buffer, 0, 24576 * sizeof(unsigned char));
    m_idx.clear();
    unsigned int nsp = m_spBySensor[sens].size();
    for (unsigned int j = 1; j < nsp; ++j) {
      unsigned int spw = m_spBySensor[sens][j];
      unsigned int idx = (spw >> 8);
      m_buffer[idx] = spw & 0xFFU;
      m_idx.push_back(j);
    }
    for (unsigned int ik = 0; ik < m_idx.size(); ++ik) {
      const unsigned int spw = m_spBySensor[sens][m_idx[ik]];
      const unsigned int idx = (spw >> 8);
      const unsigned int row = idx & 0x3FU;
      const unsigned int col = (idx >> 6);
      unsigned int no_neighbour = 1;
      for (unsigned int ni = 0; ni < 8; ++ni) {
        const int nrow = row + dy[ni];
        if (nrow < 0 || nrow > 63) continue;
        const int ncol = col + dx[ni];
        if (ncol < 0 || ncol > 383) continue;
        const unsigned int nidx = (ncol << 6) | nrow;
        if (m_buffer[nidx]) {
          no_neighbour = 0;
          break;
        }
      }
      m_spBySensor[sens][m_idx[ik]] = spw | (no_neighbour << 31);
    }
  }

  int total = 0;
  for (unsigned int sensor = 0; sensor < m_spBySensor.size(); sensor++) {

    // encode header.
    m_spBySensor[sensor][0] =
        m_spBySensor[sensor].size() - 1;  // set correct number of super pixels

    unsigned int banksize = sizeof(unsigned int) * m_spBySensor[sensor].size();
    total += banksize;
    if (m_isDebug)
      debug() << "evt " << m_evt << "sensor " << sensor << " sp "
              << m_spBySensor[sensor].size() << endmsg;

    LHCb::RawBank* spBank =
        rawEvent->createBank(sensor, LHCb::RawBank::VP, m_bankVersion, banksize,
                             &(m_spBySensor[sensor][0]));
    // Add new bank and pass memory ownership to raw event
    rawEvent->adoptBank(spBank, true);
  }

  if (m_isDebug) debug() << "total " << total << endmsg;

  return StatusCode::SUCCESS;
}
