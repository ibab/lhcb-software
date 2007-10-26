#include "GaudiOnline/Class1Task.h"
#include "CPP/IocSensor.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class1Task)

/// Standard constructor
LHCb::Class1Task::Class1Task(IInterface* svc) : GaudiTask(svc) {
  IOCSENSOR.send(this, STARTUP_DONE);
}

StatusCode LHCb::Class1Task::configure()  {
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
    
StatusCode LHCb::Class1Task::initialize()  {
  DimTaskFSM::initialize();
  return StatusCode::SUCCESS;
}

StatusCode LHCb::Class1Task::finalize()  {
  return DimTaskFSM::finalize();
}

StatusCode LHCb::Class1Task::terminate()  {
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
