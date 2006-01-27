// $Id: GenInit.cpp,v 1.2 2006-01-27 19:26:32 gcorti Exp $
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
#include "GenInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenInit
//
// 2006-01-16 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GenInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenInit::GenInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator ),
    m_memoryTool(0), 
    m_initRndmTool(0)
{
  declareProperty( "FirstEventNumber", m_firstEvent = 1 );
  declareProperty( "RunNumber",        m_runNumber  = 1 );
}

//=============================================================================
// Destructor
//=============================================================================
GenInit::~GenInit() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GenInit::initialize() {
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
StatusCode GenInit::execute() {

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Execute" << endmsg;

  // Plot the memory usage
  m_memoryTool->execute();

  // Initialize the random number
  longlong eventNumber = m_firstEvent - 1 + this->eventCounter();
  m_initRndmTool->initRndm( m_runNumber, eventNumber );
  this->printEventRun( eventNumber, m_runNumber );
  
  // Create GenHeader (running Generator phase) and partially fill it
  // - updated during execution
  LHCb::GenHeader* gen = new LHCb::GenHeader();

  gen->setApplicationName( this->appName() );
  gen->setApplicationVersion( this->appVersion() );
  gen->setRunNumber( m_runNumber );
  gen->setEvtNumber( eventNumber );

  verbose() << "GenHeader " << *gen << endmsg;

  put( gen, LHCb::GenHeaderLocation::Default );    

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GenInit::finalize() {

  debug() << "==> Finalize" << endmsg;

  return LbAppInit::finalize();  // must be called after all other actions
}

//=============================================================================
