//$Id: DumpDetectorStore.cpp,v 1.4 2006-01-19 18:32:10 marcocle Exp $
#include <stdio.h>

#include "DumpDetectorStore.h"
#include "DetDataAgent.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h" 

/// Instantiation of a static factory to create instances of this algorithm
DECLARE_ALGORITHM_FACTORY( DumpDetectorStore );

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
  info() << "Finalize()" << endmsg;
  
  // Locate the IDataManagerSvc interface of the Detector Data Service
  IDataManagerSvc* detDataMgr;
  sc = detSvc()->queryInterface ( IID_IDataManagerSvc, (void **)&detDataMgr);
  if( sc.isFailure() ) {
    error() << "Can't query IDataManagerSvc interface" << endmsg;
    return sc;
  }

  // Traverse the transient data store and dump all contents
  DetDataAgent agent( msgSvc() );
  info() << "Now dump all contents of the data store" << endmsg;
  sc = detDataMgr->traverseTree( &agent );
  if ( sc.isSuccess() ) {
    info() << "Dumped all contents of the data store" << endmsg;
  } else {
    info() << "Nothing was found in the data store" << endmsg;
  }
  
  detDataMgr->release();
  
  return GaudiAlgorithm::finalize();
}

//----------------------------------------------------------------------------
