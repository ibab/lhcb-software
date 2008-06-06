#include "GaudiOnline/Class1Task.h"
#include "GaudiKernel/MsgStream.h"
#include "CPP/IocSensor.h"
#include "RTL/rtl.h"
#include <signal.h>

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class1Task);

using namespace LHCb;

/// Standard constructor
Class1Task::Class1Task(IInterface* svc) : GaudiTask(svc) {
  IOCSENSOR.send(this, STARTUP_DONE);
}

/// Callback on configure transition
StatusCode Class1Task::configure()  {
  int result = configApplication();
  switch(result) {
  case 2:
    declareState(ST_NOT_READY);
    break;
  case 1:
    result = initApplication();
    if ( result == 1 ) {
      return DimTaskFSM::configure();
    }
    declareState(ST_ERROR);
    break;
  case 0:
  default:
    declareState(ST_ERROR);
    break;
  }
  return StatusCode::FAILURE;
}

/// Callback on start transition 
StatusCode Class1Task::initialize()  {
  return DimTaskFSM::initialize();
}

/// Callback on stop transition
StatusCode Class1Task::finalize()  {
  m_continue = false;
  return declareState(ST_STOPPED);
}

extern "C" int lib_rtl_kill_thread(lib_rtl_thread_t handle, int sig);

/// Callback on reset transition
StatusCode Class1Task::terminate()  {
  char txt[256];
  for(int i=0; i<999999; ++i) {
    if ( eventThread() ) ::lib_rtl_sleep(4000);
    cancel();
    if ( eventThread() )  {
      if ( m_handle && i>2 ) {
	::sprintf(txt,"Kill runable thread to get out of event loop.");
	output(MSG::WARNING,txt);
	::lib_rtl_kill_thread(m_handle,SIGINT);
      }
      ::lib_rtl_sleep(500);
    }
    if ( !eventThread() ) break;
    ::sprintf(txt,"Retry No. %d to cancel runable thread......",i);
    output(MSG::WARNING,txt);
  }
  int result = finalizeApplication();
  if (1 == result) {
    result = terminateApplication();
    if( 1 == result ) {
      return DimTaskFSM::terminate();
    }
  }
  declareState(ST_STOPPED);
  return StatusCode::FAILURE;
}

/// Callback on disable event processing
StatusCode Class1Task::disable()  {
  m_continue = false;
  IOCSENSOR.send(this, FINALIZE);
  return StatusCode::SUCCESS;
}
