// $Id: RichGlobalPIDMonitor.cpp,v 1.1.1.1 2003-06-30 16:10:55 jonesc Exp $
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

  MsgStream msg( msgSvc(), name() );

  // Sets up various tools and services
  if ( !RichRecAlgBase::initialize() ) return StatusCode::FAILURE;

  msg << MSG::DEBUG << "Initialize" << endreq;

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichGlobalPIDMonitor::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDMonitor::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecAlgBase::finalize();
}
