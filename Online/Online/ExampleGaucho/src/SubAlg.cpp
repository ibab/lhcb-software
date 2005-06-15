// $Id: SubAlg.cpp,v 1.1.1.1 2005-06-15 16:19:21 cattanem Exp $

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "SubAlg.h"


// Static Factory declaration
static const AlgFactory<SubAlg>  Factory;
const IAlgFactory& SubAlgFactory = Factory;

// Constructor
//------------------------------------------------------------------------------
SubAlg::SubAlg(const std::string& name, ISvcLocator* ploc)
           : Algorithm(name, ploc) {
//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
StatusCode SubAlg::initialize() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initializing...." << endreq;
//  return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode SubAlg::execute() {
//------------------------------------------------------------------------------
  MsgStream         log( msgSvc(), name() );
//  log << MSG::INFO << "executing...." << endreq;
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode SubAlg::finalize() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
//  log << MSG::INFO << "finalizing...." << endreq;
  return StatusCode::SUCCESS;
}
