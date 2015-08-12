// Include files 

// local
#include "CaloGetterInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloGetterInit
//
// 2009-04-17 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloGetterInit )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloGetterInit::CaloGetterInit( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_getter(0)
{
  declareProperty("ToolName" , m_name = "CaloGetter");
}
//=============================================================================
// Destructor
//=============================================================================
CaloGetterInit::~CaloGetterInit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloGetterInit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( UNLIKELY(msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;


  m_getter = tool<ICaloGetterTool>("CaloGetterTool", m_name );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloGetterInit::execute() {

  if ( UNLIKELY(msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  m_getter->update();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloGetterInit::finalize() {

  if ( UNLIKELY(msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
