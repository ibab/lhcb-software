// $Id: HltDecReportsWriter.cpp,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

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

}
//=============================================================================
// Destructor
//=============================================================================
HltDecReportsWriter::~HltDecReportsWriter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecReportsWriter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecReportsWriter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get input
  if( !exist<HltDecReports>(m_inputHltDecReportsLocation) ){    
    return Warning( " No HltDecReports at " + m_inputHltDecReportsLocation.value(), StatusCode::SUCCESS, 20 );
  }  
  const HltDecReports* inputSummary = get<HltDecReports>(m_inputHltDecReportsLocation);

  // get output
  if( !exist<RawEvent>(m_outputRawEventLocation) ){    
    return Error(" No RawEvent at " + m_outputRawEventLocation.value(), StatusCode::SUCCESS, 20 );
  }  
  RawEvent* rawEvent = get<RawEvent>(m_outputRawEventLocation);


  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << " Input: ";  
    for( HltDecReports::Container::const_iterator iRep=inputSummary->begin();
         iRep!=inputSummary->end();++iRep){
      const unsigned int decRep = iRep->second.decReport();
      verbose() << HltDecReport(decRep).intDecisionID() << "-" << HltDecReport(decRep).decision() << " ";
    }
    verbose() << endmsg;
  }
  

  // compose the bank body
  std::vector<unsigned int> bankBody;
  bankBody.push_back( inputSummary->configuredTCK() );
  bankBody.push_back( inputSummary->taskID() );

  for( HltDecReports::Container::const_iterator iRep=inputSummary->begin();
       iRep!=inputSummary->end();++iRep){
    const unsigned int decRep = iRep->second.decReport();
    //    if( decRep & 0x0000FFFFL ){  // only non-empty reports go into Raw Bank!      
      bankBody.push_back( decRep );
    // }
  }  

  // order according to the values, essentially orders by intDecisionID 
  // this is important since it will put "*Global" reports at the beginning of the bank
  // NOTE: we must skip the first two words (configuredTCK, taskID)
  if( !bankBody.empty() ) std::sort( bankBody.begin()+2, bankBody.end(), std::less<unsigned int>() );

  // delete any previously inserted dec reports
  const std::vector<RawBank*> hltdecreportsRawBanks = rawEvent->banks( RawBank::HltDecReports );
  for( std::vector<RawBank*>::const_iterator b=hltdecreportsRawBanks.begin();
       b!=hltdecreportsRawBanks.end(); ++b){
    rawEvent->removeBank(*b);
    if ( msgLevel(MSG::VERBOSE) ){ verbose() << " Deleted previosuly inserted HltDecReports bank " << endmsg;
    }    
  }
  
 
  rawEvent->addBank(  kSourceID, RawBank::HltDecReports, kVersionNumber, bankBody );

  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << " Output:  ";  
    std::vector<unsigned int>::const_iterator i=bankBody.begin();
    verbose() << " configuredTCK = " << *i++ << " " ;
    verbose() << " taskID = " << *i++ << " " ;
    for( ; i!=bankBody.end();++i) {
      verbose() << HltDecReport(*i).intDecisionID() << "-" << HltDecReport(*i).decision() << " ";
    }
    verbose() << endmsg;
  }

  return StatusCode::SUCCESS;
}

