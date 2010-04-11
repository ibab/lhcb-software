// $Id: MooreInit.cpp,v 1.2 2010-04-11 15:23:34 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 


#include "Event/ODIN.h"

// local
#include "MooreInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MooreInit
//
// 2005-12-15 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MooreInit );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MooreInit::MooreInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator )
  , m_odinTool("ODINDecodeTool",this)
{ 
}
//=============================================================================
// Destructor
//=============================================================================
MooreInit::~MooreInit() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MooreInit::initialize() {
  //StatusCode sc = LbAppInit::initialize(); // must be executed first
  StatusCode sc = GaudiAlgorithm::initialize(); // very dirty, to avoid printing...
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Initialize" << endmsg;

  // Tool to decode ODIN object to RawEvent
  return m_odinTool.retrieve();
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MooreInit::execute() {

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Execute" << endmsg;

  // Initialize the random number: 

  SmartDataPtr<LHCb::ODIN> odin( evtSvc() , LHCb::ODINLocation::Default);
  if (!odin) m_odinTool->execute();
  if (!odin) {
        error() << " Could not obtain ODIN...  " << endmsg;
        return StatusCode::FAILURE;
  }

  std::vector<long int> seeds = getSeeds( odin->runNumber(), odin->eventNumber() );
  sc = this->initRndm( seeds );
  if ( sc.isFailure() ) return sc;  // error printed already by initRndm  


  return StatusCode::SUCCESS;
};

StatusCode MooreInit::finalize() {
  //StatusCode sc = LbAppInit::finalize(); // must be executed first
  return GaudiAlgorithm::finalize(); // very dirty, to avoid printing...

};

