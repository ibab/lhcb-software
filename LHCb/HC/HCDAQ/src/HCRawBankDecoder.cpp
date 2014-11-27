// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb
// Event/DAQEvent
#include "Event/RawEvent.h"
#include "Event/HCDigit.h"
#include "Kernel/HCCellID.h"

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
  LHCb::HCDigits* digits = new LHCb::HCDigits();
   put(digits, "Raw/HC/Digits");
  LHCb::HCDigits* l0digits = new LHCb::HCDigits();
   put(l0digits, "Raw/HC/L0Digits");
    
  
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
      decodeV2(*it, *digits, *l0digits);
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
bool HCRawBankDecoder::decodeV2(LHCb::RawBank* bank, LHCb::HCDigits& digits, LHCb::HCDigits& l0digits) {

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
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << format("Crate: %02u, card: %02u, ", crate, card) 
                << format("trigger part: %03u bytes, ", lenTrig)
                << format("ADC part: %03u bytes", lenAdc) << endmsg;
      const unsigned int ttype = (ctrl >> 1) & 0xF;
      const unsigned int chk = (ctrl >> 7) & 0x2;
      verbose() << format("Triggger type: 0x%1x, ", ttype) 
                << "control bits: " << chk << endmsg; 
    }
    const unsigned int generalError = ctrl & 0x1;
    const unsigned int syncError = ctrl & 0x20;
    const unsigned int linkError = ctrl & 0x40;
    if (generalError != 0 || syncError != 0 || linkError != 0) {
      if (msgLevel(MSG::DEBUG)) {
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
        const int read = pattern & (1 << i);
        if (read == 0){

	  LHCb::HCCellID cell( 0 , i % 8 , ( i - ( i % 8 ))/8);
	  l0digits.insert(new LHCb::HCDigit(-5),cell);
	  continue;}
        if (offset > 31) {
          // Next word.
          offset = 0;
          word = *data++;
          --nWords;
        }
        const int adc = (word >> offset) & 0xFF;
        offset += 8;
        if (msgLevel(MSG::VERBOSE)) {
          verbose() << format("Channel: %06u ", i) 
                    << format("Trigger ADC: %06i", adc) << endmsg;
        }

	LHCb::HCCellID cell( 0 , i % 8 , ( i - ( i % 8 ))/8);
	l0digits.insert(new LHCb::HCDigit(adc),cell);
      }
    }
    else{
      
      for (unsigned int i = 0; i < 32; ++i) {
	LHCb::HCCellID cell( 0 , i % 8 , ( i - ( i % 8 ))/8);
	l0digits.insert(new LHCb::HCDigit(-5),cell);
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
      if (0 == (pattern & (1 << i))) {
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
      LHCb::HCCellID cell( 0 , i % 8 , ( i - (i % 8 ))/8);
      digits.insert(new LHCb::HCDigit(adc),cell);
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << format("Channel: %06u ", i) 
                  << format("ADC: %06i", adc) << endmsg;
      }
    }
  }
  return true;
}
