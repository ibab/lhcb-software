// $Id: HelloWorld.cpp,v 1.2 2007-01-24 20:42:24 frankb Exp $

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "HelloWorld.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(HelloWorld)

// Constructor
//------------------------------------------------------------------------------
HelloWorld::HelloWorld(const std::string& name, ISvcLocator* ploc)
           : Algorithm(name, ploc) {
//------------------------------------------------------------------------------
  m_initialized = false;
}

//------------------------------------------------------------------------------
StatusCode HelloWorld::initialize() {
//------------------------------------------------------------------------------
  // avoid calling initialize more than once
  if( m_initialized ) return StatusCode::SUCCESS;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initializing...." << endreq;
  
  m_initialized = true;
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode HelloWorld::execute() {
//------------------------------------------------------------------------------
  MsgStream         log( msgSvc(), name() );
//  log << MSG::INFO << "executing...." << endreq;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode HelloWorld::finalize() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalizing...." << endreq;

  m_initialized = false;
  return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
StatusCode HelloWorld::beginRun() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "beginning new run...." << endreq;
  
  m_initialized = true;
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode HelloWorld::endRun() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "ending new run...." << endreq;
  
  m_initialized = true;
  return StatusCode::SUCCESS;
}
