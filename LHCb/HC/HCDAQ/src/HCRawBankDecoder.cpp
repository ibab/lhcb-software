// ROOT
#include "TH2D.h"
#include "TAxis.h"

// Gaudi
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiUtils/HistoLabels.h"

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
#include "Event/RawEvent.h"
// Event/DigiEvent
#include "Event/HCDigit.h"
// Kernel/LHCbKernel
#include "Kernel/HCCellID.h"

// Local
#include "HCRawBankDecoder.h"

using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCRawBankDecoder)

//=============================================================================
// Constructor
//=============================================================================
HCRawBankDecoder::HCRawBankDecoder(const std::string& name,
                                   ISvcLocator* pSvcLocator)
    : Decoder::HistoAlgBase(name, pSvcLocator),
      m_digits(NULL), m_l0digits(NULL), m_odin(NULL) {

  declareProperty("DigitLocation", 
                  m_digitLocation = LHCb::HCDigitLocation::Default);
  declareProperty("L0DigitLocation",
                  m_l0digitLocation = LHCb::HCDigitLocation::L0);
  declareProperty("SkipErrorBanks", m_skipErrorBanks = false);
  declareProperty("SkipTriggerWords", m_skipTrigger = false);
  declareProperty("SkipAdcWords", m_skipAdc = false);
  declareProperty("Monitoring", m_monitoring = false);
 
  // Initialize search path, and then call the base method.
  m_rawEventLocations.push_back(LHCb::RawEventLocation::HC);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Other);
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

  StatusCode sc = Decoder::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

  // Get ODIN.
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder", "OdinDecoder", this);

  // Book histograms.
  for (unsigned int i = 0; i < 2; ++i) {
    const std::string name = "ErrorBanks/DiffBX/Crate" + std::to_string(i);
    const std::string title = "Crate " + std::to_string(i);
    m_hBxDiff.push_back(book1D(name, title, -20.5, 20.5, 41));
    setAxisLabels(m_hBxDiff[i], "BX_{FE} - BX_{ODIN}", "Entries");
  }
  m_hLinkErrors = book2D("ErrorBanks/Links", "Errors", 
                         -0.5, 1.5, 2, -0.5, 15.5, 16);
  m_hTell1Errors = book2D("RawBanks/ErrorBitsTELL1", "Errors", 
                         -0.5, 2.5, 3, -0.5, 1.5, 2);
  TH2D* h = Gaudi::Utils::Aida2ROOT::aida2root(m_hLinkErrors);
  if (h) {
    TAxis* axis = h->GetYaxis();
    if (axis) {
      for (unsigned int i = 0; i < 4; ++i) {
        for (unsigned int j = 0; j < 4; ++j) {
          const unsigned int bin = 16 - (4 * i + j);
          const std::string label = "PP" + std::to_string(i) + 
                                    " L" + std::to_string(j);
          axis->SetBinLabel(bin, label.c_str());
        }
      }
    }
    axis = h->GetXaxis();
    if (axis) {
      axis->SetBinLabel(1, "Link Error");
      axis->SetBinLabel(2, "Synchronisation Error");
    }
  } 
  h = Gaudi::Utils::Aida2ROOT::aida2root(m_hTell1Errors);
  if (h) {
    TAxis* axis = h->GetYaxis();
    if (axis) {
      axis->SetBinLabel(1, "Crate 0");
      axis->SetBinLabel(2, "Crate 1");
    }
    axis = h->GetXaxis();
    if (axis) {
      axis->SetBinLabel(1, "General");
      axis->SetBinLabel(2, "Synchronisation");
      axis->SetBinLabel(3, "Link");
    }
  } 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode HCRawBankDecoder::execute() {

  // Create containers and pass their ownership to the TES.
  m_digits = new LHCb::HCDigits();
  put(m_digits, m_digitLocation);
  m_l0digits = new LHCb::HCDigits();
  put(m_l0digits, m_l0digitLocation);
    
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
    if (version == 2 || version == 3) {
      decode(*it);
    } else {
      error() << "Unknown raw bank version (" << version << ")" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if (m_skipErrorBanks) return StatusCode::SUCCESS;

  // Get event information from ODIN.
  m_odin->getTime();
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN", StatusCode::SUCCESS);
  }
  const unsigned int bxid = odin->bunchId();

  auto errorBanks = rawEvent->banks(LHCb::RawBank::HCError);
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
    decodeErrorBank(*it, bxid);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Decoding of raw banks (compressed format, PS/SPD)
//=============================================================================
bool HCRawBankDecoder::decode(LHCb::RawBank* bank) {

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
      if (m_monitoring) {
        if (0 != generalError) m_hTell1Errors->fill(0, crate);
        if (0 != syncError) m_hTell1Errors->fill(1, crate);
        if (0 != linkError) m_hTell1Errors->fill(2, crate);
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
        LHCb::HCCellID cell(channel | (crate << 6));
        // Make sure there isn't already an object ID.
        if (m_l0digits->object(cell)) {
          warning() << "Duplicated cell ID " << cell << endmsg;
        } else {
          LHCb::HCDigit* l0digit = new LHCb::HCDigit(adc);
          m_l0digits->insert(l0digit, cell);
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
      LHCb::HCCellID cell(channel | crate << 6);
      if (m_digits->object(cell)) {
        warning() << "Duplicated cell ID " << cell << endmsg;
      } else {
        LHCb::HCDigit* digit = new LHCb::HCDigit(adc);
        m_digits->insert(digit, cell);
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
bool HCRawBankDecoder::decodeErrorBank(LHCb::RawBank* bank, 
                                       const int bxid) {

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
      if (m_monitoring) {
        const unsigned int bin = 15 - (4 * i + j); 
        if (errLink > 0) m_hLinkErrors->fill(0, bin);
        if (errSync > 0) m_hLinkErrors->fill(1, bin);
        m_hBxDiff[crate]->fill(int(febx) - bxid); 
      }
    }
    data += 2;
  }
  return true;

}
