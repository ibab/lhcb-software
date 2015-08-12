// Include files 

// event model
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"
#include "Event/LumiIntegral.h"
#include "Event/LumiCounters.h"

// local
#include "LumiReadBackFSR.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiReadBackFSR
//
// The purpose of this algorithm is to check the integrator tool
// 
// 2009-02-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiReadBackFSR )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiReadBackFSR::LumiReadBackFSR( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_integratorTool(NULL)
{
  declareProperty( "IntegratorToolName" , m_ToolName       = "LumiIntegrator" );  
}
//=============================================================================
// Destructor
//=============================================================================
LumiReadBackFSR::~LumiReadBackFSR() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiReadBackFSR::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // prepare tool
  m_integratorTool = tool<ILumiIntegrator>( "LumiIntegrator", m_ToolName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiReadBackFSR::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiReadBackFSR::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // use tool to count events for this file
  info() << "number of events seen: " << m_integratorTool->events( ) << endmsg;

  // use tool to get summay for this file
  info() << "integrated normalization: " << m_integratorTool->integral( ) << endmsg;

  // final results
  info() << "Integrated luminosity: " 
	 << m_integratorTool->lumiValue() << " +/- "
	 << m_integratorTool->lumiError() << " [pb-1]" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
