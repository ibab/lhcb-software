// Include files

#include "Event/IntLink.h"
#include "Event/MuonDigit.h"
#include "Event/MCMuonDigit.h"

// local
#include "MuonTileDigitInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTileDigitInfo
//
// 2006-01-22 : Alessio Sarti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonTileDigitInfo )

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
MuonTileDigitInfo::~MuonTileDigitInfo() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonTileDigitInfo::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonTileDigitInfo::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::IntLink * myIntLink = new LHCb::IntLink();
  std::map<int,int> mylink; ///< list of linked ints

  LHCb::MuonDigits* digits = 
    get<LHCb::MuonDigits>(LHCb::MuonDigitLocation::MuonDigit);

  LHCb::MCMuonDigits* mcDigits = 
    get<LHCb::MCMuonDigits>(LHCb::MCMuonDigitLocation::MCMuonDigit);

  // loop and link MuonDigits to MC truth
  LHCb::MuonDigits::const_iterator iDigit;
  for(iDigit = digits->begin(); iDigit != digits->end(); iDigit++){
    
    // match the MCMuonDigit to the MuonDigit via the Key
    LHCb::MCMuonDigit * mcDigit = mcDigits->object((*iDigit)->key());
    if(!mcDigit) {
      error() << "Could not find the match for " << (*iDigit)->key()
	      << " in " << LHCb::MCMuonDigitLocation::MCMuonDigit << endmsg;
      return StatusCode::FAILURE;
    }
    
    unsigned int digitinfo = mcDigit->DigitInfo().DigitInfo();  
    long int tile = (*iDigit)->key();  
    
    mylink.insert(std::make_pair(tile,digitinfo));
    
  }
  const std::map<int,int>& myList = mylink;
  myIntLink->setLink(myList);
  std::string path = LHCb::MCMuonDigitLocation::MCMuonDigit + "Info";
    
  put(myIntLink,path.data());
  return StatusCode::SUCCESS;
}
