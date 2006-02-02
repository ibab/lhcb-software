// $Id: BooleInit.cpp,v 1.11 2006-02-02 07:43:44 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from LHCbKernel
#include "Kernel/IGenericTool.h"
#include "Kernel/IRndmTool.h"

// from MCEvent
#include "Event/MCHeader.h"

// from DAQEvent
#include "Event/RawEvent.h"

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
  : LbAppInit ( name , pSvcLocator ), 
    m_memoryTool(0), 
    m_initRndmTool(0)
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
  StatusCode sc = LbAppInit::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

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

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Execute" << endmsg;

  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the MC Header
  LHCb::MCHeader* evt = get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default );
  this->printEventRun( evt->evtNumber(), evt->runNumber() );
  
  // Initialize the random number
  m_initRndmTool->initRndm( evt->runNumber(), evt->evtNumber() );

  // Create an empty RawEvent
  LHCb::RawEvent* raw = new LHCb::RawEvent();
  put( raw, LHCb::RawEventLocation::Default );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BooleInit::finalize() {

  return LbAppInit::finalize();  // must be called after all other actions
}

//=============================================================================
