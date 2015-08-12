// Include files 

// local
#include "MVAManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MVAManager
//
// 2013-08-06 : Sebastian Neubert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MVAManager )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MVAManager::MVAManager( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
   declareProperty
      ( "DictOfFunctors" , m_dictname ,
	"path/name of the DictOfFunctors tool"); 
   
    declareProperty
      ( "MVATransform" , m_transformname ,
	"path/name of the DictTransform tool"); 

  
}
//=============================================================================
// Destructor
//=============================================================================
MVAManager::~MVAManager() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MVAManager::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // setup the tool chain, starting with the DictOfFunctors
  //m_dictofFunctors = tool<IParticleDictTool>(m_dictname,this);
  //if(m_dictofFunctors == NULL) return StatusCode::FAILURE;
   
  // next the dict transform this will be public for access by the outside world
  m_mva = tool<IParticleDictTool>(m_transformname);
  if(m_mva == NULL) return StatusCode::FAILURE;
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MVAManager::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MVAManager::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
