// $Id: RawDataSelector.cpp,v 1.23 2009-01-23 13:30:07 cattanem Exp $
//====================================================================
//  OnlineMDFEvtSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : MDF
//
//  Description: The LHCb::RawDataSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//  Author     : M.Frank
//====================================================================

// Include files
#include "MDF/RawDataAddress.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataSelector.h"
#include "MDF/RawDataConnection.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiUtils/IIODataManager.h"

enum { S_OK = StatusCode::SUCCESS, S_ERROR=StatusCode::FAILURE };

using namespace LHCb;

/// Standard constructor
RawDataSelector::LoopContext::LoopContext(const RawDataSelector* pSelector)
: m_sel(pSelector), m_fileOffset(0), m_ioMgr(m_sel->fileMgr()), m_connection(0)
{
}

/// Set connection
StatusCode RawDataSelector::LoopContext::connect(const std::string& specs)  {
  close();
  m_connection = new RawDataConnection(m_sel,specs);
  StatusCode sc = m_ioMgr->connectRead(true,m_connection);
  if ( sc.isSuccess() )  {
    m_conSpec = m_connection->fid();
    return sc;
  }
  close();
  return sc;
}

/// close connection
void RawDataSelector::LoopContext::close()    {
  if ( m_connection )  {
    m_ioMgr->disconnect(m_connection).ignore();
    delete m_connection;
    m_connection = 0;
  }
}

RawDataSelector::RawDataSelector(const std::string& nam, ISvcLocator* svcloc)
  : Service( nam, svcloc), m_rootCLID(CLID_NULL), m_evtCount(0)
{
  declareProperty("DataManager", m_ioMgrName="Gaudi::IODataManager/IODataManager");
  declareProperty("NSkip",       m_skipEvents=0);
  declareProperty("PrintFreq",   m_printFreq=-1);
  declareProperty("AddSpace",    m_addSpace=1);
  declareProperty("Input",       m_input="");
}

// IInterface::queryInterface
StatusCode RawDataSelector::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if (riid == IEvtSelector::interfaceID() )  {
    *ppvIf = (IEvtSelector*)this;
    addRef();
    return S_OK;
  }
  return Service::queryInterface(riid, ppvIf);
}

/// IService implementation: Db event selector override
StatusCode RawDataSelector::initialize()  {
  // Initialize base class
  StatusCode status = Service::initialize();
  MsgStream log(msgSvc(), name());
  if ( !status.isSuccess() )    {
    log << MSG::ERROR << "Error initializing base class Service!" << endreq;
    return status;
  }
  // Retrieve conversion service handling event iteration
  status = Service::service(m_ioMgrName, m_ioMgr);
  if( !status.isSuccess() ) {
    log << MSG::ERROR 
        << "Unable to localize interface IID_IIODataManager from service:" 
        << m_ioMgrName << endreq;
    return status;
  }
  // Get DataSvc
  IDataManagerSvc* eds = 0;
  status = serviceLocator()->service("EventDataSvc", eds);
  if( !status.isSuccess() ) {
    log << MSG::ERROR << "Cannot get IID_IDataManagerSvc from EventDataSvc" << endmsg;
    return status;
  }
  m_rootCLID = eds->rootCLID();
  eds->release();
  log << MSG::DEBUG << "Selection CLID:" << m_rootCLID << endmsg;
  m_evtCount = 0;
  return status;
}

/// IService implementation: finalize the service
StatusCode RawDataSelector::finalize()  {
  if ( m_ioMgr )  {
    m_ioMgr->release();
    m_ioMgr = 0;
  }
  m_evtCount = 0;
  return Service::finalize();
}

StatusCode RawDataSelector::next(Context& ctxt) const  {
  LoopContext* pCtxt = dynamic_cast<LoopContext*>(&ctxt);
  if ( pCtxt != 0 )   {
    ++m_evtCount;
    if ( m_printFreq>0 && (m_evtCount%m_printFreq)==0 ) {
      MsgStream log(messageService(), name());
      log << MSG::ALWAYS << "Reading Event record " << (m_evtCount-m_skipEvents)
          << ". Record number within stream " << m_evtCount << endreq;
    }
    return pCtxt->receiveData(msgSvc());
  }
  return S_ERROR;
}

StatusCode RawDataSelector::next(Context& ctxt, int jump) const {
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = next(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return S_OK;
  }
  return S_ERROR;
}

StatusCode RawDataSelector::previous(Context& /* ctxt */) const  {
  MsgStream log(msgSvc(), name());
  log << MSG::FATAL << " EventSelector Iterator, operator -- not supported " << endmsg;
  return S_ERROR;
}

StatusCode RawDataSelector::previous(Context& ctxt, int jump) const  {
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = previous(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return S_OK;
  }
  return S_ERROR;
}

StatusCode RawDataSelector::releaseContext(Context*& ctxt) const  {
  LoopContext* pCtxt = dynamic_cast<LoopContext*>(ctxt);
  if ( pCtxt ) {
    pCtxt->close();
    delete pCtxt;
    pCtxt = 0;
    return S_OK;
  }
  return S_ERROR;
}

StatusCode 
RawDataSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const   {
  const LoopContext* pctxt = dynamic_cast<const LoopContext*>(&ctxt);
  if ( pctxt ) {
    if ( pctxt->data().second>0 )  {
      RawDataAddress* pA = new RawDataAddress(RAWDATA_StorageType,m_rootCLID,pctxt->specs(),"0",0,0);
      pA->setType(RawDataAddress::DATA_TYPE);
      pA->setFileOffset(pctxt->offset());
      pA->setData(pctxt->data());
      pAddr = pA;
      return S_OK;
    }
  }
  pAddr = 0;
  return S_ERROR;
}

StatusCode 
RawDataSelector::resetCriteria(const std::string& criteria,Context& context) const    {
  MsgStream log(messageService(), name());
  LoopContext* ctxt = dynamic_cast<LoopContext*>(&context);
  if ( ctxt )  {
    std::string crit = criteria.substr(5);
    ctxt->close();
    StatusCode sc = ctxt->connect(crit);
    if ( !sc.isSuccess() )  {
      log << MSG::ERROR << "Failed to connect to:" << crit << endmsg;
    }
    else if ( m_skipEvents > 0 ) {
      sc = ctxt->skipEvents(messageService(),m_skipEvents);
    }
    return sc;
  }
  log << MSG::ERROR << "Invalid iteration context." << endmsg;
  return S_ERROR;
}
