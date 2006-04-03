//	============================================================
//
//	MEPRxSvc.cpp
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiOnline/MEPRxSvc.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPRxSvc)

// Standard Constructor
LHCb::MEPRxSvc::MEPRxSvc(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc), m_handle(0), m_receiveEvents(true)
{
  declareProperty("ConsumeEvents",m_receiveEvents);
}

// Standard Destructor
LHCb::MEPRxSvc::~MEPRxSvc()   {
}

int LHCb::MEPRxSvc::exec(void* arg)  {
  LHCb::MEPRxSvc* s = (LHCb::MEPRxSvc*)arg;
  return s->execute();
}

int LHCb::MEPRxSvc::execute()  {
  while ( m_receiveEvents )  {
    lib_rtl_sleep(500);
    std::cout << "Sleep.." << std::endl;
  }
  return 1;
}

// IInterface implementation: Query interface
StatusCode 
LHCb::MEPRxSvc::queryInterface(const InterfaceID& riid,void** ppvInterface) {
  if ( riid == IID_IRunable )   {
    *ppvInterface = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  // Interface is not directly availible: try out a base class
  return Service::queryInterface(riid, ppvInterface);
}

StatusCode LHCb::MEPRxSvc::initialize()  {
  MsgStream log(msgSvc(),name());
  log << MSG::ALWAYS << "Entering initialize....." << endmsg;
  int sc = lib_rtl_start_thread(MEPRxSvc::exec, this, &m_handle);
  if ( lib_rtl_is_success(sc) )  {
    return StatusCode::SUCCESS;
  }
  log << MSG::ERROR << "Failed to spawn off thread." << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LHCb::MEPRxSvc::finalize()  {
  m_receiveEvents = false;
  int sc = lib_rtl_join_thread(m_handle);
  return StatusCode::SUCCESS;
}

// IRunable implementation: Run the object
StatusCode LHCb::MEPRxSvc::run() {
  MsgStream log(msgSvc(),name());
  log << MSG::ALWAYS << "Entering run....." << endmsg;
  m_receiveEvents = true;
  return StatusCode::SUCCESS;
}
