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
: Service(nam, svc)
{
}

// Standard Destructor
LHCb::MEPRxSvc::~MEPRxSvc()   {
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
  //_asm int 3
  log << MSG::ALWAYS << "Entering initialize....." << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode LHCb::MEPRxSvc::finalize()  {
  return StatusCode::SUCCESS;
}

// IRunable implementation: Run the object
StatusCode LHCb::MEPRxSvc::run() {
  MsgStream log(msgSvc(),name());
  log << MSG::ALWAYS << "Entering run....." << endmsg;
  return StatusCode::SUCCESS;
}
