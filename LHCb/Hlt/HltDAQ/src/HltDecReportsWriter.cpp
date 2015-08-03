// $Id: HltDecReportsWriter.cpp,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
// Include files

#include "Event/HltDecReports.h"
#include "Event/RawEvent.h"

#include <functional>

// local
#include "HltDecReportsWriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDecReportsWriter
//
// 2008-07-26 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDecReportsWriter )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDecReportsWriter::HltDecReportsWriter( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputHltDecReportsLocation",
    m_inputHltDecReportsLocation= LHCb::HltDecReportsLocation::Default);
  declareProperty("OutputRawEventLocation",
    m_outputRawEventLocation= LHCb::RawEventLocation::Default);
  declareProperty("SourceID",
    m_sourceID= kSourceID_Dummy );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecReportsWriter::initialize()
{
  auto sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if( m_sourceID > kSourceID_Max || m_sourceID<0){
    return Error("Illegal SourceID specified; maximal allowed value is 7" , StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecReportsWriter::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get input
  const auto inputSummary = getIfExists<HltDecReports>(m_inputHltDecReportsLocation);
  if (!inputSummary) {
      return Warning( " No HltDecReports at " + m_inputHltDecReportsLocation.value(), StatusCode::SUCCESS, 20 );
  }
  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << " Input: ";
    for( const auto& rep : *inputSummary) {
      auto decRep = HltDecReport{rep.second.decReport()};
      verbose() << decRep.intDecisionID() << "-" << decRep.decision() << " ";
    }
    verbose() << endmsg;
  }

  // get output
  auto rawEvent = getIfExists<RawEvent>(m_outputRawEventLocation);
  if( !rawEvent) {
    return Error(" No RawEvent at " + m_outputRawEventLocation.value(), StatusCode::SUCCESS, 20 );
  }
  // delete any previously inserted dec reports
  for( const auto& b : rawEvent->banks( RawBank::HltDecReports ) ) {
    auto sourceID =  b->version() > 1 ? ( b->sourceID() >> kSourceID_BitShift ) : kSourceID_Hlt;
    if( m_sourceID != sourceID )continue;
    rawEvent->removeBank(b);
    Warning( " Deleted previously inserted HltDecReports bank " , StatusCode::SUCCESS, 20).ignore();
  }

  // compose the bank body
  std::vector<unsigned int> bankBody; bankBody.reserve( inputSummary->size() + 2 );
  bankBody.push_back( inputSummary->configuredTCK() );
  bankBody.push_back( inputSummary->taskID() );
  std::transform( std::begin(*inputSummary), std::end(*inputSummary),
                  std::back_inserter(bankBody),
                  [](HltDecReports::Container::const_reference r) { return r.second.decReport(); } );

  // order according to the values, essentially orders by intDecisionID
  // this is important since it will put "*Global" reports at the beginning of the bank
  // NOTE: we must skip the first two words (configuredTCK, taskID)
  std::sort( std::next( std::begin(bankBody), 2), std::end(bankBody) );

  // shift bits in sourceID for the same convention as in HltSelReports
  rawEvent->addBank(  int(m_sourceID<<kSourceID_BitShift), RawBank::HltDecReports, kVersionNumber, bankBody );

  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << " Output:  ";
    verbose() << " VersionNumber= " << kVersionNumber;
    verbose() << " SourceID= " << m_sourceID;
    auto i=std::begin(bankBody);
    verbose() << " configuredTCK = " << *i++ << " " ;
    verbose() << " taskID = " << *i++ << " " ;
    for( ; i!=std::end(bankBody);++i) {
      auto rep = HltDecReport{*i};
      verbose() << rep.intDecisionID() << "-" << rep.decision() << " ";
    }
    verbose() << endmsg;
  }
  return StatusCode::SUCCESS;
}
