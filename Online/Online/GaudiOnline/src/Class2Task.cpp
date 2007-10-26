#include "GaudiOnline/Class2Task.h"
#include "CPP/IocSensor.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class2Task)

/// Standard constructor
LHCb::Class2Task::Class2Task(IInterface* svc) : GaudiTask(svc) {
  IOCSENSOR.send(this, STARTUP_DONE);
}

StatusCode LHCb::Class2Task::initialize()  {
  int result = configApplication();
  if ( result == 1 ) {
    result = initApplication();
    if ( result == 1 ) {
      return DimTaskFSM::initialize();
    }
  }
  declareState(ST_ERROR);
  return StatusCode::FAILURE;
}

StatusCode LHCb::Class2Task::finalize()  {
  int result = finalizeApplication();
  if ( result == 1 ) {
    result = terminateApplication();
    if ( result == 1 ) {
      return DimTaskFSM::finalize();
    }
  }
  declareState(ST_STOPPED);
  return StatusCode::FAILURE;
}
