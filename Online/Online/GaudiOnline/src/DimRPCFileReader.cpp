// $Id: DimRPCFileReader.cpp,v 1.26 2009-12-14 10:49:54 frankb Exp $
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiOnline/DimRPCFileReader.h"
#include "GaudiOnline/MEPManager.h"
#include "RTL/rtl.h"

#define NOMSG
#define WIN32_LEAN_AND_MEAN
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
: OnlineService(nam, svcLoc), m_mepMgr(0), m_dataSvc(0), m_evtSelector(0), m_evtLoopMgr(0),
  m_receiveEvts(false), m_evtCount(0), m_rpc(0,0)
{
  ::lib_rtl_create_lock(0,&m_lock);
  declareProperty("Incident",     m_incidentName="NEW_INPUT_FID");
}

/// Standard Destructor
DimRPCFileReader::~DimRPCFileReader()  {
  ::lib_rtl_delete_lock(m_lock);
}

/// IInterface implementation : queryInterface
StatusCode DimRPCFileReader::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( riid == IRunable::interfaceID() ) {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

/// IService implementation: initialize the service
StatusCode DimRPCFileReader::initialize()   {
  StatusCode sc;
  if ( !(sc=OnlineService::initialize()).isSuccess() )
    return error("Failed to initialize service base class.");
  else if ( !(sc=service("MEPManager/MEPManager",m_mepMgr)).isSuccess() )
    return error("Failed to access MEP manager service.");
  else if( !(sc=service("EventDataSvc",m_dataSvc,true)).isSuccess() )
    return error("Error retrieving EventDataSvc interface IDataProviderSvc.");
  else if( !(sc=service("EventSelector",m_evtSelector,true)).isSuccess() )
    return error("Error retrieving EventSelector interface IEvtSelector.");
  else if( !(sc=service("EventLoopMgr",m_evtLoopMgr,true)).isSuccess() )
    return error("Error retrieving EventLoopManager interface IEvtProcessor.");

  incidentSvc()->addListener(this,"DAQ_CANCEL");
  declareInfo("EvtCount",m_evtCount=0,"Number of events processed");

  m_command = new Command();
  std::stringstream outstream;
  outstream << "ds6:statusi1es7:commands4:idles6:paramsdee";
  outstream >> m_idle;
  m_reply = m_idle;

  // Init the extra Services
  std::string svcName=RTL::processName()+"/RpcFileDBOut";
  std::string cmdName=RTL::processName()+"/RpcFileDBIn";
  m_rpc.first  = ::dis_add_service((char*)svcName.c_str(),(char*)"C",0,0,publishEvents,(long)this);
  m_rpc.second = ::dis_add_cmnd((char*)cmdName.c_str(),(char*)"C",cmndCallback,(long)this);
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

StatusCode DimRPCFileReader::sysStart()   {
  std::string svcName=RTL::processName()+"/RpcFileDBOut";
  std::string cmdName=RTL::processName()+"/RpcFileDBIn";
  if ( 0 == m_rpc.first )
    m_rpc.first  = ::dis_add_service((char*)svcName.c_str(),(char*)"C",0,0,publishEvents,(long)this);
  if ( 0 == m_rpc.second )
    m_rpc.second = ::dis_add_cmnd((char*)cmdName.c_str(),(char*)"C",cmndCallback,(long)this);
  return StatusCode::SUCCESS;
}

/// IService implementation: finalize the service
StatusCode DimRPCFileReader::sysStop()     {
  if ( m_rpc.first ) {
    ::dis_remove_service(m_rpc.first);
    m_rpc.first = 0;
  }
  if ( m_rpc.second ) {
    ::dis_remove_service(m_rpc.second);
    m_rpc.second = 0;
  }
  incidentSvc()->removeListener(this);
  return StatusCode::SUCCESS;
}

void DimRPCFileReader::publishEvents(void* tag, void** buf, int* size, int* first)    {
  DimRPCFileReader* self=*(DimRPCFileReader**) tag;
  std::string result=self->m_reply;
  if (*first) {
    //result="ds6:statusi1es6:paramsdes7:commands4:idlee";
  }
  self->info("Publishing work information:%s",result.c_str());
  self->info(result);
  *size = result.length()+1;
  *buf = (void*)result.c_str();
  return;
}
bool s_isProcessing = false;

void DimRPCFileReader::handleCommand(const char* address, int /* size */){
  ::time(&m_timerStartPrep);
  std::string in = address;

  if ( s_isProcessing ) {
    error("File is processing and STILL got new proc request:"+in);
  }
  int sc = m_command->decodeCommand(in);
  if (!sc) {
    info("Error decoding "+in);
    return;
  }
  char infostring[200];
  const Command::Data& c = m_command->data;
  ::sprintf(infostring,"%s command: parameters: Filename: %s FID: %i", 
	    c.name.c_str(), c.file.c_str(), c.fileID);
  info(infostring);
  SmartIF<IProperty> prp(m_evtSelector);
  if ( prp ) {
    time(&m_timerStopPrep);
    time(&m_timerStartProc);
    SmartIF<IService> es(m_evtLoopMgr);
    SmartIF<IService> am(serviceLocator());
    am->stop();
    prp->setProperty("Input","FILE=file://"+c.file);
    prp->setProperty("GUID",c.guid);
    error("Input: FILE=file://"+c.file);
    error("GUID:"+c.guid);

    es->reinitialize();
    am->start();
    m_fileID = c.fileID;
    incidentSvc()->fireIncident(ContextIncident<int>(c.guid,m_incidentName,c.fileID));
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
    ::lib_rtl_unlock(m_lock);
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
      if ( !m_receiveEvts ) {
        info("Quitting...");
        // m_reply = "ds7:commands4:quits6:paramsdee"
        // ::dis_update_service(m_rpc.first);
        break;
      }
      s_isProcessing = true;
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
      m_reply = m_command->encodeResponse(2,m_evtCount);
      ::dis_update_service(m_rpc.first);      
      s_isProcessing = false;
      m_reply = m_idle;
      m_evtCount = 0;
      info("Preparation took %5.2f seconds",difftime(m_timerStopPrep,m_timerStartPrep));
      info("Processing  took %5.2f seconds",difftime(m_timerStopProc,m_timerStartProc));
      ::dis_update_service(m_rpc.first);
    }
    return StatusCode::SUCCESS;
  }
  return error("Failed to access Application manager UI.");
}
