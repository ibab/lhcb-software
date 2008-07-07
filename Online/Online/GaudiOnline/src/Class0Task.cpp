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
      //::fprintf(stdout,"Calling init\n");fflush(stdout);
      result = initApplication();
      if ( result == 1 ) {
	//::fprintf(stdout,"Calling start\n");fflush(stdout);
	result = startApplication();
	if ( result == 1 ) {
	  //::fprintf(stdout,"m_configured = true\n");fflush(stdout);
	  m_configured = true;
	  IOCSENSOR.send(this, STARTUP_DONE);
	  return StatusCode::SUCCESS;
	}
      }
    }
    declareState(ST_ERROR);
    return StatusCode::FAILURE;
  }
  return DimTaskFSM::configure();
}

StatusCode LHCb::Class0Task::terminate()  {
  int result = stopApplication();
  if (1 == result) {
    //::fprintf(stdout,"Calling finalize\n");fflush(stdout);
    result = finalizeApplication();
    m_configured = false;
    if ( result == 1 ) {
      //::fprintf(stdout,"Calling terminate\n");fflush(stdout);
      result = terminateApplication();
      if ( result == 1 ) {
	//::fprintf(stdout,"m_configured=false\n");fflush(stdout);
	m_configured = false;
	return DimTaskFSM::terminate();
      }
    }
  }
  declareState(ST_STOPPED);
  return StatusCode::FAILURE;
}

StatusCode LHCb::Class0Task::unload()  {
  if ( m_configured ) terminate();
  return DimTaskFSM::unload();
}
