// $Id: HltDecReportsReader.cpp,v 1.1.1.1 2008-08-02 16:40:07 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltDecReports.h"
#include "Event/RawEvent.h"



// local
#include "HltDecReportsReader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDecReportsReader
//
// 2008-08-02 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDecReportsReader );


using namespace LHCb;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDecReportsReader::HltDecReportsReader( const std::string& name,
                                          ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
  //  : HltBaseAlg ( name , pSvcLocator )
{

  declareProperty("OutputHltDecReportsLocation",
    m_outputHltDecReportsLocation= LHCb::HltDecReportsLocation::Default);  
  declareProperty("InputRawEventLocation",
    m_inputRawEventLocation= LHCb::RawEventLocation::Default);  


  m_hltANNSvc = 0;

}
//=============================================================================
// Destructor
//=============================================================================
HltDecReportsReader::~HltDecReportsReader() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecReportsReader::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  //  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltBaseAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecReportsReader::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get input
  if( !exist<RawEvent>(m_inputRawEventLocation) ){    
    error() << " No RawEvent at " << m_inputRawEventLocation << endmsg;
    return StatusCode::SUCCESS;  
  }  
  RawEvent* rawEvent = get<RawEvent>(m_inputRawEventLocation);

  // create output container and put it on TES
  HltDecReports* outputSummary = new HltDecReports();
  put( outputSummary, m_outputHltDecReportsLocation );


   // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltdecreportsRawBanks = rawEvent->banks( RawBank::HltDecReports );
  if( !hltdecreportsRawBanks.size() ){
    warning() << " No HltDecReports RawBank in RawEvent. Quiting. " << endmsg;
    return StatusCode::SUCCESS;  
  } else if( hltdecreportsRawBanks.size() != 1 ){
    warning() << " More then one HltDecReports RawBanks in RawEvent. Will process only the first one. " << endmsg;
  }
  const RawBank* hltdecreportsRawBank = *(hltdecreportsRawBanks.begin());
  if( hltdecreportsRawBank->version() > kVersionNumber ){
  }
  if( hltdecreportsRawBank->sourceID() != kSourceID ){
    warning() << " HltDecReports RawBank has unexpected source ID. Will try to decode it anyway." << endmsg;
  }

  // ----------------------------------------------------------

  std::vector< unsigned int > bankBody;
  unsigned int bankSize = (hltdecreportsRawBank->size()+3)/4; // from bytes to words
  for(unsigned int i=0; i!=bankSize; ++i){
    bankBody.push_back( (hltdecreportsRawBank->data())[i] );
  }

  // --------------------------------- get configuration --------------------

  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  

  // std::vector<IANNSvc::minor_value_type> hlt = m_hltANNSvc->items("SelectionID"); // old style
  // selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt.begin(),hlt.end() );

  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); // new style
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );

  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );

  // ---------------- loop over decisions in the bank body; insert them into the output container

  for( std::vector< unsigned int >::const_iterator idec=bankBody.begin();
       idec!=bankBody.end();++idec){

    HltDecReport* dec = new HltDecReport( *idec );
    int id=dec->intSelectionID();

    std::string selName="Dummy";
    switch(id){
    case 1: selName="Hlt1Global"; break;      
    case 2: selName="Hlt2Global"; break;
    case 3: selName="L0Global"; break;
    default:
      for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
           si!=selectionNameToIntMap.end();++si){
        if( si->second == id ){
          selName = si->first;
          break;
        }
      }
    }    
    if( selName != "Dummy" ){
      if( outputSummary->hasSelectionName( selName ) ){
        error() << " Duplicate decision report in storage " << endmsg;
      } else {
        outputSummary->insert( selName, *dec );
      }
    } else {
      error() << " No string key found for trigger decision in storage "
              << " id=" << id << endmsg;      
    }
  }

  // ---------------- put decision=false decisions for "*Decision"s in configuration

  // loop over all selections in this configuration
  for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
       si!=selectionNameToIntMap.end();++si){
    const std::string & selName = si->first;

    // don't bother if already in 
    if( outputSummary->hasSelectionName(selName) )continue;

    if( selName.find("Decision") != std::string::npos ){
      
      HltDecReport* selSumOut = new HltDecReport( 0, 0, 0, si->second );
      if( selSumOut->invalidIntSelectionID() ){
        delete selSumOut;
        warning() << " selectionName=" << selName << " has invalid intSelectionID=" << endmsg;
        continue;
      }

      // insert selection into the container
      if( outputSummary->insert( selName, *selSumOut ) == StatusCode::FAILURE ){
        error() << " Failed to add HltDecReport selectionName=" << selName 
                << " to its container "
                << endmsg;
      }    
    }
    

  }
 
  if ( msgLevel(MSG::VERBOSE) ){
    // debugging info
    verbose() << " ====== HltDecReports container size=" << outputSummary->size() << endmsg;  
    verbose() << *outputSummary << endmsg;
  }
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltDecReportsReader::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
  // return HltBaseAlg::finalize();  // must be called after all other actions
}

//=============================================================================
