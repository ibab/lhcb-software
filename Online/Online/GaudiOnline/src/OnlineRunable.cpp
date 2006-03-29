//	============================================================
//
//	OnlineRunable.cpp
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
#include "GaudiOnline/OnlineRunable.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineRunable)

// Standard Constructor
LHCb::OnlineRunable::OnlineRunable(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc)
{
  declareProperty("Debug",m_debug=0);
}

// Standard Destructor
LHCb::OnlineRunable::~OnlineRunable()   {
}

// IService implementation: Initialize the service
StatusCode LHCb::OnlineRunable::initialize()    {
  StatusCode status = Service::initialize();
  MsgStream log(msgSvc(),name());
  int retry = 5;
  if ( m_debug ) ::lib_rtl_start_debugger();
  if ( AMS_SUCCESS != (status=amsuc_init()))  {
    log << MSG::ERROR << "amsuc_init Failed status:" << status 
        << ". " << lib_rtl_error_message(status) << endmsg;
    return StatusCode::FAILURE;
  }
  do  {
    status=amsc_init(0);
    if ( AMS_SUCCESS == status ) {
      break;
    }
    log << MSG::ERROR << "amsc_init Failed status:" << status << " Retrying...." << endmsg;
    lib_rtl_sleep(100);
    retry--;
  } while(retry > 0);
  if ( AMS_SUCCESS != status ) {
    log << MSG::ERROR << "amsc_init Failed status:" << status 
        << ". " << lib_rtl_error_message(status) << endmsg;
    exit(status);
  }
  status=wtc_subscribe(WT_FACILITY_AMS,0,amsuc_dispatch);
  if ( status != WT_SUCCESS ) {
    log << MSG::ERROR << "wtc_subscribe Failed status:" << status 
        << ". " << lib_rtl_error_message(status) << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// IService implementation: Finalize the service
StatusCode LHCb::OnlineRunable::finalize()     {
  wtc_remove(WT_FACILITY_AMS);
  amsc_close();
  return Service::finalize();
}

// IInterface implementation: Query interface
StatusCode 
LHCb::OnlineRunable::queryInterface(const InterfaceID& riid,void** ppvInterface) {
  if ( riid == IID_IRunable )   {
    *ppvInterface = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  // Interface is not directly availible: try out a base class
  return Service::queryInterface(riid, ppvInterface);
}

// IRunable implementation: Run the object
StatusCode LHCb::OnlineRunable::run() {
  int status, substatus;
  unsigned int facility;
  void* param;
  MsgStream log(msgSvc(),name());
  log << MSG::ALWAYS << "Entering wtc_wait....." << endmsg;
  while(1)  {
    status = wtc_wait( &facility, &param, &substatus );  
    if ( status != WT_SUCCESS && status != WT_BADACTIONSTAT && status != WT_NOSUBSCRIBED) {
      log << MSG::WARNING << " exiting wt_wait status = " << status
          << " param " << param << " substat " << substatus << " fac " << facility << endmsg;
    }
  } 
  return StatusCode::SUCCESS;
}
