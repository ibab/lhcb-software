// Include files:
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VPChannelID.h"
// Event
#include "Event/RawEvent.h"
// VPelDet
#include "VPDet/DeVP.h"
// Local
#include "VPDigitWord.h"
#include "VPDigitRawBankDecoder.h"
#include "VPDigitRawBankToVPDigit.h"
#include "Event/VPDigit.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPDigitRawBankToVPDigit
// 2013-08-12 : Tim Evans - heavily 'borrowed' from VPRawBankToLiteCluster
// 2009-12-27 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VPDigitRawBankToVPDigit)

VPDigitRawBankToVPDigit::VPDigitRawBankToVPDigit(const std::string& name, ISvcLocator* pSvcLocator): 
	GaudiAlgorithm(name, pSvcLocator), m_isDebug(false)
{
  declareProperty( "RawEventLocation", m_rawEventLocation = LHCb::RawEventLocation::Default );
  declareProperty( "DigitLocation"   , m_digitLocation    = LHCb::VPDigitLocation::VPDigitLocation );
}

VPDigitRawBankToVPDigit::~VPDigitRawBankToVPDigit(){}

StatusCode VPDigitRawBankToVPDigit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode VPDigitRawBankToVPDigit::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;

  // Make new clusters container
  VPDigits* digiCont = new VPDigits();
  put(digiCont, m_digitLocation);

  LHCb::RawEvent* rawEvent = NULL;
  rawEvent = getIfExists<LHCb::RawEvent>(m_rawEventLocation);

  if( NULL == rawEvent ) return Warning("Failed to find raw data", StatusCode::SUCCESS,1);

  StatusCode sc = decodeRawBanks(rawEvent,digiCont);
  if(sc.isFailure()) return Error("Problems in decoding, event skipped", StatusCode::SUCCESS );

  return sc;
}

StatusCode VPDigitRawBankToVPDigit::decodeRawBanks(RawEvent* rawEvt, VPDigits* digiCont) const
{
  const std::vector<RawBank*>& tBanks = rawEvt->banks(LHCb::RawBank::VPDigit);
  if(tBanks.size() == 0) return Warning("No VP RawBanks found", StatusCode::SUCCESS);

  int nrClu = 0;
  std::vector<RawBank*>::const_iterator iterBank;
  for(iterBank = tBanks.begin(); iterBank != tBanks.end(); ++iterBank) {
    nrClu++;
    VPDigitRawBankDecoder<VPDigitWord> decoder((*iterBank)->data());
    unsigned int bankVersion = (*iterBank)->version();
    if(m_isDebug) debug() << "Decoding bank version " << bankVersion << endmsg;
    VPDigitRawBankDecoder<VPDigitWord>::pos_iterator iterDigit = decoder.posBegin();
    for(;iterDigit != decoder.posEnd(); ++iterDigit) {
      createVPDigit(*iterDigit,digiCont);
    }
  } 
  return StatusCode::SUCCESS;
}

void VPDigitRawBankToVPDigit::createVPDigit( VPDigitWord aWord, VPDigits* digiCont) const 
{
  LHCb::VPChannelID achan;
  achan.setChannelID(aWord.ChannelID());
  VPDigit* newDigit = new VPDigit((int)aWord.ToT(),0);
  digiCont->insert(newDigit, achan);
  return;
}
