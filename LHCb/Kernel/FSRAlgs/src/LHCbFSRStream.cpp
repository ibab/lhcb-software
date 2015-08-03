// $Id: RecordStream.cpp,v 1.1 2008/12/10 18:37:36 marcocle Exp $
#define LHCB_FSRSTREAM_CPP

// Framework include files
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LHCbFSRStream.h"

// Define the algorithm factory for the standard output data writer
DECLARE_ALGORITHM_FACTORY(LHCbFSRStream)

// Standard Constructor
LHCbFSRStream::LHCbFSRStream(const std::string& name, ISvcLocator* pSvcLocator)
  : LHCbRecordStream(name, pSvcLocator),
    m_ioFsrSvc(0),
    m_doIOFsr(true),
    m_cleanTree(true)
{
  declareProperty("FSRCleaningDirectory",m_cleanRoot="/FileRecords");
  declareProperty("CleanTree",m_cleanTree=true);
  declareProperty("AddIOFSR",m_doIOFsr=true);
}


StatusCode LHCbFSRStream::initialize() 
{
  MsgStream log(msgSvc(), name());
  StatusCode sc=LHCbRecordStream::initialize();
  if (!sc.isSuccess() ) return sc;
  if (!m_doIOFsr) return sc;
  
  SmartIF<IIOFSRSvc> ioFsrSvc(serviceLocator()->service<IIOFSRSvc>("IOFSRSvc"));
  if( !ioFsrSvc )  
  {
    log << MSG::ERROR << "Error retrieving IOFSRSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  m_ioFsrSvc=ioFsrSvc;
  
  return sc;
  
  
}


StatusCode LHCbFSRStream::finalize() 
{
  MsgStream log(msgSvc(), name());
  if (m_doIOFsr) 
  {
    
    //clean any existing top-level FSR
    StatusCode sc=m_ioFsrSvc->cleanTopFSR();
    //it probably isn't there...
    sc.ignore();
    
    
    //add the IOFSR to the TES
    sc=m_ioFsrSvc->storeIOFSR(m_outputName);
    if( !sc.isSuccess() )  
    {
      log << MSG::ERROR << "Error storing IOFSR." << endmsg;
    }
  }
  
  //TODO cleanup the existing store, removing all empty directories
  if (m_cleanTree)
  {
    // Try and load the root DataObject for the configured stream
    SmartDataPtr<DataObject> root( m_pDataProvider, m_cleanRoot );
    
    // if found, recursively clean
    if ( root ) cleanNodes( root, m_cleanRoot );
  }
  
  return LHCbRecordStream::finalize();
  
}

void LHCbFSRStream::cleanNodes( DataObject * obj,
                                const std::string & location,
                                unsigned int nRecCount )
{
  MsgStream log(msgSvc(), name());
  // protect against infinite recursion
  if ( ++nRecCount > 99999 )
  {
    log << MSG::ERROR << "Maximum recursion limit reached...." << endmsg;
    return;
  }
  
  SmartIF<IDataManagerSvc> mgr( m_pDataManager );
  
  typedef std::vector<IRegistry*> Leaves;
  Leaves leaves;
  
  // Load the leaves
  StatusCode sc = mgr->objectLeaves( obj, leaves );
  if ( sc )
  {
    
    if ( !leaves.empty() )
    {
      for ( Leaves::const_iterator iL = leaves.begin(); leaves.end() != iL; ++iL )
      {
        const std::string & id = (*iL)->identifier();
        DataObject* tmp(NULL);
        sc = m_pDataProvider->findObject( id, tmp );
        if ( sc && tmp )
        {
          if ( CLID_DataObject == tmp->clID() )
          {
            cleanNodes( tmp, id, nRecCount );
          }
        }
      }
      // Load again, after cleaning, to see what remains
      sc = mgr->objectLeaves( obj, leaves );
    }

    if ( sc && leaves.empty() )
    {
      log << MSG::DEBUG << "Removing node " << location << endmsg;
      sc = m_pDataProvider->unlinkObject(location);
    }

  }

}
