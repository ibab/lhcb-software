// $Id: BooleInit.cpp,v 1.13 2006-02-14 14:01:57 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from LHCbKernel
#include "Kernel/IGenericTool.h"

// from EventBase
#include "Event/ProcessHeader.h"

// from MCEvent
#include "Event/MCHeader.h"

// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

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
    m_memoryTool(0)
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
  std::vector<long int> seeds = getSeeds( evt->runNumber(), evt->evtNumber() );
  sc = this->initRndm( seeds );
  if ( sc.isFailure() ) return sc;  // error printed already by initRndm  


  // Create the Boole event header
  LHCb::ProcessHeader* header = new LHCb::ProcessHeader();
  header->setApplicationName( this->appName() );
  header->setApplicationVersion( this->appVersion() );
  header->setRunNumber( evt->runNumber() );
  header->setRandomSeeds( seeds );
  put( header, LHCb::ProcessHeaderLocation::Digi );

  // Create an empty RawEvent
  LHCb::RawEvent* raw = new LHCb::RawEvent();
  put( raw, LHCb::RawEventLocation::Default );

  // Add a DAQ bank
  int data[3];
  data[0] = 0;  // Partition ID
  data[1] = evt->runNumber();
  data[2] = 0;  // Number of missing sources
  
  LHCb::RawBank* bank = raw->createBank(1, LHCb::RawBank::DAQ, 1, 12, data);
  raw->adoptBank(bank, true);
  
  return StatusCode::SUCCESS;
};

//=============================================================================
