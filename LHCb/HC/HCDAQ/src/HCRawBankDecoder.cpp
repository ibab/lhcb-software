// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb
// Event/DAQEvent
#include "Event/RawEvent.h"
// Event/DigiEvent
#include "Event/HCDigit.h"
// Kernel/LHCbKernel
#include "Kernel/HCCellID.h"

// Local
#include "HCRawBankDecoder.h"

DECLARE_ALGORITHM_FACTORY(HCRawBankDecoder)

//=============================================================================
// Constructor
//=============================================================================
HCRawBankDecoder::HCRawBankDecoder(const std::string& name,
                                   ISvcLocator* pSvcLocator)
    : Decoder::AlgBase(name, pSvcLocator),
      m_digits(NULL), m_l0digits(NULL) {

  // Initialize search path, and then call the base method.
  m_rawEventLocations = {LHCb::RawEventLocation::Other,
                         LHCb::RawEventLocation::Default};
  initRawEventSearch();
}

//=============================================================================
// Destructor
//=============================================================================
HCRawBankDecoder::~HCRawBankDecoder() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCRawBankDecoder::initialize() {

  StatusCode sc = Decoder::AlgBase::initialize();
  if (sc.isFailure()) return sc;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode HCRawBankDecoder::execute() {

  // Create containers and pass their ownership to the TES.
  m_digits = new LHCb::HCDigits();
  put(m_digits, LHCb::HCDigitLocation::Default);
  m_l0digits = new LHCb::HCDigits();
  put(m_l0digits, LHCb::HCDigitLocation::L0);
    
  // Retrieve the raw event.
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  if (NULL == rawEvent) {
    return Warning("Cannot find raw event", StatusCode::SUCCESS, 1);
  }

  // Retrieve the HC raw banks.
  auto banks = rawEvent->banks(LHCb::RawBank::HC);
  if (banks.empty()) {
    return Warning("Cannot find HC raw banks", StatusCode::SUCCESS);
  }

  for (auto it = banks.begin(), end = banks.end(); it != end; ++it) {
    // Make sure the bank is not corrupted.
    if (LHCb::RawBank::MagicPattern != (*it)->magic()) {
      error() << "Bad magic pattern" << endmsg;
      continue;
    }
    const unsigned int source = (*it)->sourceID();
    const unsigned int version = (*it)->version();
    if (UNLIKELY(msgLevel(MSG::DEBUG))) {
      debug() << "Source: " << source << ", version: " << version << endmsg;
    }
    // Decode the raw bank.
    if (version == 1) {
      decodeV1(*it);
    } else if (version == 2) {
      decodeV2(*it);
    } else if (version == 3) {
      decodeV3(*it);
    } else {
      error() << "Unknown raw bank version (" << version << ")" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  auto errorBanks = rawEvent->banks(LHCb::RawBank::EcalPackedError);
  if (errorBanks.empty()) {
    if (UNLIKELY(msgLevel(MSG::DEBUG))) {
      debug() << "No error banks in raw event." << endmsg;
    }
    return StatusCode::SUCCESS;
  }
  for (auto it = errorBanks.begin(), end = errorBanks.end(); it != end; ++it) {
    // Make sure the bank is not corrupted.
    if (LHCb::RawBank::MagicPattern != (*it)->magic()) {
      error() << "Bad magic pattern in error bank" << endmsg;
      continue;
    }
    decodeErrorBank(*it);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Decoding of raw bank version 1 (simple format)
//=============================================================================
bool HCRawBankDecoder::decodeV1(LHCb::RawBank* bank) {

  const unsigned int nWords = bank->size() / sizeof(unsigned int);
  for (unsigned int i = 0; i < nWords; ++i) {
    const unsigned int word = bank->data()[i];
    const unsigned int channel = (word >> 16) & 0xFFFF;
    const unsigned int adc = word & 0xFFFF;
    if (UNLIKELY(msgLevel(MSG::VERBOSE))) {
      verbose() << "Channel: " << channel << ", ADC: " << adc << endmsg;
    }
  }
  return true;
}

//=============================================================================
// Decoding of raw bank version 2 (compressed format for ECAL and HCAL)
//=============================================================================
bool HCRawBankDecoder::decodeV2(LHCb::RawBank* bank) {

  uint32_t* data = bank->data();
  unsigned int nWords = bank->size() / sizeof(uint32_t);
  while (nWords > 0) {
    // Read the bank header.
    uint32_t word = *data++;
    --nWords;
    // Length of the trigger part in bytes, without padding.
    const unsigned int lenTrig = word & 0x7F;
    // Length of the ADC part in bytes, without padding.
    const unsigned int lenAdc = (word >> 7) & 0x7F;
    const unsigned int card = (word >> 14) & 0xF;
    const unsigned int crate = (word >> 18) & 0x1F; 
    // Control word.
    const unsigned int ctrl = (word >> 23) & 0x1FF;
    if (UNLIKELY(msgLevel(MSG::VERBOSE))) {
      verbose() << format("Crate: %02u, card: %02u, ", crate, card) 
                << format("trigger part: %03u bytes, ", lenTrig)
                << format("ADC part: %03u bytes", lenAdc) << endmsg;
      const unsigned int ttype = (ctrl >> 1) & 0xF;
      const unsigned int chk = (ctrl >> 7) & 0x2;
      verbose() << format("Trigger type: 0x%1x, ", ttype) 
                << "control bits: " << chk << endmsg; 
    }
    const unsigned int generalError = ctrl & 0x1;
    const unsigned int syncError = ctrl & 0x20;
    const unsigned int linkError = ctrl & 0x40;
    if (generalError != 0 || syncError != 0 || linkError != 0) {
      if (UNLIKELY(msgLevel(MSG::DEBUG))) {
        debug() << "Tell1 error bits have been detected in data" << endmsg;
        if (0 != generalError) debug() << "General error" << endmsg; 
        if (0 != syncError) debug() << "Synchronisation error" << endmsg; 
        if (0 != linkError) debug() << "Link error" << endmsg; 
      }
    }
    uint32_t pattern = 0;
    int offset = 0;
    // Read the trigger data.
    if (m_skipTrigger) {
      const unsigned int nSkip = (lenTrig + 3) / 4;
      data += nSkip;
      nWords -= nSkip;
    } else if (lenTrig > 0) {
      pattern = *data++;
      word = *data++;
      nWords -= 2;
      for (unsigned int i = 0; i < 32; ++i) {
        const unsigned int station = i % 8;
        const unsigned int quadrant = (i - station) / 8; 
        LHCb::HCCellID cell(0, station, quadrant);
        const uint32_t mask = 1 << i;
        if ((pattern & mask) == 0) {
          m_l0digits->insert(new LHCb::HCDigit(-5), cell);
          continue;
        }
        if (offset > 31) {
          // Next word.
          offset = 0;
          word = *data++;
          --nWords;
        }
        const int adc = (word >> offset) & 0xFF;
        offset += 8;
        if (UNLIKELY(msgLevel(MSG::VERBOSE))) {
          verbose() << format("Channel: %06u ", i) 
                    << format("Trigger ADC: %06i", adc) << endmsg;
        }
        m_l0digits->insert(new LHCb::HCDigit(adc), cell);
      }
    } else {
      // Empty trigger block. 
      for (unsigned int i = 0; i < 32; ++i) {
        const unsigned int station = i % 8;
        const unsigned int quadrant = (i - station) / 8; 
        LHCb::HCCellID cell(0, station, quadrant);
        m_l0digits->insert(new LHCb::HCDigit(-5), cell);
      }
    }
    if (m_skipAdc) {
      const unsigned int nSkip = (lenAdc + 3) / 4;
      data += nSkip;
      nWords -= nSkip;
      continue;
    }
    // Read the ADC part.
    pattern = *data++;
    offset = 0;
    word = *data++;
    nWords -= 2;
    for (unsigned int i = 0; i < 32; ++i) {
      int adc = 256;
      if (offset > 31) {
        // Next word.
        offset = 0;
        word = *data++;
        --nWords;
      }
      const uint32_t mask = 1 << i;
      if (0 == (pattern & mask)) {
        // Short (4 bit) coding
        adc = ((word >> offset) & 0xF) - 8;
        offset += 4;
      } else {
        // Long (12 bit) coding
        adc = (word >> offset) & 0xFFF;
        if (24 == offset) adc &= 0xFF;
        if (28 == offset) adc &= 0xF;
        offset += 12;
        if (offset > 32) {
          // Get the extra bits on next word
          word = *data++;
          --nWords;
          offset -= 32;
          const int temp = (word << (12 - offset)) & 0xFFF;
          adc += temp;
        }
        adc -= 256;
      }
      const unsigned int station = i % 8;
      const unsigned int quadrant = (i - station) / 8; 
      LHCb::HCCellID cell(0, station, quadrant);
      m_digits->insert(new LHCb::HCDigit(adc), cell);
      if (UNLIKELY(msgLevel(MSG::VERBOSE))) {
        verbose() << format("Channel: %06u ", i) 
                  << format("ADC: %06i", adc) << endmsg;
      }
    }
  }
  return true;
}

//=============================================================================
// Decoding of raw bank version 3 (compressed format for PS and SPD)
//=============================================================================
bool HCRawBankDecoder::decodeV3(LHCb::RawBank* bank) {

  uint32_t* data = bank->data();
  unsigned int nWords = bank->size() / sizeof(uint32_t);
  while (nWords > 0) {
    // Read the bank header.
    uint32_t word = *data++;
    --nWords;
    // Length of the trigger part.
    unsigned int lenTrig = word & 0x7F;
    // Length of the ADC part.
    unsigned int lenAdc = (word >> 7) & 0x7F;
    const unsigned int card = (word >> 14) & 0xF;
    const unsigned int crate = (word >> 18) & 0x1F; 
    // Control word.
    const unsigned int ctrl = (word >> 23) & 0x1FF;
    if (UNLIKELY(msgLevel(MSG::VERBOSE))) {
      verbose() << format("Crate: %02u, card: %02u, ", crate, card) 
                << format("trigger part: %03u bytes, ", lenTrig)
                << format("ADC part: %03u bytes", 2 * lenAdc) << endmsg;
      const unsigned int ttype = (ctrl >> 1) & 0xF;
      const unsigned int chk = (ctrl >> 7) & 0x2;
      verbose() << format("Trigger type: 0x%1x, ", ttype) 
                << "control bits: " << chk << endmsg; 
    }
    const unsigned int generalError = ctrl & 0x1;
    const unsigned int syncError = ctrl & 0x20;
    const unsigned int linkError = ctrl & 0x40;
    if (generalError != 0 || syncError != 0 || linkError != 0) {
      if (UNLIKELY(msgLevel(MSG::DEBUG))) {
        debug() << "Tell1 error bits have been detected in data" << endmsg;
        if (0 != generalError) debug() << "General error" << endmsg; 
        if (0 != syncError) debug() << "Synchronisation error" << endmsg; 
        if (0 != linkError) debug() << "Link error" << endmsg; 
      }
    }
    unsigned int offset = 32;
    // Read the trigger data.
    if (m_skipTrigger) {
      const unsigned int nSkip = (lenTrig + 3) / 4;
      data += nSkip;
      nWords -= nSkip;
    } else {
      while (lenTrig > 0) {
        if (32 == offset) {
          // Read the next word.
          word = *data++;
          --nWords;
          offset = 0;
        }
        const unsigned int channel = (word >> offset) & 0x3F;
        const int prs = (word >> (offset + 6)) & 1;
        const int spd = (word >> (offset + 7)) & 1;
        const int adc = prs + 10 * spd;
        if (UNLIKELY(msgLevel(MSG::VERBOSE))) {
          verbose() << format("Channel: %06u ", channel) 
                    << format("PRS: %01i ", prs) 
                    << format("SPD: %01i ", spd) << endmsg;
        }
        const unsigned int station = channel % 8;
        const unsigned int quadrant = (channel - station) / 8; 
        LHCb::HCCellID cell(0, station, quadrant);
        // Make sure there isn't already an object ID.
        if (m_l0digits->object(cell)) {
          warning() << "Duplicated cell ID " << cell << endmsg;
        } else {
          m_l0digits->insert(new LHCb::HCDigit(adc), cell);
        }
        offset += 8;
        --lenTrig;
      }
    }
    if (m_skipAdc) {
      const unsigned int nSkip = (lenAdc + 1) / 2;
      data += nSkip;
      nWords -= nSkip;
      continue;
    }
    // Read the ADC data
    offset = 32;
    while (lenAdc > 0) {
      if (32 == offset) {
        // Read the next word.
        word = *data++;
        --nWords;
        offset = 0;
      }
      const int adc = (word >> offset) & 0x3FF;
      const unsigned int channel = (word >> (offset + 10)) & 0x3F;
      --lenAdc;
      offset += 16;
      const unsigned int station = channel % 8;
      const unsigned int quadrant = (channel - station) / 8; 
      LHCb::HCCellID cell(0, station, quadrant);
      if (m_digits->object(cell)) {
        warning() << "Duplicated cell ID " << cell << endmsg;
      } else {
        m_digits->insert(new LHCb::HCDigit(adc), cell);
      }
      if (UNLIKELY(msgLevel(MSG::VERBOSE))) {
        verbose() << format("Channel: %06u ", channel) 
                  << format("ADC: %06i", adc) << endmsg;
      }
    }
  }
  return true;
}

//=============================================================================
// Decoding of error raw banks
//=============================================================================
bool HCRawBankDecoder::decodeErrorBank(LHCb::RawBank* bank) {

  uint32_t* data = bank->data();
  const unsigned int nPP = 4;
  for (unsigned int i = 0; i < nPP; ++i) {
    const uint32_t evtrl = *data;
    const uint32_t bcnt = evtrl & 0xFFF;
    const uint32_t detid = (evtrl >> 12) & 0xF;
    const uint32_t bankList = (evtrl >> 16) & 0xFF;
    const uint32_t eventInfo = (evtrl >> 24) & 0xFF;
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << std::string(60, '=') << endmsg;
      verbose() << "PP FPGA " << i << endmsg;
      verbose() << std::string(60, '=') << endmsg;
      verbose() << "Bunch counter: " << bcnt << endmsg;
      verbose() << format("Detector ID: 0x%x", detid) << endmsg;
      verbose() << format("Bank list: 0x%x", bankList) << endmsg;
      if (eventInfo != 0) {
        verbose() << format("Event info: 0x%x", eventInfo) << endmsg;
      }
    }
    ++data;
    const uint32_t l0cnt = *data;
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "L0 event counter: " << l0cnt << endmsg;
    }
    data += 3;
    const unsigned int errorBankLength = (*data >> 16) & 0xFFFF;
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Error bank length: " << errorBankLength << endmsg;
    }
    if (errorBankLength == 0) {
      data += 2;
      continue;
    }
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Link  Sync Err  Link Err  Crate  Card   L0   BX" << endmsg;
    }
    for (unsigned int j = 0; j < 4; ++j) {
      ++data;
      const uint32_t word = *data;
      const unsigned int febx = word & 0x3FF;
      const unsigned int fel0 = (word >> 10) & 0x3FF;
      const unsigned int card = (word >> 20) & 0xF;
      const unsigned int crate = (word >> 24) & 0x1F;
      const unsigned int errLink = (word >> 29) & 0x1;
      const unsigned int errSync = (word >> 30) & 0x1;
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << j 
                  << "         " << errSync 
                  << "         " << errLink << "      " 
                  << format("%2u    %2u   %4u  %4u", crate, card, fel0, febx) 
                  << endmsg; 
      }
    }
    data += 2;
  }
  return true;

}
