// $Id: RawEventCreator.cpp,v 1.1 2007-12-07 14:34:05 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCb
#include "Kernel/IEventTimeDecoder.h"

// local
#include "Event/RawEvent.h"
#include "Event/ODIN.h"

#include "RawEventCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawEventCreator
//
// 2005-10-13 : Markus Frank
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DAQEventTests,RawEventCreator)

#ifndef ODIN_VERSION
#define ODIN_VERSION 3
#endif

using namespace LHCb;
using namespace DAQEventTests;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawEventCreator::RawEventCreator( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_eventTimeDecoder(NULL)
{
  
}
//=============================================================================
// Destructor
//=============================================================================
RawEventCreator::~RawEventCreator() {}; 

StatusCode RawEventCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  
  m_eventTimeDecoder = tool<IEventTimeDecoder>("OdinTimeDecoder",this,true);
  
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawEventCreator::execute() {

  debug() << "==> Execute" << endmsg;

  RawEvent* raw = new RawEvent();
  for(int i=0; i<16; ++i)  {
    int cnt = 0;
    int len  = (i+1)*64;
    RawBank* bank = raw->createBank(i, RawBank::DAQ, 1, len, 0);
    for(int* p=bank->begin<int>(); p != bank->end<int>(); ++p)  {
      *p = cnt++;
    }
    raw->adoptBank(bank, true);
  }

  for(int i=0; i<9; ++i)  {
    int cnt = 0;
    int len  = (i+1)*32;
    RawBank* bank = raw->createBank(i, RawBank::PrsE, 1, len, 0);
    for(int* p=bank->begin<int>(); p != bank->end<int>(); ++p)  {
      *p = cnt++;
    }
    raw->adoptBank(bank, true);
  }

  { // Fake ODIN Bank
#if ODIN_VERSION == 1 || ODIN_VERSION == 2
    const int words = 9;
    int len = sizeof(unsigned int) * words;
    unsigned int data[words] = 
      {
        1984,   // run number 
        0xACDC, // event type 
        23,     // orbit id
        1,      // L0 event ID (HI)
        1,      // L0 event ID (LO)
        0x00041E09, // GPS Time (HI)
        0xC736E521, // GPS Time (LO)... Fri Sep 22 14:14:12.645153 2006
        0xAABBBBBB, // Error Bits (AA) and Detector Status (BBBBBB)
        0x11CF0123  // Bunch Current (11), BXType (beam crossing), Force (false),
                    // ReadoutType (NonZeroSuppression), TriggerType (7),
                    // Bunch Id (123)
      };
#else // ODIN_VERSION == 3
    const int words = 10;
    int len = sizeof(unsigned int) * words;
    unsigned int data[words] = 
      {
        1973,       // run number 
        0xCAFEACDC, // Calibration Step Number (CAFE), event type (ACDC) 
        23,         // orbit id
        1,          // L0 event ID (HI)
        1,          // L0 event ID (LO)
        0x000440B1, // GPS Time (HI)
        0xE73F4B05, // GPS Time (LO)... Fri Dec  7 14:05:39.916549 2007
        0xAABBBBBB, // Error Bits (AA) and Detector Status (BBBBBB)
        0x11CF0123, // Bunch Current (11), BXType (beam crossing), Force (false),
                    // ReadoutType (NonZeroSuppression), TriggerType (7),
                    // Bunch Id (123)
        0x01234567  // Trigger Configuration Key
      };
#endif
    RawBank* bank = raw->createBank(0, RawBank::ODIN, ODIN_VERSION, len, data);
    raw->adoptBank(bank, true);
  }

  put( raw, RawEventLocation::Default );

  // triggers a decoding of the ODIN bank
  m_eventTimeDecoder->getTime();
  
  info() << " ======== ODIN data ========\n";
  LHCb::ODIN *odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  info() << *odin << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
