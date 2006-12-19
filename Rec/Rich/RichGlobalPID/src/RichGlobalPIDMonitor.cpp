
/** @file RichGlobalPIDMonitor.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : RichGlobalPIDMonitor
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDMonitor.cpp,v 1.6 2006-12-19 09:06:21 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichGlobalPIDMonitor.h"

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RichGlobalPIDMonitor );

// Standard constructor, initializes variables
RichGlobalPIDMonitor::RichGlobalPIDMonitor( const std::string& name,
                                            ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) { }

// Destructor
RichGlobalPIDMonitor::~RichGlobalPIDMonitor() {}

//  Initialize
StatusCode RichGlobalPIDMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichGlobalPIDAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichGlobalPIDMonitor::execute()
{
  debug() << "Execute" << endreq;

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDMonitor::finalize()
{
  // Execute base class method
  return RichGlobalPIDAlgBase::finalize();
}
