// $Id: RecordStream.cpp,v 1.1 2008/12/10 18:37:36 marcocle Exp $
#define LHCB_FSRSTREAM_CPP

// Framework include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "LHCbFSRStream.h"

// Define the algorithm factory for the standard output data writer
DECLARE_ALGORITHM_FACTORY(LHCbFSRStream)

// Standard Constructor
LHCbFSRStream::LHCbFSRStream(const std::string& name, ISvcLocator* pSvcLocator)
  : LHCbRecordStream(name, pSvcLocator),
    m_ioFsrSvc(0),
    m_doIOFsr(true)
{
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
  if (!m_doIOFsr) return LHCbRecordStream::finalize();
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
    
  //TODO cleanup the existing store, removing all empty directories
  
  return LHCbRecordStream::finalize();
  
}
