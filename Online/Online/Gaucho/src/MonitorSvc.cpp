// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaucho/MonitorSvc.h"
#include "Gaucho/DimEngine.h"
#include "Gaucho/DimPropServer.h"
#ifdef WIN32
namespace wins {
#include <Winsock.h>
}
#endif

// Factory for instantiation of service objects
static SvcFactory<MonitorSvc> s_factory;
const  ISvcFactory& MonitorSvcFactory = s_factory;

// UNIQUE Interface identifiers defined elsewhere
//! need the next declaration ?  compiler seems to find the static var
//extern const InterfaceID IID_IPublish;

// Constructor
MonitorSvc::MonitorSvc(const std::string& name, ISvcLocator* sl)
: Service(name, sl) {
}

//!destructor

//Query interfaces of Interface
// @param riid       ID of Interface to be retrieved
// @param ppvUnknown Pointer to Location for interface pointer
StatusCode MonitorSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IID_IMonitorSvc == riid) {
    *ppvIF = dynamic_cast<IMonitorSvc*> (this);
  } 
//  else if(IID_IMonitor == riid) {
//    *ppvIF = dynamic_cast<IMonitor*> (this);
//    return StatusCode::SUCCESS;
//  } 
  else {
    return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}

// implement Service methods
StatusCode MonitorSvc::initialize() {
  MsgStream log(msgSvc(),"MonitorSvc");
  Service::initialize();
  m_nodename=new char[50];
#ifdef WIN32
  int errcode;
  errcode=wins::gethostname(m_nodename,50);
#else
  gethostname(m_nodename,50);
#endif
//! limited to 1 gaudijob per cpu now, to be expanded with e.g. prefix set by property
  log << MSG::INFO << "found the nodename:" << m_nodename << endreq;
  m_dimeng=new DimEngine(m_nodename,serviceLocator());
  m_dimpropsvr= new DimPropServer(m_nodename,serviceLocator());
//! setProperties();
  return StatusCode::SUCCESS;
}


StatusCode MonitorSvc::finalize() {
  MsgStream log(msgSvc(),"MonitorSvc");
  delete [] m_nodename;
  delete m_dimeng;
  delete m_dimpropsvr;
  log << MSG::INFO << "finalized successfully" << endreq;
  return StatusCode::SUCCESS;
}


void MonitorSvc::DeclInfo(std::string InfoName, int& InfoVar){
  m_dimeng->declSvc(InfoName,InfoVar);
}

void MonitorSvc::UndeclInfo(std::string InfoName, int& InfoVar){
//  m_intInfos.erase(m_intInfos.find(InfoName));
  m_dimeng->undeclSvc(InfoName);
}

void MonitorSvc::PublInfo(std::string InfoName, int& InfoValue){
//! push method for Info, to be implemented
}

void MonitorSvc::DeclInfo(std::string InfoName, float& InfoVar){
  m_dimeng->declSvc(InfoName,InfoVar);
}

void MonitorSvc::UndeclInfo(std::string InfoName, float& InfoVar){
  m_dimeng->undeclSvc(InfoName);
}

void MonitorSvc::PublInfo(std::string InfoName, float& InfoValue){
//! push method for Info, to be implemented
}

void MonitorSvc::DeclInfo(std::string InfoName, char* InfoVar){
//  MsgStream log( msgSvc(), "Publish" );
/*  std::pair<StringMapIt,bool> p;
  m_stringInfosIt = m_stringInfos.end();
  p = m_stringInfos.insert(StringMap::value_type(InfoName,""));
  if (p.second) {
	m_stringInfos[InfoName]="";
//	std::cout << "New StringInfo successfully declared with value" << StringInfos[InfoName] << std::endl;
  }
  else {
	std::cout << "Error: already existing StringInfo:" << (*(p.first)).first
		<< " -> " << (*(p.first)).second << std::endl;
  }
*/
  m_dimeng->declSvc(InfoName,InfoVar);
}

void MonitorSvc::UndeclInfo(std::string InfoName, char* InfoVar){
  m_dimeng->undeclSvc(InfoName);
}

void MonitorSvc::PublInfo(std::string InfoName, char* InfoValue){
//! push method for Info, to be implemented
}

void MonitorSvc::DeclInfo(std::string InfoName, IHistogram1D* InfoVar){
  m_dimeng->declSvc(InfoName,InfoVar);
}

void MonitorSvc::UndeclInfo(std::string InfoName, IHistogram1D* InfoVar){
  m_dimeng->undeclSvc(InfoName);
}

void MonitorSvc::PublInfo(std::string InfoName, IHistogram1D* InfoValue){
//! push method for Info, to be implemented
}

