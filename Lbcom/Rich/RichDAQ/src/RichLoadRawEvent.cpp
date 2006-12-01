
//-----------------------------------------------------------------------------
/** @file RichLoadRawEvent.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RichLoadRawEvent
 *
 *  CVS Log :-
 *  $Id: RichLoadRawEvent.cpp,v 1.2 2006-12-01 13:03:31 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichLoadRawEvent.h"

// namespaces
using namespace LHCb; ///< General LHCb namespace

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RichLoadRawEvent );

// Standard constructor, initializes variables
RichLoadRawEvent::RichLoadRawEvent( const std::string& name,
                                    ISvcLocator* pSvcLocator )
  : RichAlgBase ( name , pSvcLocator ) { }

// Destructor
RichLoadRawEvent::~RichLoadRawEvent() {};

// Main execution
StatusCode RichLoadRawEvent::execute()
{
  // load raw event data object
  get<RawEvent>( RawEventLocation::Default );

  // return OK
  return StatusCode::SUCCESS;
}

