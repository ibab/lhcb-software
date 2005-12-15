// $Id: BooleInit.cpp,v 1.9 2005-12-15 13:56:37 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from LHCbKernel
#include "Kernel/IGenericTool.h"
#include "Kernel/IRndmTool.h"

// from MCEvent
#include "Event/MCHeader.h"

// local
#include "BooleInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BooleInit
//
// 2005-12-15 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BooleInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BooleInit::BooleInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), 
    m_memoryTool(0), 
    m_initRndmTool(0),
    m_eventCounter(0)
{

}
//=============================================================================
// Destructor
//=============================================================================
BooleInit::~BooleInit() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode BooleInit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Private tool to plot the memory usage
  m_memoryTool = tool<IGenericTool>( "MemoryTool", "BooleMemory", this, true );
  
  // Private tool to initialize random number
  m_initRndmTool = tool<IRndmTool>( "InitRndmTool", "BooleRndm", this, true );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BooleInit::execute() {

  debug() << "==> Execute" << endmsg;

  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the MC Header
  ++m_eventCounter;
  LHCb::MCHeader* evt = get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default );
  info() << "Evt " << evt->evtNumber() << ",  Run " << evt->runNumber()
         << ",  Nr. in job = " << m_eventCounter << endmsg;

  // Initialize the random number
  m_initRndmTool->initRndm( evt->runNumber(), evt->evtNumber() );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BooleInit::finalize() {

  always()
    << std::endl
    << "=================================================================="
    << "=================================================================="
    << std::endl
    << "                                                 "
    << m_eventCounter << " events processed" << std::endl
    << "=================================================================="
    << "=================================================================="
    << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
