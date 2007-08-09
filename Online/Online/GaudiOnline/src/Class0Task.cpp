#include "GaudiOnline/Class0Task.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class0Task)

StatusCode LHCb::Class0Task::configure()  {
  int result = configApplication();
  if ( result == 1 ) {
    result = initApplication();
    if ( result == 1 ) {
      return DimTaskFSM::configure();
    }
  }
  declareState(ST_ERROR);
  return StatusCode::FAILURE;
}

StatusCode LHCb::Class0Task::terminate()  {
  int result = finalizeApplication();
  if ( result == 1 ) {
    result = terminateApplication();
    if ( result == 1 ) {
      return DimTaskFSM::terminate();
    }
  }
  declareState(ST_READY);
  return StatusCode::FAILURE;
}

