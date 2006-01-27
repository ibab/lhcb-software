// $Id: SimInit.cpp,v 1.2 2006-01-27 19:26:37 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SmartIF.h"

// from LHCbKernel
#include "Kernel/IGenericTool.h"
#include "Kernel/IRndmTool.h"

// from GenEvent
#include "Event/GenHeader.h"

// from MCEvent
#include "Event/MCHeader.h"

// local
#include "SimInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimInit
//
// 2006-01-16 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SimInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimInit::SimInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator ),
    m_memoryTool(0), 
    m_initRndmTool(0)
{

}

//=============================================================================
// Destructor
//=============================================================================
SimInit::~SimInit() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimInit::initialize() {
  StatusCode sc = LbAppInit::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Initialize" << endmsg;

  // Private tool to plot the memory usage
  std::string toolName = name()+"Memory";
  m_memoryTool = tool<IGenericTool>( "MemoryTool", toolName, this, true );
  
  // Private tool to initialize random number
  toolName = name()+"Rndm";
  m_initRndmTool = tool<IRndmTool>( "InitRndmTool", toolName, this, true );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode SimInit::execute() {

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Execute" << endmsg;

  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the GenHeader if it exist (i.e.
  // Generator phase already run or events read from file)
  LHCb::GenHeader* gen = 
    get<LHCb::GenHeader>( LHCb::GenHeaderLocation::Default );
  this->printEventRun( gen->evtNumber(), gen->runNumber() );

  // Initialize the random number
  m_initRndmTool->initRndm( gen->runNumber(), gen->evtNumber() );  

  // Create MCHeader and partially fill it - updated in Simulation phase
  LHCb::MCHeader* mc = new LHCb::MCHeader();

  mc->setApplicationName( this->appName() ); // update it later?
  mc->setApplicationVersion( this->appVersion() );
  mc->setRunNumber( gen->runNumber() );
  mc->setEvtNumber( gen->evtNumber() );
  mc->setEvtTime( 0 ); // need to get it from EventClockSvc

  verbose() << "GenHeader " << *gen << endmsg;
  verbose() << "MCHeader " << *mc << endmsg;

  put( mc, LHCb::MCHeaderLocation::Default );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SimInit::finalize() {

  debug() << "==> Finalize" << endmsg;

  return LbAppInit::finalize();  // must be called after all other actions
}

//=============================================================================
