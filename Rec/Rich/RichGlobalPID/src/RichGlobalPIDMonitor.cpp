
/** @file RichGlobalPIDMonitor.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : RichGlobalPIDMonitor
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDMonitor.cpp,v 1.4 2004-07-27 10:56:37 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichGlobalPIDMonitor.h"

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDMonitor>          s_factory ;
const        IAlgFactory& RichGlobalPIDMonitorFactory = s_factory ;

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
  const StatusCode sc = RichRecAlgBase::initialize();
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
  return RichRecAlgBase::finalize();
}
