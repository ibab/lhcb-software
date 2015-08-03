// Include files 

// local
#include "STErrorDecoding.h"

// Event
#include "Event/STTELL1BoardErrorBank.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : STErrorDecoding
//
// 2007-11-29: Mathias Knecht
// Update 2008 M Needham
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STErrorDecoding )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STErrorDecoding::STErrorDecoding( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : STDecodingBaseAlg ( name , pSvcLocator ){
 
  declareProperty("PrintErrorInfo", m_PrintErrorInfo = false);  
}

//=============================================================================
// Destructor
//=============================================================================
STErrorDecoding::~STErrorDecoding() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode STErrorDecoding::execute() {

  // in fact all the work is delegated to the base class

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute " << endmsg;

  STTELL1BoardErrorBanks* errorBanks = getErrorBanks();

  if (errorBanks == 0) {
    // was not possible to decode the banks
    return StatusCode::FAILURE;
  }
   
  // print out the error banks
  if (m_PrintErrorInfo == true){
    STTELL1BoardErrorBanks::const_iterator iterBank = errorBanks->begin();
    for ( ; iterBank != errorBanks->end();  ++iterBank){
      info() << **iterBank << endmsg;
    }  //iterBanks
  }
  
  return StatusCode::SUCCESS;
} 

