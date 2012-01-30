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
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiOnline/OnlineRunable.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"
#include <cstring>

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineRunable)

// Standard Constructor
LHCb::OnlineRunable::OnlineRunable(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc)
{
  declareProperty("InitFlags", m_initFlags);
  declareProperty("Call",      m_call);
  declareProperty("Dll",       m_dll);
  declareProperty("Wait",      m_wait = 1);
  declareProperty("Warnings",  m_warnings = false); // Dummy for compatibility
  ::wtc_init();
}

// Standard Destructor
LHCb::OnlineRunable::~OnlineRunable()   {
}

// IInterface implementation: Query interface
StatusCode LHCb::OnlineRunable::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( IEventProcessor::interfaceID().versionMatch(riid) )
    *ppIf = (IEventProcessor*)this;
  else if ( IRunable::interfaceID().versionMatch(riid) )
    *ppIf = (IRunable*)this;
  else
    return Service::queryInterface(riid, ppIf);
  addRef();
  return StatusCode::SUCCESS;
}

// IRunable implementation: Run the object
StatusCode LHCb::OnlineRunable::run() {
  MsgStream log(msgSvc(),name());
  if ( !m_call.empty() && !m_dll.empty() )  {
    void* handle = 0;
    StatusCode sc = System::loadDynamicLib(m_dll,&handle);
    if ( sc.isSuccess() )  {
      int (*fun)(int, char**) = 0;
      sc = System::getProcedureByName(handle,m_call,(System::EntryPoint*)&fun);
      if ( sc.isSuccess() )  {
        size_t ikey = 1;
        char* items[64];
        std::string tmp = m_initFlags;
        if ( !m_initFlags.empty() )  {
          items[0] = (char*)"dummy.exe";
          for(char* tok=::strtok((char*)tmp.c_str()," "); tok; tok=::strtok(NULL," ")) {
            items[ikey++] = tok;
          }
          for(size_t i=0; i<ikey; ++i)  {
            if ( ::strchr(items[i],' ') != 0 ) {
              *strchr(items[i],' ') = 0;
            }
          }
        }
        sc = (*fun)(ikey, items);
        if ( sc.isSuccess() )  {
          return wait();
        }
        log << MSG::ERROR << "Failed to execute function:" << m_call 
            << " Error=" << System::getLastErrorString() << endmsg;
        return StatusCode::FAILURE;
      }
      log << MSG::ERROR << "Failed to access function:" << m_call
          << " Error=" << System::getLastErrorString() << endmsg;
      return StatusCode::FAILURE;
    }
    log << MSG::ERROR << "Failed to load DLL:" << m_dll
        << " Error=" << System::getLastErrorString() << endmsg;
    return StatusCode::FAILURE;
  }
  return wait();
}

StatusCode LHCb::OnlineRunable::wait() {
  if ( m_wait == 1 )  {
    MsgStream log(msgSvc(),name());
    int status, substatus;
    unsigned int facility;
    void* param;
    log << MSG::DEBUG << "Entering wtc_wait....." << endmsg;
    while(1)  {
      status = wtc_wait( &facility, &param, &substatus );
      if ( status != WT_SUCCESS && status != WT_BADACTIONSTAT && status != WT_NOSUBSCRIBED) {
        log << MSG::INFO << " exiting wt_wait status = " << status
            << " param " << param << " substat " << substatus << " fac " << facility << endmsg;
      }
    } 
  }
  else if ( m_wait == 2 )  {
    while(1) lib_rtl_sleep(100);
  }
  else if ( m_wait == 3 )  {
    // noop
  }
  return StatusCode::SUCCESS;
}

/// Process single event
StatusCode LHCb::OnlineRunable::executeEvent(void* /* par */ )    {
  return StatusCode::SUCCESS;
}

/// Process the maxevt events as a Run (beginRun() and endRun() called)
StatusCode LHCb::OnlineRunable::executeRun(int /* maxevt */ )   {
  return StatusCode::SUCCESS;
}

/// Process the next maxevt events
StatusCode LHCb::OnlineRunable::nextEvent(int /* maxevt */ )    {
  return StatusCode::SUCCESS;
}

/// Schedule a stop of the current event processing
StatusCode LHCb::OnlineRunable::stopRun()    {
  return StatusCode::SUCCESS;
}
