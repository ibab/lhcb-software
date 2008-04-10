// $Id: DimRPCFileReader.cpp,v 1.1 2008-04-10 10:06:26 frankb Exp $
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiOnline/DimRPCFileReader.h"
#include "GaudiOnline/MEPManager.h"
#include "OnlineKernel/RTL/rtl.h"
#include "OnlineKernel/RTL/Lock.h"
#include <cstdlib>
extern "C" {
#include "dim.h"
#include "dis.h"
}

using namespace LHCb;

// Instantiation of a static factory class used by clients to create instances of this service
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DimRPCFileReader);

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
  lib_rtl_create_lock(0,&m_lockId);
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

  // Init the extra Services
  m_rpc.first  = ::dis_add_service("RpcOut","C",0,0,publishEvents,(long)this);
  m_rpc.second = ::dis_add_cmnd("RpcIn","C",cmndCallback,(long)this);
  ::lib_rtl_lock(m_lockId);
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
  DimRPCFileReader* self=(DimRPCFileReader*) tag;
  std::cout << "Publish events" << std::endl;
  static const char* empty = "";
  if ( !(*first) )  {
    std::stringstream s;
    s << "newFile/status=SUCCESS/" << self->m_reply << "/" << self->m_evtCount << std::ends;
    self->m_reply = s.str();
    std::cout << "Sending: " << self->m_reply << std::endl;
    *size = self->m_reply.length()+1;
    *buf = (void*)self->m_reply.c_str();
    return;
  }
  std::cout << "first" << std::endl;
  *size = 0;
  *buf  = (void*)empty;
}

void DimRPCFileReader::handleCommand(const char* address, int /* size */){
  std::string in = address, cmd, file, fid, left;
  size_t pos = in.find("/");
  if ( pos != std::string::npos ){
    cmd = in.substr(0,pos);
    if( cmd == "newFile" )  {
      left = in.substr(pos+1);
      std::cout << "Command received: " << cmd << " " << left << std::endl;
      if ( (pos=left.find("/")) != std::string::npos ) {
	file = left.substr(pos+1);
	fid  = left.substr(0,pos);      
	std::cout << "Parameters. Filename: " << file << "   FID: " << fid << std::endl;
	SmartIF<IProperty> prp(m_evtSelector);
	if ( prp ) {
	  prp->setProperty("Input","[ \"DATA='file://"+file+"' SVC='LHCb::MDFSelector'\" ]");
	  prp->setProperty("PrintFreq","1");
	  m_evtSelector->reinitialize();
	  m_evtLoopMgr->reinitialize();
	  m_reply = fid+"/"+RTL::processName()+"/";
	  ::lib_rtl_unlock(m_lockId);
	  return;
	}
      }
    }
  }
  std::cout << "Unknown command " << in << std::endl;
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
      ::dis_update_service(m_rpc.first);
      std::cout << "Locking event loop. Waiting for work...." << std::endl;
      ::lib_rtl_lock(m_lockId);
      m_evtCount = 0;
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
      ::dis_update_service(m_rpc.first);
      m_reply = "";
      m_evtCount = 0;
    }
    return StatusCode::SUCCESS;
  }
  return error("Failed to access Application manager UI.");
}

