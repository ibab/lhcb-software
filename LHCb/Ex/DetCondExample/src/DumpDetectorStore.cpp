//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/DumpDetectorStore.cpp,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#include <stdio.h>

#include "DumpDetectorStore.h"
#include "DetDataAgent.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<DumpDetectorStore> Factory;
const IAlgFactory& DumpDetectorStoreFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
DumpDetectorStore::DumpDetectorStore( const std::string&  name, 
			      ISvcLocator*        pSvcLocator )
  : Algorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 

StatusCode DumpDetectorStore::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode DumpDetectorStore::execute( ) {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode DumpDetectorStore::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize()" << endreq;
  
  // Locate the IDataManagerSvc interface of the Detector Data Service
  IDataManagerSvc* storeManager;
  StatusCode sc = detSvc()->queryInterface
    ( IID_IDataManagerSvc, (void **)&storeManager);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Can't query IDataManagerSvc interface" << endreq;
    return sc;
  }

  // Traverse the transient data store and dump all contents
  DetDataAgent agent( msgSvc() );
  log << MSG::INFO << "Now dump all contents of the data store" << endreq;
  StatusCode status = storeManager->traverseTree( &agent );
  if ( status.isSuccess() ) {
    log << MSG::INFO << "Dumped all contents of the data store" << endreq;
  } else {
    log << MSG::INFO << "Nothing was found in the data store" << endreq;
  }
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
