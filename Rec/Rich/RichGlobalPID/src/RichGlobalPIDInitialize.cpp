//------------------------------------------------------------------------------------
/** @file RichGlobalPIDInitialize.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Initialize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDInitialize.cpp,v 1.14 2007-12-14 14:21:18 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//------------------------------------------------------------------------------------

// local
#include "RichGlobalPIDInitialize.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Initialize );

// Standard constructor, initializes variables
Initialize::Initialize( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : AlgBase ( name, pSvcLocator ) {}

// Destructor
Initialize::~Initialize() {}

//  Initialize
StatusCode Initialize::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biased any timing numbers
  statusCreator();   // pre-load the status creator

  return sc;
}

// Main execution
StatusCode Initialize::execute()
{
  // Set event status to OK for start of GlobalPID processing
  richStatus()->setEventOK( true );

  // create the TES event
  createGPIDEvent();

  return StatusCode::SUCCESS;
}
