#if 0
// $Id: DimRPCFileReader.cpp,v 1.8 2008-06-05 19:04:55 niko Exp $
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiOnline/DimRPCFileReader.h"
#include "GaudiOnline/MEPManager.h"
#include "OnlineKernel/RTL/rtl.h"

extern "C" {
#include "Python.h"
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
  
  // Init the extra Services
  m_rpc.first  = ::dis_add_service("RpcOut","C",0,0,publishEvents,(long) this);
  m_rpc.second = ::dis_add_cmnd("RpcIn","C",cmndCallback,(long) this);
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

  PyObject *pName, *pModule, *pEncode, *pValue; 
  PyObject *pMsg, *pDict, *pArgs;
  std::string result;
  Py_Initialize();
  pName = PyString_FromString("DEncode");
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);
  
  if (pModule != NULL ) {
    pEncode = PyObject_GetAttrString(pModule, "encode");
    if (pEncode && PyCallable_Check(pEncode)) {
      pArgs = PyTuple_New(1);
      pDict= PyDict_New();
      pMsg = PyList_New(3);
      // PyDict_SetItem(pDict,PyString_FromString("clientName"),PyString_FromString(RTL::processName().c_str()));
      // PyDict_SetItem(pDict,PyString_FromString("clientType"),PyString_FromString("Reader"));      
      if (!(self->m_evtCount)){
        PyList_SetItem(pMsg,0,PyString_FromString("goIdle"));
        PyList_SetItem(pMsg,1,PyInt_FromLong(1));
        PyList_SetItem(pMsg,2,pDict); 
      } else {
        PyDict_SetItem(pDict,PyString_FromString("nEvts"),PyInt_FromLong(self->m_evtCount));
        PyDict_SetItem(pDict,PyString_FromString("fileID"),PyInt_FromLong(self->m_fileID));
        PyList_SetItem(pMsg,0,PyString_FromString("newFileToRead"));
        PyList_SetItem(pMsg,1,PyInt_FromLong(1));
        PyList_SetItem(pMsg,2,pDict);
      }
      PyTuple_SetItem(pArgs,0,pMsg);
      pValue = PyObject_CallObject(pEncode, pArgs);
      Py_DECREF(pMsg);
      Py_DECREF(pDict);
      Py_DECREF(pArgs);
      result="";
      if (pValue != NULL) {
        result = PyString_AsString(pValue);
        Py_DECREF(pValue);
      } else {
        self->error("Call failed\n");
      }
    }
    Py_XDECREF(pEncode);
    Py_DECREF(pModule);
  }
  Py_Finalize();
  self->info(result);
  *size = result.length()+1;
  *buf = (void*)result.c_str();
  return;
}

// void DimRPCFileReader::publishEvents(void* tag, void** buf, int* size, int* first)    {
//   DimRPCFileReader* self=*(DimRPCFileReader**) tag;
//  std::stringstream s;
//  //   static const char* empty = "newFileToRead/status=SUCCESS/000000-0000-0000-00000000/None";
//  //   if ( !(*first) )  {
//  //  std::stringstream s;
//  //     s << "newFileToRead/status=SUCCESS/" << self->m_reply << self->m_evtCount << std::ends;
//  //     self->m_reply = s.str();
//  //     self->info(self->m_reply);
//  //     *size = self->m_reply.length()+1;
//  //     *buf = (void*)self->m_reply.c_str();
//  //     return;
//  //   }
//  //   *size = ::strlen(empty);
//  //   *buf  = (void*)empty;
//  if (!(self->m_evtCount)){
//    s << "goIdle/status=SUCCESS/clientName:" << RTL::processName() << "/clientType:Reader" << std::ends;
//  } else {
//    s << "newFileToRead/status=SUCCESS/" << self->m_reply << "nEvts:"<<self->m_evtCount << std::ends;
//  }
//  self->m_reply = s.str();
//   self->info(self->m_reply);
//   *size = self->m_reply.length()+1;
//   *buf = (void*)self->m_reply.c_str();
//   return;
// }

// void DimRPCFileReader::handleCommand(const char* address, int /* size */){
//   std::string in = address, cmd, file, fid, left;
//   size_t pos = in.find("/");
//   if ( pos != std::string::npos ){
//     cmd = in.substr(0,pos);
//     if( cmd == "newFileToRead" )  {
//       left = in.substr(pos+1);
//       if ( (pos=left.find("/")) != std::string::npos ) {
//        file = left.substr(pos+1);
//        fid  = left.substr(0,pos);
//        info("newFileToRead command: parameters: Filename:"+file+" FID: "+fid);
//        SmartIF<IProperty> prp(m_evtSelector);
//        if ( prp ) {
//          prp->setProperty("Input","[ \"DATA='file://"+file+"' SVC='LHCb::MDFSelector'\" ]");
//          prp->setProperty("PrintFreq","1");
//          m_evtSelector->reinitialize();
//          m_evtLoopMgr->reinitialize();
//          m_fileID=atoi(fid.c_str());
//          m_reply = "fileID:"+fid+"/clientName:"+RTL::processName()+"/clientType:Reader/";
//          ::lib_rtl_unlock(m_lock);
//          return;
//        }
//       }
//     }
//   }
//   error("Unknown command:"+in);
// }

void DimRPCFileReader::handleCommand(const char* address, int /* size */){
  PyObject *pName, *pModule, *pDecode, *pValue; 
  PyObject *pInput, *pLength, *pArgs, *pDict;
  std::string in = address;
  std::string cmd, file;
  std::stringstream fileID;

  Py_Initialize();
  pName = PyString_FromString("DEncode");
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule != NULL ) {
    pDecode = PyObject_GetAttrString(pModule, "decode");
    if (pDecode && PyCallable_Check(pDecode)) {
      pArgs = PyTuple_New(1);
      PyTuple_SetItem(pArgs,0,PyString_FromString(in.c_str()));
      pValue = PyObject_CallObject(pDecode, pArgs);
      if (pValue != NULL) {
        pInput=PyTuple_GetItem(pValue,0);
        pLength=PyTuple_GetItem(pValue,1);
        if (PyInt_AsLong(pLength)==in.size()) {
          cmd=PyString_AsString(PyList_GetItem(pInput,0));
          pDict=PyList_GetItem(pInput,1);
        } else {
          error("Wrong message length!"); return;
        }
        Py_DECREF(pValue);
        Py_DECREF(pInput);
        Py_DECREF(pLength);
      } else {
        error("Decoding failed\n");
      }
      Py_DECREF(pArgs); 
      Py_DECREF(pDecode); 
    }
    Py_DECREF(pModule); 
  }
  
  if( cmd == "newFileToRead" )  {
    file=PyString_AsString(PyDict_GetItemString(pDict,"file"));
    m_fileID=PyLong_AsLong(PyDict_GetItemString(pDict,"fileID"));
    char fileID[128];
    sprintf(fileID,"%d",m_fileID);
    info("newFileToRead command: parameters: Filename:"+file+" FID: "+fileID);
    Py_Finalize();
    SmartIF<IProperty> prp(m_evtSelector);
    if ( prp ) {
      prp->setProperty("Input","[ \"DATA='file://"+file+"' SVC='LHCb::MDFSelector'\" ]");
      prp->setProperty("PrintFreq","1");
      m_evtSelector->reinitialize();
      m_evtLoopMgr->reinitialize();
      ::lib_rtl_unlock(m_lock);
      return;
    }
  }
  Py_XDECREF(pDict);

  Py_Finalize();
  error("Unknown command:"+in);
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
      info("Locking event loop. Waiting for work....");
      ::lib_rtl_lock(m_lock);
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

// int DimRPCFileReader::sendData() {
//  PyObject *pName, *pModule, *pEncode, *pValue; 
//  PyObject *pMsg, *pDict, *pArgs;
//  std::string result;
//  Py_Initialize();
//  pName = PyString_FromString("DEncode");
//  pModule = PyImport_Import(pName);
//   Py_DECREF(pName);
//  
//  if (pModule != NULL ) {
//    pEncode = PyObject_GetAttrString(pModule, "encode");
//    if (pEncode && PyCallable_Check(pEncode)) {
//      pArgs = PyTuple_New(1);
//      pMsg = PyList_New(3);
//      pDict= PyDict_New();
//      PyDict_SetItem(pDict,PyString_FromString("fileID"),PyInt_FromLong(1));
//      PyList_SetItem(pMsg,0,PyString_FromString("commandName"));
//      PyList_SetItem(pMsg,1,PyInt_FromLong(1));
//      PyList_SetItem(pMsg,2,pDict); 
//      PyTuple_SetItem(pArgs,0,pMsg);
//      pValue = PyObject_CallObject(pEncode, pArgs);
//      Py_DECREF(pMsg);
//      Py_DECREF(pDict);
//      Py_DECREF(pArgs);     
//      if (pValue != NULL) {
//        result = PyString_AsString(pValue);
//        Py_DECREF(pValue);
//      } else {fprintf(stderr,"Call failed\n");}
//    }
//    Py_XDECREF(pEncode);
//     Py_DECREF(pModule);
//  }
//  Py_Finalize();
//   return 1;
// }
#endif

