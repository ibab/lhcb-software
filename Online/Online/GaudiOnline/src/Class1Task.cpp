#include "GaudiOnline/Class1Task.h"
#include "GaudiKernel/MsgStream.h"
#include "CPP/IocSensor.h"

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

/// Callback on configure transition
StatusCode Class1Task::start()  {
  int result = startApplication();
  switch(result) {
  case 2:
    declareState(ST_READY);
    return StatusCode::FAILURE;
  case 1:
    return DimTaskFSM::start();
  case 0:
  default:
    declareState(ST_ERROR);
    return StatusCode::FAILURE;
  }
}

/// Callback on reset transition
StatusCode Class1Task::stop()  {
  stopRunable();
  int result = stopApplication();
  if (1 == result) {
    return DimTaskFSM::stop();
  }
  declareState(ST_RUNNING);
  return StatusCode::FAILURE;
}

/// Callback on reset transition
StatusCode Class1Task::terminate()  {
  stopRunable();
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
  IOCSENSOR.send(this, STOP);
  return StatusCode::SUCCESS;
}
