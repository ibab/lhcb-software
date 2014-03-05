//====================================================================
//  TestEvtSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : MDF
//
//  Description: The LHCb::TestEvtSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//  Author     : M.Frank
//====================================================================

// Include files
#include "MDF/RawDataAddress.h"
#include "MDF/RawEventHelpers.h"
#include "OnlineFileSelector/TestEvtSelector.h"
#include "MDF/RawDataConnection.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiUtils/IIODataManager.h"


enum { S_OK = StatusCode::SUCCESS, S_ERROR=StatusCode::FAILURE };

using namespace std;
using namespace LHCb;

/// Standard constructor
TestEvtSelector::LoopContext::LoopContext(const TestEvtSelector* pSelector)
  : m_sel(pSelector), m_fileOffset(0), m_ioMgr(m_sel->fileMgr()), m_connection(0),
    m_trgMask(0), m_vetoMask(0)
{
}

/// Set connection
StatusCode TestEvtSelector::LoopContext::connect(const std::string& specs)  {
  close();
  m_connection = new RawDataConnection(m_sel,specs);
  StatusCode sc = m_ioMgr->connectRead(true,m_connection);
  if ( sc.isSuccess() )  {
    m_conSpec  = m_connection->fid();
    m_vetoMask = (m_sel->vetoMask().empty()) ? 0 : &(m_sel->vetoMask());
    m_trgMask  = (m_sel->triggerMask().empty()) ? 0 : &(m_sel->triggerMask());
    return sc;
  }
  close();
  return sc;
}

/// close connection
void TestEvtSelector::LoopContext::close()    {
  if ( m_connection )  {
    m_ioMgr->disconnect(m_connection).ignore();
    delete m_connection;
    m_connection = 0;
  }
}

TestEvtSelector::TestEvtSelector(const std::string& nam, ISvcLocator* svcloc)
  : Service( nam, svcloc), m_rootCLID(CLID_NULL), m_ioMgr(0), m_evtCount(0)
{
  declareProperty("DataManager", m_ioMgrName="Gaudi::IODataManager/IODataManager");
  declareProperty("TriggerMask", m_trgMask);
  declareProperty("VetoMask",    m_vetoMask);
  declareProperty("NSkip",       m_skipEvents=0);
  declareProperty("PrintFreq",   m_printFreq=-1);
  declareProperty("AddSpace",    m_addSpace=1);
  declareProperty("Input",       m_input="");
  declareProperty("MaxNoEvents", m_maxNoEvt);
}

// IInterface::queryInterface
StatusCode TestEvtSelector::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if (IEvtSelector::interfaceID().versionMatch(riid) )  {
    *ppvIf = (IEvtSelector*)this;
    addRef();
    return S_OK;
  }
  else if(IAlertSvc::interfaceID().versionMatch(riid)) {
    *ppvIf = (IAlertSvc*)this;
    addRef();
    return S_OK;
  }
  else if ( ISuspendable::interfaceID().versionMatch(riid) ) {
    *ppvIf = (ISuspendable*)this;
    addRef();
    return S_OK;
  }
  else
    return Service::queryInterface(riid, ppvIf);
}

/// IService implementation: Db event selector override
StatusCode TestEvtSelector::initialize()  {
  // Initialize base class
  StatusCode status = Service::initialize();
  MsgStream log(msgSvc(), name());
  setSvcName(Service::name());
  if ( !status.isSuccess() )    {
    log << MSG::ERROR << "<TestEvtSelector> Error initializing base class Service!" << endmsg;
    return status;
  }

  

   StatusCode sc = Service::service("LHCb::FilePoller/Poller",m_filePoller,true);
   if ( !sc.isSuccess() )  {
     log << MSG::ERROR << "Cannot access poller service." << endmsg;
     return sc;
   }

  // Locks
  // Create lock to steer suspend/resume operations
  if ( !lib_rtl_is_success(lib_rtl_create_event(0,&m_suspendLock)) )  {
    log << MSG::ERROR << "Cannot create lock to suspend operations." << endmsg;
    
    return StatusCode::FAILURE;
  }
  //Set initial event state in case selector works standalone --  using suspend/resume
  if ( !lib_rtl_is_success(lib_rtl_clear_event(m_suspendLock)) )  {
    log << MSG::ERROR << "Cannot activate event lock."<< endmsg;
    
    return StatusCode::FAILURE;
  }
  
  
  // Retrieve conversion service handling event iteration
  status = Service::service(m_ioMgrName, m_ioMgr);
  if( !status.isSuccess() ) {
    log << MSG::ERROR 
        << "Unable to localize interface IID_IIODataManager from service:" 
        << m_ioMgrName << endmsg;
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
  if( UNLIKELY(log.level() <= MSG::DEBUG) )
    log << MSG::DEBUG << "Selection CLID:" << m_rootCLID << endmsg;
  if ( !m_trgMask.empty() ) {
    log << MSG::INFO << "Trigger mask: " << std::hex;
    for(size_t i=0; i<m_trgMask.size(); ++i) 
      log << "0x" << std::setw(8) << std::setfill('0') << m_trgMask[i] << " ";
    log << endmsg;
  }
  if ( !m_vetoMask.empty() ) {
    log << MSG::INFO << "Trigger mask: " << std::hex;
    for(size_t i=0; i<m_vetoMask.size(); ++i) 
      log << "0x" << std::setw(8) << std::setfill('0') << m_vetoMask[i] << " ";
    log << endmsg;
  }
  m_evtCount = 0;

  return status;
}

/// IService implementation: finalize the service
StatusCode TestEvtSelector::finalize()  {
  if ( m_ioMgr )  {
    m_ioMgr->release();
    m_ioMgr = 0;
  }
  m_evtCount = 0;

  if ( m_filePoller )  { 
    m_filePoller->remListener(this); 
    m_filePoller = 0;
  }
  
  // Release lock to steer suspend/resume operations
  ::lib_rtl_delete_event(m_suspendLock);
  m_suspendLock = 0;
  
  return Service::finalize();
}


/// Added locking mechanism in next method.

StatusCode TestEvtSelector::next(Context& ctxt) const  {
  MsgStream log(messageService(), name());
  LoopContext* pCtxt = dynamic_cast<LoopContext*>(&ctxt);
  StatusCode sc = StatusCode::FAILURE;
  
  if (m_maxNoEvt == m_evtCount) { 
		      m_evtCount = 1; ///Read as much as we wanted, read the same number from next file -- first event is immediately read
	        cout << "Going idle..." << endl;
          return goIdle();
  } 

  if ( pCtxt != 0 )   {

           if  (0 != TestEvtSelector::m_firstConnection) {
               sc = pCtxt->receiveData(msgSvc());  
           } 
           
           if (sc.isSuccess())
           {
             ++m_evtCount;
             if ( m_printFreq>0 && (m_evtCount%m_printFreq)==0 ) {
               MsgStream log(messageService(), name());
               log << MSG::ALWAYS << "Reading Event record " << (m_evtCount-m_skipEvents)
                  << ". Record number within stream " << m_evtCount << endmsg;
             }
             return sc;
           }
           else {

             return goIdle();

           }
           
  }
  return S_ERROR;
}


StatusCode TestEvtSelector::goIdle() const {

  MsgStream log(msgSvc(), name());
  m_isWaiting = true;
  m_filePoller->addListener((IAlertSvc*)this); //Why have to force the cast???

  if (!lib_rtl_is_success(lib_rtl_wait_for_event(m_suspendLock))) {
    log << MSG::ERROR << "Cannot lock to suspend operations." << endmsg;
    if ( m_filePoller ) {
      m_filePoller->remListener((IAlertSvc*)this);
    }
  }
  m_isWaiting = false;

  return StatusCode::SUCCESS;

}



StatusCode TestEvtSelector::next(Context& ctxt, int jump) const {
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

StatusCode TestEvtSelector::previous(Context& /* ctxt */) const  {
  MsgStream log(msgSvc(), name());
  log << MSG::FATAL << " EventSelector Iterator, operator -- not supported " << endmsg;
  return S_ERROR;
}

StatusCode TestEvtSelector::previous(Context& ctxt, int jump) const  {
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

StatusCode TestEvtSelector::releaseContext(Context*& ctxt) const  {
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
TestEvtSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const   {
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
TestEvtSelector::resetCriteria(const std::string& criteria,Context& context) const    {
  MsgStream log(messageService(), name());
  LoopContext* ctxt = dynamic_cast<LoopContext*>(&context);
  if ( ctxt )  {
    std::string crit = criteria;//std::string crit = criteria.substr(5);
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


// ISuspendable implementation: suspend operation
StatusCode TestEvtSelector::suspend() const  {
  MsgStream log(messageService(), name());
  if ( !lib_rtl_is_success(lib_rtl_clear_event(m_suspendLock)) )  {
    log << MSG::ERROR << "Cannot lock to suspend operations." << endmsg;
    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ISuspendable implementation: resume operation
StatusCode TestEvtSelector::resume() const  {
    MsgStream log(messageService(), name());
    if ( !lib_rtl_is_success(lib_rtl_set_event(m_suspendLock)) )  {
      log << MSG::ERROR << "Cannot unlock to resume operations." << endmsg;
      
      return StatusCode::FAILURE;
      
    }
  
  return StatusCode::SUCCESS;
}



StatusCode TestEvtSelector::alertSvc(const string& file_name) 
{
  
  MsgStream log(messageService(), name());
  TestEvtSelector::m_firstConnection = 1;

  //Wake listener up,take care of any lock/synchronisation issues.
  if ( !file_name.empty() )  {
        m_input = file_name;
        Context* refpCtxt = getCurContext();
        StatusCode sc = resetCriteria(m_input,*refpCtxt);
        if ( !sc.isSuccess() ) {
          delete refpCtxt;
          refpCtxt = 0;
          return sc;
        }
        m_filePoller->statusReport(sc,m_input);
   }

  if (!(m_filePoller->remListener((IAlertSvc*)this))) {
      log << MSG::ERROR << "Error removing listener from waiting list." << endmsg;
      return StatusCode::FAILURE;
      }

  cout << "EventSelector:Ready to read events from " + file_name << endl;
  
  TestEvtSelector::resume();
  
  return StatusCode::SUCCESS;
}


void  TestEvtSelector::setSvcName(const string& name)
{
  m_listener_name = name;
}


string  TestEvtSelector::getSvcName()
{
  return m_listener_name;
  
}
      

