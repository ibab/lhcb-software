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
#include "WT/wtdef.h"
#include "RTL/rtl.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineRunable)

// Standard Constructor
LHCb::OnlineRunable::OnlineRunable(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc)
{
  ::wtc_init();
}

// Standard Destructor
LHCb::OnlineRunable::~OnlineRunable()   {
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
  log << MSG::DEBUG << "Entering wtc_wait....." << endmsg;
  while(1)  {
    status = wtc_wait( &facility, &param, &substatus );
    if ( status != WT_SUCCESS && status != WT_BADACTIONSTAT && status != WT_NOSUBSCRIBED) {
      log << MSG::INFO << " exiting wt_wait status = " << status
          << " param " << param << " substat " << substatus << " fac " << facility << endmsg;
    }
  } 
  return StatusCode::SUCCESS;
}
