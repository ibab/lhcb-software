#include "GaudiOnline/Class2Task.h"
#include "CPP/IocSensor.h"

/// Standard constructor
LHCb::Class2Task::Class2Task(IInterface* svc) : GaudiTask(svc, false) {
  IOCSENSOR.send(this, STARTUP_DONE);
}

/// Callback on start transition
StatusCode LHCb::Class2Task::initialize()  {
  int result = configApplication();
  if ( result == 1 ) {
    result = initApplication();
    if ( result == 1 ) {
      result = startApplication();
      if ( result == 1 ) {
        result = goApplication();
        if ( result == 1 ) {
          return DimTaskFSM::initialize();
        }
      }
    }
  }
  declareState(ST_ERROR);
  return StatusCode::FAILURE;
}

/// Callback on activate transition
StatusCode LHCb::Class2Task::activate()  {
  int result = configApplication();
  if ( result == 1 ) {
    result = initApplication();
    if ( result == 1 ) {
      result = startApplication();
      if ( result == 1 ) {
        result = goApplication();
        if ( result == 1 ) {
          return DimTaskFSM::activate();
        }
      }
    }
  }
  declareState(ST_ERROR);
  return StatusCode::FAILURE;
}

StatusCode LHCb::Class2Task::finalize()  {
  int result = stopApplication();
  if ( result == 1 ) {
    result = finalizeApplication();
    if ( result == 1 ) {
      result = terminateApplication();
      if ( result == 1 ) {
        return DimTaskFSM::finalize();
      }
    }
  }
  declareState(ST_STOPPED);
  return StatusCode::FAILURE;
}
