#include "GaudiOnline/GaudiTask.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartIF.h"
#include "CPP/IocSensor.h"
#include "WT/wt_facilities.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiTask)

LHCb::GaudiTask::GaudiTask(IInterface*) : GaudiDeamon(0), m_subMgr(0)  {
  propertyMgr().declareProperty("OptionalOptions",m_optOptions  = "");
  m_haveEventLoop = true;
}

LHCb::GaudiTask::~GaudiTask()  {
}

StatusCode LHCb::GaudiTask::cancel()  {
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

/// Set properties of application manager instance
StatusCode LHCb::GaudiTask::setInstanceProperties(IAppMgrUI* /* inst */ )  {
  return StatusCode::SUCCESS;
}

StatusCode LHCb::GaudiTask::configure()  {
  m_subMgr = Gaudi::createApplicationMgrEx("GaudiSvc", "ApplicationMgr");
  if ( m_subMgr )  {
    Gaudi::setInstance(m_subMgr);
    StatusCode sc = setInstanceProperties(m_subMgr);
    if ( sc.isSuccess() )  {
      sc = m_subMgr->configure();
      if ( sc.isSuccess() )  {
        return DimTaskFSM::configure();
      }
      m_subMgr->terminate();
    }
    return sc;
  }
  declareState(NOT_READY);
  return StatusCode::FAILURE;
}

StatusCode LHCb::GaudiTask::initialize()  {
  StatusCode sc = m_subMgr->initialize();
  if ( sc.isSuccess() )   {
    return DimTaskFSM::initialize();
  }
  declareState(READY);
  return sc;
}
	
StatusCode LHCb::GaudiTask::nextEvent(int /* num_event */)  {
  //_asm int 3
  return m_subMgr->nextEvent(1).isSuccess()  ? DimTaskFSM::nextEvent(1)
    : printErr(1,"Failed to execute single shot.");
}
	
StatusCode LHCb::GaudiTask::finalize()  {
  StatusCode sc = m_subMgr->finalize();
  if ( sc.isSuccess() )   {
    return DimTaskFSM::finalize();
  }
  declareState(RUNNING);
  return sc;
}

StatusCode LHCb::GaudiTask::terminate()  {
  StatusCode sc = m_subMgr->terminate();
  if ( sc.isSuccess() )   {
    m_subMgr->release();
    m_subMgr = 0;
    Gaudi::setInstance(m_appMgr);
    return DimTaskFSM::terminate();
  }
  declareState(READY);
  return sc;
}
