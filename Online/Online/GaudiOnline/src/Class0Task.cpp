#include "GaudiOnline/Class0Task.h"
#include "CPP/IocSensor.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class0Task)

/// Standard constructor
LHCb::Class0Task::Class0Task(IInterface* svc) : GaudiTask(svc) {
  m_configured = false;
  IOCSENSOR.send(this,CONFIGURE);
}

/// Default destructor
LHCb::Class0Task::~Class0Task() {
}

StatusCode LHCb::Class0Task::configure()  {
  if ( !m_configured ) {
    int result = configApplication();
    if ( result == 1 ) {
      result = initApplication();
      if ( result == 1 ) {
	m_configured = true;
        IOCSENSOR.send(this, STARTUP_DONE);
	return StatusCode::SUCCESS;
      }
    }
    declareState(ST_ERROR);
    return StatusCode::FAILURE;
  }
  return DimTaskFSM::configure();
}

StatusCode LHCb::Class0Task::terminate()  {
  int result = finalizeApplication();
  m_configured = false;
  if ( result == 1 ) {
    result = terminateApplication();
    if ( result == 1 ) {
      return DimTaskFSM::terminate();
    }
  }
  declareState(ST_STOPPED);
  return StatusCode::FAILURE;
}

