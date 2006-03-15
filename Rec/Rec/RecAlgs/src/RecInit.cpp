// $Id: RecInit.cpp,v 1.1.1.1 2006-03-15 16:25:05 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from LHCbKernel
#include "Kernel/IGenericTool.h"

// from EventBase
#include "Event/ProcessHeader.h"

// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODINDefinitions.h"

// from RecEvent
#include "Event/RecHeader.h"
#include "Event/ProcStatus.h"

// local
#include "RecInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecInit
//
// 2006-03-14 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RecInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecInit::RecInit( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
RecInit::~RecInit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RecInit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Private tool to plot the memory usage
  m_memoryTool = tool<IGenericTool>( "MemoryTool", "BrunelMemory", this, true );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RecInit::execute() {

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Execute" << endmsg;
  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the ODIN bank
  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent> ( LHCb::RawEventLocation::Default );
  const std::vector<LHCb::RawBank*>* banks = &rawEvt->banks( LHCb::RawBank::ODIN );
  if ( 0 == banks->size() ) return Error( "ODIN bank not found" );

  std::vector<LHCb::RawBank*>::const_iterator itB = banks->begin();
  unsigned int* odinData = (*itB)->data();
  unsigned int runNumber = odinData[LHCb::ODIN::RunNumber];
  ulonglong temp = odinData[LHCb::ODIN::L0EventIDHi];
  ulonglong evtNumber = (temp << 32) + odinData[LHCb::ODIN::L0EventIDLo];
  temp = odinData[LHCb::ODIN::GPSTimeHi];
  ulonglong evtTime   = (temp << 32) + odinData[LHCb::ODIN::GPSTimeLo];

  this->printEventRun( evtNumber, runNumber );
  debug() << "Event time: " << evtTime << endmsg;

  // Initialize the random number
  std::vector<long int> seeds = getSeeds( runNumber, evtNumber );
  sc = this->initRndm( seeds );
  if ( sc.isFailure() ) return sc;  // error printed already by initRndm


  // Create the Reconstruction event header
  LHCb::RecHeader* header = new LHCb::RecHeader();
  header->setApplicationName( this->appName() );
  header->setApplicationVersion( this->appVersion() );
  header->setRunNumber( runNumber );
  header->setEvtNumber( evtNumber );
  header->setEvtTime( evtTime );
  header->setRandomSeeds( seeds );
  put( header, LHCb::RecHeaderLocation::Default );

  // Create a ProcStatus if it does not already exist
  if( !exist<LHCb::ProcStatus>( LHCb::ProcStatusLocation::Default ) ) {
    LHCb::ProcStatus* procStat = new LHCb::ProcStatus();
    put( procStat, LHCb::ProcStatusLocation::Default );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
