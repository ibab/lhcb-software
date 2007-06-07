// $Id: BooleInit.cpp,v 1.24 2007-06-07 06:22:35 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from LHCbKernel
#include "Kernel/IEventTimeDecoder.h"
#include "Kernel/IGenericTool.h"

// from EventBase
#include "Event/ProcessHeader.h"

// from MCEvent
#include "Event/MCHeader.h"

// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

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
    m_memoryTool(0), m_odinTool(0)
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
  if ( "" == rootInTES() )
    m_memoryTool = tool<IGenericTool>( "MemoryTool", "BooleMemory", this, true );

  // Tool do put the ODIN object on the TES
  m_odinTool = tool<IEventTimeDecoder>( "OdinTimeDecoder" );
  
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
  if ( "" == rootInTES() ) m_memoryTool->execute();

  // Get the run and event number from the MC Header
  LHCb::MCHeader* evt = get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default,
                                             IgnoreRootInTES                 );
  if ( "" == rootInTES() ) printEventRun( evt->evtNumber(), evt->runNumber() );
  
  // Initialize the random number
  if ( "" == rootInTES() ) {
    std::vector<long int> seeds = getSeeds( evt->runNumber(), evt->evtNumber() );
    sc = this->initRndm( seeds );
    if ( sc.isFailure() ) return sc;  // error printed already by initRndm  

    // Create the Boole event header
    LHCb::ProcessHeader* header = new LHCb::ProcessHeader();
    header->setApplicationName( this->appName() );
    header->setApplicationVersion( this->appVersion() );
    header->setRunNumber( evt->runNumber() );
    header->setRandomSeeds( seeds );
    header->setCondDBTags( this->condDBTags() );
    put( header, LHCb::ProcessHeaderLocation::Digi );
  }
  
  // Create an empty RawEvent
  LHCb::RawEvent* raw = new LHCb::RawEvent();
  put( raw, LHCb::RawEventLocation::Default );

  // Add the ODIN bank (EDMS 704084 v2.0)
  unsigned int odin[9];
  odin[LHCb::ODIN::RunNumber]   = evt->runNumber();
  odin[LHCb::ODIN::EventType]   = 0;
  odin[LHCb::ODIN::OrbitNumber] = 0;
  odin[LHCb::ODIN::L0EventIDHi] = 0xFFFFFFFF & (evt->evtNumber() >> 32);
  odin[LHCb::ODIN::L0EventIDLo] = 0xFFFFFFFF & evt->evtNumber();
  odin[LHCb::ODIN::GPSTimeHi]   = 0xFFFFFFFF & (evt->evtTime()) >> 32;
  odin[LHCb::ODIN::GPSTimeLo]   = 0xFFFFFFFF & evt->evtTime();
  odin[LHCb::ODIN::Word7]       = 0; // Error bits, Detector status
  odin[LHCb::ODIN::Word8]       = 0; // Bunch current, Force bit, Bx/Readout/Triger type, Bunch ID
  
  LHCb::RawBank* odinBank = raw->createBank(0,LHCb::RawBank::ODIN,2, 8+36, odin);
  raw->adoptBank(odinBank, true);

  // Now decode the bank and put it on the TES, for use by other algorithms
  m_odinTool->getTime();

  return StatusCode::SUCCESS;
};

//=============================================================================
