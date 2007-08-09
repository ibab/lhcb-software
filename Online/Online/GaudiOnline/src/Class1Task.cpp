#include "GaudiOnline/Class1Task.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class1Task)

StatusCode LHCb::Class1Task::configure()  {
  int result = configApplication();
  switch(result) {
  case 2:
    declareState(ST_NOT_READY);
    break;
  case 1:
    return DimTaskFSM::configure();
  case 0:
  default:
    declareState(ST_ERROR);
    break;
  }
  return StatusCode::FAILURE;
}
    
StatusCode LHCb::Class1Task::initialize()  {
  int result = initApplication();
  switch(result) {
  case 3:
    declareState(ST_READY);
    return StatusCode::SUCCESS;
  case 1:
    DimTaskFSM::initialize();
    return StatusCode::SUCCESS;
  case 0:
  default:
    declareState(ST_ERROR);
    return StatusCode::FAILURE;
  }
}

StatusCode LHCb::Class1Task::finalize()  {
  int result = finalizeApplication();
  if (1 == result) {
    return DimTaskFSM::finalize();
  }
  declareState(ST_RUNNING);
  return StatusCode::FAILURE;
}

StatusCode LHCb::Class1Task::terminate()  {
  int result = terminateApplication();
  switch(result) {
  case 1:
    return DimTaskFSM::terminate();
  default:
  case 0:
    declareState(ST_READY);
    return StatusCode::FAILURE;
  }
}
