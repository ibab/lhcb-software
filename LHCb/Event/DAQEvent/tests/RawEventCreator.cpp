// $Id: RawEventCreator.cpp,v 1.1 2005-10-14 12:48:50 cattanem Exp $
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

  put( raw, RawEventLocation::Default );

  return StatusCode::SUCCESS;
};

//=============================================================================
