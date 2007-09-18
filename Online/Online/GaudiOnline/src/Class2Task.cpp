#include "GaudiOnline/Class2Task.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class2Task)

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
  declareState(ST_READY);
  return StatusCode::FAILURE;
}
