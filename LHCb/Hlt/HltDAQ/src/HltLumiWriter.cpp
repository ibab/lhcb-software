// $Id: HltLumiWriter.cpp,v 1.4 2010-04-07 16:59:08 jpalac Exp $
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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
      m_bankType(LHCb::RawBank::HltLumiSummary),
      m_inputRawEventLocation("")
{
  declareProperty( "InputBank", m_inputBank = LHCb::HltLumiSummaryLocation::Default );
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
  if( m_inputRawEventLocation != "" )m_rawEventLocations.push_back(m_inputRawEventLocation);
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

  LHCb::RawEvent* rawEvent = 0;
  auto iLoc = m_rawEventLocations.begin();
  for ( ; iLoc != m_rawEventLocations.end() ; ++iLoc ) {
    //    try RootInTES independent path first
    if (exist<LHCb::RawEvent>(*iLoc, false)) {
      rawEvent = get<LHCb::RawEvent>(*iLoc, false);
      break;
    }
    //   now try RootInTES dependent path
    if (exist<LHCb::RawEvent>(*iLoc)) {
      rawEvent = get<LHCb::RawEvent>(*iLoc);
      break;
    }
  }
 if( ! rawEvent ){
    return Error(" No RawEvent found at any location.");
  }  


  // set source, type, version
  rawEvent->addBank( 0, m_bankType, 0, m_bank );
  totDataSize += m_bank.size();

  m_totDataSize += totDataSize;
  m_nbEvents++;

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Bank size: ";
    debug() << format( "%4d ", m_bank.size() )
            << "Total Data bank size " << totDataSize << endreq;
  }

  if ( MSG::VERBOSE >= msgLevel() ) {
    verbose() << "DATA bank : " << endreq;
    int kl = 0;

    for (auto  itW = m_bank.begin(); m_bank.end() != itW; itW++ ){
      verbose() << format ( " %8x %11d   ", (*itW), (*itW) );
      kl++;
      if ( 0 == kl%4 ) verbose() << endreq;
    }
    verbose() << endreq ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltLumiWriter::finalize() {

  if ( 0 < m_nbEvents ) {
    m_totDataSize /= m_nbEvents;
    info() << "Average event size : " << format( "%7.1f words", m_totDataSize )
           << endreq;
  }
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Fill the data bank, structure: Key (upper 16 bits) + value
//=========================================================================
void HltLumiWriter::fillDataBankShort ( ) {

  if (!exist<LHCb::HltLumiSummary>(m_inputBank) ){
    debug() << m_inputBank << " not found" << endmsg ;
    return;
  }

  LHCb::HltLumiSummary* HltLumiSummary = get<LHCb::HltLumiSummary>(m_inputBank);
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
