// $Id: RawEventCreator.cpp,v 1.2 2006-09-26 10:55:58 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "Event/RawEvent.h"
#include "RawEventCreator.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawEventCreator
//
// 2005-10-13 : Markus Frank
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RawEventCreator>          s_factory ;
const        IAlgFactory& RawEventCreatorFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawEventCreator::RawEventCreator( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
RawEventCreator::~RawEventCreator() {}; 


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
        0xC736E521, // GPS Time (LO)... Fri Sep 22 14:14:12 2006
        0xAABBBBBB, // Error Bits (AA) and Detector Status (BBBBBB)
        0x11CF0123  // Bunch Current (11), BXType (beam crossing), Force (false),
                    // ReadoutType (NonZeroSuppression), TriggerType (7),
                    // Bunch Id (123)
      };
    
    RawBank* bank = raw->createBank(0, RawBank::ODIN, 1, len, data);
    raw->adoptBank(bank, true);
  }

  put( raw, RawEventLocation::Default );

  return StatusCode::SUCCESS;
};

//=============================================================================
