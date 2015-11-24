#include "GaudiOnline/Class1Task.h"
#include "GaudiKernel/MsgStream.h"
#include "CPP/IocSensor.h"

using namespace LHCb;
namespace {
  enum States { 
    TASK_OFFLINE,
    TASK_CONFIGURED,
    TASK_INITIALIZED,
    TASK_STARTING,
    TASK_STARTED,
    TASK_STOPPING,
    TASK_STOPPED, 
    TASK_FINALIZED,
    TASK_TERMINATED
  };
}

/// Standard constructor
Class1Task::Class1Task(IInterface* svc) : GaudiTask(svc, false) {
  IOCSENSOR.send(this, STARTUP_DONE);
  m_myState = TASK_OFFLINE;
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
      m_myState = TASK_CONFIGURED;
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
  m_myState = TASK_INITIALIZED;
  return DimTaskFSM::initialize();
}

/// Callback on stop transition
StatusCode Class1Task::finalize()  {
  m_continue = false;
  m_myState = TASK_FINALIZED;
  return declareState(ST_STOPPED);
}

/// Callback on activate transition
StatusCode Class1Task::activate()   {
  m_myState = TASK_STARTING;
  int result = startApplication();
  switch(result) {
  case 2:
    declareState(ST_READY);
    return StatusCode::FAILURE;
  case 1:
    if ( result == 1 ) {
      m_myState = TASK_STARTED;
      return DimTaskFSM::activate();
    }
    // Fall through in case of failure
  case 0:
  default:
    declareState(ST_ERROR);
    return StatusCode::FAILURE;
  }
}

/// Callback on configure transition
StatusCode Class1Task::start()  {
  m_myState = TASK_STARTING;
  int result = startApplication();
  switch(result) {
  case 2:
    declareState(ST_READY);
    return StatusCode::FAILURE;
  case 1:
    result = goApplication();
    if ( result == 1 ) {
      m_myState = TASK_STARTED;
      return DimTaskFSM::start();
    }
    // Fall through in case of failure
  case 0:
  default:
    declareState(ST_ERROR);
    return StatusCode::FAILURE;
  }
}

/// Callback on reset transition
StatusCode Class1Task::stop()  {
  int result = stopApplication();
  if (1 == result) {
    m_myState = TASK_STOPPED;
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
      m_myState = TASK_TERMINATED;
      return DimTaskFSM::terminate();
    }
  }
  declareState(ST_STOPPED);
  return StatusCode::FAILURE;
}

/// Callback on disable event processing
StatusCode Class1Task::disable()  {
  return GaudiTask::disable();
}

/// Callback on I/O cancel
StatusCode Class1Task::cancel()  {
  if ( m_myState == TASK_STOPPING ) {
    stopRunable();
    return StatusCode::SUCCESS;
  }
  m_myState = TASK_STOPPING;
  return GaudiTask::cancel();
}

