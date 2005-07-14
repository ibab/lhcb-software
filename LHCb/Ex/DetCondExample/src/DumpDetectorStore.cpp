//$Id: DumpDetectorStore.cpp,v 1.3 2005-07-14 15:11:17 marcocle Exp $
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
DumpDetectorStore::DumpDetectorStore( const std::string&  name, ISvcLocator* pSvcLocator )
  : GaudiAlgorithm(name, pSvcLocator)
{}

//----------------------------------------------------------------------------

StatusCode DumpDetectorStore::execute(){ return StatusCode::SUCCESS; }

//----------------------------------------------------------------------------

StatusCode DumpDetectorStore::finalize( ) {

  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize()" << endmsg;
  
  // Locate the IDataManagerSvc interface of the Detector Data Service
  IDataManagerSvc* detDataMgr;
  sc = detSvc()->queryInterface ( IID_IDataManagerSvc, (void **)&detDataMgr);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Can't query IDataManagerSvc interface" << endmsg;
    return sc;
  }

  // Traverse the transient data store and dump all contents
  DetDataAgent agent( msgSvc() );
  log << MSG::INFO << "Now dump all contents of the data store" << endmsg;
  sc = detDataMgr->traverseTree( &agent );
  if ( sc.isSuccess() ) {
    log << MSG::INFO << "Dumped all contents of the data store" << endmsg;
  } else {
    log << MSG::INFO << "Nothing was found in the data store" << endmsg;
  }
  
  detDataMgr->release();
  
  return GaudiAlgorithm::finalize();
}

//----------------------------------------------------------------------------
