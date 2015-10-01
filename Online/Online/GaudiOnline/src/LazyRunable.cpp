#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiOnline/LazyRunable.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"
#include <cstring>

// Standard Constructor
LHCb::LazyRunable::LazyRunable(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc)
{
  ::wtc_init();
}

// Standard Destructor
LHCb::LazyRunable::~LazyRunable()   {
}

// IInterface implementation: Query interface
StatusCode LHCb::LazyRunable::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( IEventProcessor::interfaceID().versionMatch(riid) )
    *ppIf = (IEventProcessor*)this;
  else if ( IRunable::interfaceID().versionMatch(riid) )
    *ppIf = (IRunable*)this;
  else
    return Service::queryInterface(riid, ppIf);
  addRef();
  return StatusCode::SUCCESS;
}

// IRunable implementation: do nothing
StatusCode LHCb::LazyRunable::run() {
  return StatusCode::SUCCESS;
}


StatusCode LHCb::LazyRunable::executeEvent(void* /* par */ )    {
  return StatusCode::SUCCESS;
}

/// Process the maxevt events as a Run (beginRun() and endRun() called)
StatusCode LHCb::LazyRunable::executeRun(int /* maxevt */ )   {
  return StatusCode::SUCCESS;
}

/// Process the next maxevt events
StatusCode LHCb::LazyRunable::nextEvent(int /* maxevt */ )    {
  return StatusCode::SUCCESS;
}

/// Schedule a stop of the current event processing
StatusCode LHCb::LazyRunable::stopRun()    {
  return StatusCode::SUCCESS;
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,LazyRunable)
