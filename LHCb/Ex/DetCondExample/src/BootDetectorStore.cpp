//$Id: BootDetectorStore.cpp,v 1.1 2001-11-29 13:16:04 andreav Exp $
#include <stdio.h>

#include "BootDetectorStore.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<BootDetectorStore> Factory;
const IAlgFactory& BootDetectorStoreFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
BootDetectorStore::BootDetectorStore( const std::string&  name, 
				      ISvcLocator*        pSvcLocator )
  : Algorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 

StatusCode BootDetectorStore::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode BootDetectorStore::execute( ) {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode BootDetectorStore::finalize( ) {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
