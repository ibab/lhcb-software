//  ============================================================
//
//  OnlineRunable.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : Markus Frank
//
//  ===========================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiOnline/AMSService.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,AMSService)

// Standard Constructor
LHCb::AMSService::AMSService(const std::string& nam, ISvcLocator* svc)
: OnlineService(nam, svc)
{
}

// Standard Destructor
LHCb::AMSService::~AMSService()   {
}

// IService implementation: Initialize the service
StatusCode LHCb::AMSService::initialize()    {
  StatusCode status = OnlineService::initialize();
  int retry = 5;

  if ( !status.isSuccess() )  {
    return error("Failed to initialize base class.");
  }
  status = amsuc_init();
  if ( AMS_SUCCESS != status )  {
    return error("amsuc_init Failed status:%d. %s",status.getCode(),lib_rtl_error_message(status));
  }
  do  {
    status = amsc_init(0);
    if ( AMS_SUCCESS == status ) {
      break;
    }
    error("amsc_init Failed. status:%d Retrying....", status.getCode());
    lib_rtl_sleep(50);
    retry--;
  } while(retry > 0);
  if ( AMS_SUCCESS != status ) {
    error("amsc_init Failed status:%d. %s",status.getCode(),lib_rtl_error_message(status));
    exit(status);
  }
  status = wtc_subscribe(WT_FACILITY_AMS,0,amsuc_dispatch);
  if ( status != WT_SUCCESS ) {
    return error("wtc_subscribe Failed status:%d. %s",status.getCode(),lib_rtl_error_message(status));
  }
  return StatusCode::SUCCESS;
}

// IService implementation: Finalize the service
StatusCode LHCb::AMSService::finalize()     {
  wtc_remove(WT_FACILITY_AMS);
  amsc_close();
  return OnlineService::finalize();
}
