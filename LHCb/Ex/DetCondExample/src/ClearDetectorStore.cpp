//$Id: ClearDetectorStore.cpp,v 1.3 2001-11-30 09:35:48 andreav Exp $
#include <stdio.h>

#include "ClearDetectorStore.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<ClearDetectorStore> Factory;
const IAlgFactory& ClearDetectorStoreFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
ClearDetectorStore::ClearDetectorStore( const std::string&  name, 
			ISvcLocator*        pSvcLocator )
  : Algorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 
/// It creates the environment needed for processing the events. 
/// In this example this is the main method, where we play with the CondDB.

StatusCode ClearDetectorStore::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;

  // Query the IDataManagerSvc interface of the detector data service
  IDataManagerSvc* detDataMgr;
  StatusCode sc = detSvc()->queryInterface
    ( IID_IDataManagerSvc, (void **)&detDataMgr);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not query IDataManagerSvc interface of DetectorDataSvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IDataManagerSvc interface of DetectorDataSvc" 
	<< endreq;
  }

  // Clear the store
  log << MSG::INFO 
      << "Clear the Detector Data Store to start from scratch" << endreq;
  if ( !detDataMgr->clearStore().isSuccess() ) {
    log << MSG::WARNING
	<< "The detector data store has not been cleared: already empty?" 
	<< endreq;    
  } else {
    log << MSG::INFO 
	<< "The detector data store has been cleared" << endreq;    
  }
  
  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode ClearDetectorStore::execute( ) {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode ClearDetectorStore::finalize( ) {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
