// $Id: RichGlobalPIDMonitor.cpp,v 1.2 2004-03-16 13:43:35 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichGlobalPIDMonitor.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDMonitor
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
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
StatusCode RichGlobalPIDMonitor::initialize() {

  // Sets up various tools and services
  StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  debug() << "Initialize" << endreq;

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichGlobalPIDMonitor::execute() {

  debug() << "Execute" << endreq;

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDMonitor::finalize() {

  debug() << "Finalize" << endreq;

  // Execute base class method
  return RichRecAlgBase::finalize();
}
