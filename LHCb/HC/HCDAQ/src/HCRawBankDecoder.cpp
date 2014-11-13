// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb
// Event/DAQEvent
#include "Event/RawEvent.h"

// Local
#include "HCRawBankDecoder.h"

DECLARE_ALGORITHM_FACTORY(HCRawBankDecoder)

//=============================================================================
// Constructor
//=============================================================================
HCRawBankDecoder::HCRawBankDecoder(const std::string& name,
                                   ISvcLocator* pSvcLocator)
    : Decoder::AlgBase(name, pSvcLocator) {

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

  // Create a container and pass its ownership to the TES.
  // LHCb::HCDigits* digits = new LHCb::HCDigits();
  // put(digits, m_digitLocation);

  // Retrieve the raw event.
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  if (NULL == rawEvent) {
    return Warning("Cannot find raw event", StatusCode::SUCCESS, 1);
  }

  // Retrieve the HC raw banks.
  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::HC);
  if (banks.empty()) {
    return Warning("Cannot find HC raw banks", StatusCode::SUCCESS);
  }

  std::vector<LHCb::RawBank*>::const_iterator it;
  for (it = banks.begin(); it != banks.end(); ++it) {
    // Make sure the bank is not corrupted.
    if (LHCb::RawBank::MagicPattern != (*it)->magic()) {
      error() << "Bad magic pattern" << endmsg;
      continue;
    }
    const unsigned int source = (*it)->sourceID();
    const unsigned int version = (*it)->version();
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Source: " << source << ", version: " << version << endmsg;
    }
    // Decode the raw bank.
    if (version == 1) {
      decodeV1(*it);
    } else if (version == 2) {
      decodeV2(*it);
    } else {
      error() << "Unknown raw bank version (" << version << ")" << endmsg;
      return StatusCode::FAILURE;
    }
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
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Channel: " << channel << ", ADC: " << adc << endmsg;
    }
  }
  return true;
}

//=============================================================================
// Decoding of raw bank version 2 (compressed format)
//=============================================================================
bool HCRawBankDecoder::decodeV2(LHCb::RawBank* bank) {

  unsigned int* data = bank->data();
  unsigned int nWords = bank->size() / sizeof(unsigned int);
  while (nWords != 0) {
    unsigned int word = *data++;
    --nWords;
    // Read bank header.
    const int lenTrig = word & 0x7F;
    const int code = (word >> 14) & 0x1FF;
    const int ctrl = (word >> 23) & 0x1FF;
    if (0 != (0x1 & ctrl) || 0 != (0x20 & ctrl) || 0 != (0x40 & ctrl)) {
      if (msgLevel(MSG::DEBUG)) {
        debug() << "Tell1 error bits have been detected in data" << endmsg;
        if (0 != (0x1  & ctrl)) debug() << "Tell1Error" << endmsg; 
        if (0 != (0x20 & ctrl)) debug() << "Tell1Sync" << endmsg; 
        if (0 != (0x40 & ctrl)) debug() << "Tell1Link" << endmsg; 
      }
    }
    // Skip the trigger bits.
    const int nSkip = (lenTrig + 3) / 4;
    nWords -= nSkip;
    data += nSkip;
    unsigned int pattern = *data++;
    int offset = 0;
    unsigned int lastData = *data++;
    nWords -= 2;
    for (unsigned int bitNum = 0; bitNum < 32; ++bitNum) {
      int adc = 0;
      if (31 < offset) {
        offset -= 32;
        lastData = *data++;
        nWords--;
      }
      if (0 == (pattern & (1 << bitNum))) {
        // Short coding
        adc = ((lastData >> offset) & 0xF) - 8;
        offset += 4;
      } else {
        adc = ((lastData >> offset) & 0xFFF);
        if (24 == offset) adc &= 0xFF;
        // Clean-up extra bits
        if (28 == offset) adc &= 0xF;
        offset += 12;
        if (32 < offset) {
          // Get the extra bits on next word
          lastData = *data++;
          nWords--;
          offset -= 32;
          int temp = (lastData << (12 - offset)) & 0xFFF;
          adc += temp;
        }
        adc -= 256;
      }
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << format("Code: %06i ", code)
                  << format("Channel: %06i ", bitNum) 
                  << format("ADC: %06i", adc) << endmsg;
      }
    }
  }
  return true;
}
