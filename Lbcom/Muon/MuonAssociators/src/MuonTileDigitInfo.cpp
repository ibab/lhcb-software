// $Id: MuonTileDigitInfo.cpp,v 1.1 2006-01-27 17:15:59 asarti Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "MuonTileDigitInfo.h"
#include "Event/IntLink.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTileDigitInfo
//
// 2006-01-22 : Alessio Sarti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonTileDigitInfo );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTileDigitInfo::MuonTileDigitInfo( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
MuonTileDigitInfo::~MuonTileDigitInfo() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonTileDigitInfo::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};


//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonTileDigitInfo::execute() {

  debug() << "==> Execute" << endmsg;
  LHCb::IntLink myIntLink = LHCb::IntLink::IntLink();
  std::map<int,int> mylink; ///< list of linked ints

  SmartDataPtr<LHCb::MuonDigits> digits(eventSvc(),
                                        LHCb::MuonDigitLocation::MuonDigit);
  if (0 == digits){    
    error() << "Failed to find MuonDigits" << endreq;
    return StatusCode::FAILURE;
  }
  
  // get the MCMuonDigits
  SmartDataPtr<LHCb::MCMuonDigits> mcDigits(eventSvc(), 
					    LHCb::MCMuonDigitLocation::MCMuonDigit);

  // loop and link MuonDigits to MC truth
  LHCb::MuonDigits::const_iterator iDigit;
  for(iDigit = digits->begin(); iDigit != digits->end(); iDigit++){
    
    // match the MCMuonDigit to the MuonDigit via the Key
    LHCb::MCMuonDigit * mcDigit = mcDigits->object((*iDigit)->key());
    if(!mcDigit) {
      error() << "Could not find the match for " << (*iDigit)->key()
	      << " in " << LHCb::MCMuonDigitLocation::MCMuonDigit << endreq;
      return StatusCode::FAILURE;
    }
    
    unsigned int digitinfo = mcDigit->DigitInfo().DigitInfo();  
    long int tile = (*iDigit)->key();  
    
    mylink.insert(std::make_pair(tile,digitinfo));
    
  }
  const std::map<int,int>& myList = mylink;
  myIntLink.setLink(myList);
  return StatusCode::SUCCESS;
};


//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonTileDigitInfo::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
