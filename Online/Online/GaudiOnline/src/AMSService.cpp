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
#include "GaudiOnline/AMSService.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,AMSService)

// Standard Constructor
LHCb::AMSService::AMSService(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc)
{
}

// Standard Destructor
LHCb::AMSService::~AMSService()   {
}

// IService implementation: Initialize the service
StatusCode LHCb::AMSService::initialize()    {
  StatusCode status = Service::initialize();
  MsgStream log(msgSvc(),name());
  int retry = 5;
  status = amsuc_init();
  if ( AMS_SUCCESS != status )  {
    log << MSG::ERROR << "amsuc_init Failed status:" << status 
        << ". " << lib_rtl_error_message(status) << endmsg;
    return StatusCode::FAILURE;
  }
  do  {
    status = amsc_init(0);
    if ( AMS_SUCCESS == status ) {
      break;
    }
    log << MSG::ERROR << "amsc_init Failed status:" << status << " Retrying...." << endmsg;
    lib_rtl_sleep(50);
    retry--;
  } while(retry > 0);
  if ( AMS_SUCCESS != status ) {
    log << MSG::ERROR << "amsc_init Failed status:" << status 
        << ". " << lib_rtl_error_message(status) << endmsg;
    exit(status);
  }
  status = wtc_subscribe(WT_FACILITY_AMS,0,amsuc_dispatch);
  if ( status != WT_SUCCESS ) {
    log << MSG::ERROR << "wtc_subscribe Failed status:" << status 
        << ". " << lib_rtl_error_message(status) << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// IService implementation: Finalize the service
StatusCode LHCb::AMSService::finalize()     {
  wtc_remove(WT_FACILITY_AMS);
  amsc_close();
  return Service::finalize();
}
