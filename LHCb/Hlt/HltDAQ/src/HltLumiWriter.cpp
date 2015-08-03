// $Id: HltLumiWriter.cpp,v 1.4 2010-04-07 16:59:08 jpalac Exp $
// Include files
// local
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"
#include "HltLumiWriter.h"


//-----------------------------------------------------------------------------
// Implementation file for class : HltLumiWriter
//
// 2008-07-22 : Jaap Panman (using the calo as template)
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HltLumiWriter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  HltLumiWriter::HltLumiWriter( const std::string& name,
                                ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator ),
      m_totDataSize(0),
      m_nbEvents(0),
      m_bankType(LHCb::RawBank::HltLumiSummary)
{
  declareProperty("InputBank", m_inputBank = LHCb::HltLumiSummaryLocation::Default );
  declareProperty("RawEventLocation",m_inputRawEventLocation );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltLumiWriter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_nbEvents    = 0;
  m_totDataSize = 0;
  m_bank.reserve(20);
  m_bankType  = LHCb::RawBank::HltLumiSummary;

  m_rawEventLocations.clear();
  if( !m_inputRawEventLocation.empty() ) m_rawEventLocations.push_back(m_inputRawEventLocation);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Copied);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Trigger);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltLumiWriter::execute() {

  debug() << "==> Execute" << endmsg;

  m_bank.clear( );

  //== Build the data banks
  fillDataBankShort( );

  int totDataSize = 0;

  LHCb::RawEvent* rawEvent = nullptr;
  for (const auto& loc : m_rawEventLocations) {
    //    try RootInTES independent path first
    rawEvent = getIfExists<LHCb::RawEvent>(loc, false);
    if (rawEvent) break;
    //   now try RootInTES dependent path
    rawEvent = getIfExists<LHCb::RawEvent>(loc);
    if (rawEvent) break;
  }
 if( !rawEvent ) return Error(" No RawEvent found at any location.");

  // set source, type, version
  rawEvent->addBank( 0, m_bankType, 0, m_bank );
  totDataSize += m_bank.size();

  m_totDataSize += totDataSize;
  m_nbEvents++;

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Bank size: " << format( "%4d ", m_bank.size() )
            << "Total Data bank size " << totDataSize << endmsg;
  }

  if ( MSG::VERBOSE >= msgLevel() ) {
    verbose() << "DATA bank : " << endmsg;
    int kl = 0;
    for (const auto& w : m_bank ) { 
      verbose() << format ( " %8x %11d   ", w, w);
      if ( ++kl%4 == 0 ) verbose() << endmsg;
    }
    verbose() << endmsg ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltLumiWriter::finalize() {

  if ( m_nbEvents > 0 ) {
    m_totDataSize /= m_nbEvents;
    info() << format( "Average event size : %7.1f words", m_totDataSize )
           << endreq;
  }
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Fill the data bank, structure: Key (upper 16 bits) + value
//=========================================================================
void HltLumiWriter::fillDataBankShort ( ) {

  LHCb::HltLumiSummary* HltLumiSummary = getIfExists<LHCb::HltLumiSummary>(m_inputBank);
  if (!HltLumiSummary) {
    debug() << m_inputBank << " not found" << endmsg ;
    return;
  }
  debug() << m_inputBank << " found" << endmsg ;

  // handle overflow
  auto i_value = [](int s) { return (s < 0xFFFF) ? (int) s : (int)0xFFFF; };
  for (const auto& summaryIter : HltLumiSummary->extraInfo()) {
    m_bank.push_back( ( summaryIter.first << 16 ) | ( i_value(summaryIter.second) & 0xFFFF ) );
    if ( MSG::VERBOSE >= msgLevel() ) {
      auto word = m_bank.back();
      verbose() << format ( " %8x %11d %11d %11d ", word, word, word>>16, word&0xFFFF ) << endreq;
    }
  }
}
//=============================================================================
