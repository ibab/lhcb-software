
//-----------------------------------------------------------------------------
/** @file RichLoadRawEvent.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RichLoadRawEvent
 *
 *  CVS Log :-
 *  $Id: RichLoadRawEvent.cpp,v 1.1 2006-10-04 10:46:47 jonrob Exp $
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

// Declaration of the Algorithm Factory
static const  AlgFactory<RichLoadRawEvent>          s_factory ;
const        IAlgFactory& RichLoadRawEventFactory = s_factory ;

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

