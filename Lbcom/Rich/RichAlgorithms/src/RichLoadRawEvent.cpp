
//-----------------------------------------------------------------------------
/** @file RichLoadRawEvent.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RichLoadRawEvent
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichLoadRawEvent.h"

// RICH DAQ
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( LoadRawEvent )

// Standard constructor, initializes variables
LoadRawEvent::LoadRawEvent( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : Rich::AlgBase ( name , pSvcLocator ) { }

// Destructor
LoadRawEvent::~LoadRawEvent() {}

// Main execution
StatusCode LoadRawEvent::execute()
{
  // load raw event data object
  get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );

  // return OK
  return StatusCode::SUCCESS;
}

