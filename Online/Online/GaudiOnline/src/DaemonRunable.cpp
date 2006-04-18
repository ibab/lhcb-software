//	============================================================
//
//	DaemonRunable.cpp
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiOnline/DaemonRunable.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DaemonRunable)

// Standard Constructor
LHCb::DaemonRunable::DaemonRunable(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc)
{
}

// Standard Destructor
LHCb::DaemonRunable::~DaemonRunable()   {
}

// IInterface implementation: Query interface
StatusCode LHCb::DaemonRunable::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( riid == IID_IEventProcessor )
    *ppIf = (IEventProcessor*)this;
  else if ( riid == IID_IRunable )
    *ppIf = (IRunable*)this;
  else
    return Service::queryInterface(riid, ppIf);
  addRef();
  return StatusCode::SUCCESS;
}

/// Process single event
StatusCode LHCb::DaemonRunable::run()  {
  return StatusCode::SUCCESS;
}

/// Process single event
StatusCode LHCb::DaemonRunable::executeEvent(void* /* par */ )    {
  return StatusCode::SUCCESS;
}

/// Process the maxevt events as a Run (beginRun() and endRun() called)
StatusCode LHCb::DaemonRunable::executeRun(int /* maxevt */ )   {
  return StatusCode::SUCCESS;
}

/// Process the next maxevt events
StatusCode LHCb::DaemonRunable::nextEvent(int /* maxevt */ )    {
  return StatusCode::SUCCESS;
}

/// Schedule a stop of the current event processing
StatusCode LHCb::DaemonRunable::stopRun()    {
  return StatusCode::SUCCESS;
}
