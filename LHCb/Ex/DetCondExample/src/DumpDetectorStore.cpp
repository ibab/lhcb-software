//$Id: DumpDetectorStore.cpp,v 1.7 2009-01-27 16:09:16 cattanem Exp $
#include <stdio.h>

#include "DumpDetectorStore.h"
#include "DetDataAgent.h"

#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartIF.h"

/// Instantiation of a static factory to create instances of this algorithm
DECLARE_ALGORITHM_FACTORY( DumpDetectorStore )

//----------------------------------------------------------------------------

/// Constructor
DumpDetectorStore::DumpDetectorStore( const std::string&  name, ISvcLocator* pSvcLocator )
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("DumpConditions", m_dumpConds = false );
}

//----------------------------------------------------------------------------

StatusCode DumpDetectorStore::execute(){ return StatusCode::SUCCESS; }

//----------------------------------------------------------------------------

StatusCode DumpDetectorStore::finalize( ) {

  StatusCode sc;
  info() << "Finalize()" << endmsg;
  
  // Locate the IDataManagerSvc interface of the Detector Data Service
  SmartIF<IDataManagerSvc> detDataMgr( detSvc() );
  if( !detDataMgr ) {
    return Error( "Can't query IDataManagerSvc interface", sc );
  }

  // Traverse the transient data store and dump all contents
  DetDataAgent agent( msgSvc(), m_dumpConds );
  info() << "Now dump all contents of the data store" << endmsg;
  sc = detDataMgr->traverseTree( &agent );
  if ( sc.isSuccess() ) {
    info() << "Dumped all contents of the data store" << endmsg;
  } else {
    info() << "Nothing was found in the data store" << endmsg;
  }
  
  return GaudiAlgorithm::finalize();
}

//----------------------------------------------------------------------------
