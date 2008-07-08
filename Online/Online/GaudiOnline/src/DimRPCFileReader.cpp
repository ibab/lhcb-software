// $Id: DimRPCFileReader.cpp,v 1.10 2008-07-08 13:24:06 frankb Exp $
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiOnline/DimRPCFileReader.h"
#include "GaudiOnline/MEPManager.h"
#include "OnlineKernel/RTL/rtl.h"

#define _WINDOWS_
#define _WINSOCKAPI_
extern "C" {
#include "dim.h"
#include "dis.h"
}

// Instantiation of a static factory class used by clients to create instances of this service
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DimRPCFileReader);

using namespace LHCb;

/// Static DIM command callback
void DimRPCFileReader::cmndCallback(void* tag, void* address, int* size){
  DimRPCFileReader* self=*(DimRPCFileReader**) tag;
  self->handleCommand((const char*)address,*size);
}

/// Standard Constructor
DimRPCFileReader::DimRPCFileReader(const std::string& nam, ISvcLocator* svcLoc)   
  : OnlineService(nam, svcLoc), m_mepMgr(0), m_dataSvc(0), 
    m_evtSelector(0), m_evtLoopMgr(0),
    m_receiveEvts(false), m_evtCount(0), m_rpc(0,0)
{
  ::lib_rtl_create_lock(0,&m_lock);
}

/// Standard Destructor
DimRPCFileReader::~DimRPCFileReader()  {
  ::lib_rtl_delete_lock(m_lock);
}

/// IInterface implementation : queryInterface
StatusCode DimRPCFileReader::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( riid == IID_IRunable ) {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

/// IService implementation: initialize the service
StatusCode DimRPCFileReader::initialize()   {
  StatusCode sc = OnlineService::initialize();

  if ( !sc.isSuccess() )     {
    return error("Failed to initialize service base class.");
  }
  if ( !(sc=service("MEPManager/MEPManager",m_mepMgr)).isSuccess() )  {
    return error("Failed to access MEP manager service.");
  }
  if( !(sc=service("EventDataSvc",m_dataSvc,true)).isSuccess() )  {
    return error("Error retrieving EventDataSvc interface IDataProviderSvc.");
  }
  if( !(sc=service("EventSelector",m_evtSelector,true)).isSuccess() )  {
    return error("Error retrieving EventSelector interface IEvtSelector.");
  }
  if( !(sc=service("EventLoopMgr",m_evtLoopMgr,true)).isSuccess() )  {
    return error("Error retrieving EventLoopManager interface IEvtProcessor.");
  }
  incidentSvc()->addListener(this,"DAQ_CANCEL");
  declareInfo("EvtCount",m_evtCount=0,"Number of events processed");

  m_command = new Command();
  // m_reply = "ds6:statusi1es6:paramsdes7:commands4:idlee";
  
  // Init the extra Services
  std::string svcName=RTL::processName()+"/RpcOut";
  std::string cmdName=RTL::processName()+"/RpcIn";
  m_rpc.first  = ::dis_add_service((char*) svcName.c_str(),"C",0,0,publishEvents,(long) this);
  m_rpc.second = ::dis_add_cmnd((char*) cmdName.c_str(),"C",cmndCallback,(long) this);
  ::lib_rtl_lock(m_lock);
  return sc;
}

/// IService implementation: finalize the service
StatusCode DimRPCFileReader::finalize()     {
  if ( m_rpc.first ) {
    ::dis_remove_service(m_rpc.first);
    m_rpc.first = 0;
  }
  if ( m_rpc.second ) {
    ::dis_remove_service(m_rpc.second);
    m_rpc.second = 0;
  }
  releaseInterface(m_evtSelector);
  releaseInterface(m_evtLoopMgr);
  releaseInterface(m_dataSvc);
  releaseInterface(m_mepMgr);
  return OnlineService::finalize();
}

void DimRPCFileReader::publishEvents(void* tag, void** buf, int* size, int* first)    {
  DimRPCFileReader* self=*(DimRPCFileReader**) tag;
  std::string result=self->m_reply;
  if (*first) {
    result="ds6:statusi1es6:paramsdes7:commands4:idlee";
  }
  self->info(result);
  *size = result.length()+1;
  *buf = (void*)result.c_str();
  return;
}

void DimRPCFileReader::handleCommand(const char* address, int /* size */){
  time(&m_timerStartPrep);
  std::string in = address;
  int sc=m_command->decodeCommand(in);
  if (!sc){
    info("Error decoding "+in);
    return;
  }
  char infostring[200];
  sprintf(infostring,"%s command: parameters: Filename: %s FID: %i", (m_command->data).name.c_str(), (m_command->data).file.c_str(), (m_command->data).fileID);
  info(infostring);
  SmartIF<IProperty> prp(m_evtSelector);
  if ( prp ) {
    prp->setProperty("Input","[ \"DATA='file://"+m_command->data.file+"' SVC='LHCb::MDFSelector'\" ]");
    prp->setProperty("PrintFreq","1");
    time(&m_timerStopPrep);
    time(&m_timerStartProc);    
    m_evtSelector->reinitialize();
    m_evtLoopMgr->reinitialize();
    m_fileID=m_command->data.fileID;
    ::lib_rtl_unlock(m_lock);
    return;
  }
}

/// Incident handler implemenentation: Inform that a new incident has occured
void DimRPCFileReader::handle(const Incident& inc)    {
  info("Got incident:"+inc.source()+" of type "+inc.type());
  if ( inc.type() == "DAQ_CANCEL" )  {
    m_receiveEvts = false;
    if ( m_mepMgr ) {
      m_mepMgr->cancel();
    }
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
    m_receiveEvts = true;
  }
}

/// IRunable implementation : Run the class implementation
StatusCode DimRPCFileReader::run()   {  
  SmartIF<IAppMgrUI> ui(serviceLocator());
  if ( ui )    {
    while(1) {
      m_receiveEvts = true;
      info ("Starting loop through events");
      // ::dis_update_service(m_rpc.first);
      info("Locking event loop. Waiting for work....");
      ::lib_rtl_lock(m_lock);
      m_evtCount = 0;
      m_reply=m_command->encodeResponse(1);
      ::dis_update_service(m_rpc.first);
      // loop over the events
      while ( m_receiveEvts )   {
        DataObject* pObj = 0;
        StatusCode sc = ui->nextEvent(1);
        if ( !sc.isSuccess() )  {
          error("Failed to process event.");
          break;
        }
        if ( !m_dataSvc->findObject("/Event",pObj).isSuccess() )  {
          info("End of event input reached.");
          break;
        }
        m_evtCount++;
      }
      time(&m_timerStopProc);
      m_reply=m_command->encodeResponse(2,m_evtCount);
      ::dis_update_service(m_rpc.first);      
      m_reply = "ds6:statusi1es6:paramsdes7:commands4:idlee";
      m_evtCount = 0;
      std::cout << "Prep took " << difftime (m_timerStopPrep,m_timerStartPrep) << " seconds" << std::endl;
      std::cout << "Proc took " << difftime (m_timerStopProc,m_timerStartProc) << " seconds" << std::endl;
      ::dis_update_service(m_rpc.first);
    }
    return StatusCode::SUCCESS;
  }
  return error("Failed to access Application manager UI.");
}
