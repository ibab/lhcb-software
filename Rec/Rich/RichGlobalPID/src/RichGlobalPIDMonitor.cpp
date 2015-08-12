
/** @file RichGlobalPIDMonitor.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::MC::Monitor
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */

// from Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichGlobalPIDMonitor.h"

using namespace Rich::Rec::GlobalPID::MC;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Monitor )

// Standard constructor, initializes variables
Monitor::Monitor( const std::string& name,
                  ISvcLocator* pSvcLocator )
  : Rich::Rec::HistoAlgBase ( name, pSvcLocator ) { }

// Destructor
Monitor::~Monitor() {}

//  Initialize
StatusCode Monitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode Monitor::execute()
{
  debug() << "Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode Monitor::finalize()
{
  // Execute base class method
  return Rich::Rec::HistoAlgBase::finalize();
}
