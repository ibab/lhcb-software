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
                                   ISvcLocator* pSvcLocator) :
    Decoder::AlgBase(name, pSvcLocator) {

  // Initialize search path, and then call the base method.
  m_rawEventLocations = {LHCb::RawEventLocation::Other, 
                         LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
}

//=============================================================================
// Destructor
//=============================================================================
HCRawBankDecoder::~HCRawBankDecoder() {

}

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
    const unsigned int nWords = (*it)->size() / sizeof(unsigned int);
    for (unsigned int i = 0; i < nWords; ++i) {
      const unsigned int word = (*it)->data()[i];
      const unsigned int channel = (word >> 16) & 0xFFFF;
      const unsigned int adc = word & 0xFFFF;
      if (msgLevel(MSG::DEBUG)) {
        debug() << "Channel: " << channel << ", ADC: " << adc << endmsg;
      }
    }
  } 
  return StatusCode::SUCCESS;

}

