#include "GaudiOnline/GaudiDimFSM.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/MsgStream.h"
#include "CPP/IocSensor.h"

LHCb::GaudiDimFSM::GaudiDimFSM(IAppMgrUI* ui) : DimTaskFSM(0), m_appMgr(ui)  {
  if (m_appMgr) m_appMgr->addRef();
  IOCSENSOR.send(this, STARTUP_DONE);
}

LHCb::GaudiDimFSM::~GaudiDimFSM()  {
  if ( m_appMgr ) m_appMgr->release();
  m_appMgr= 0;
}

StatusCode LHCb::GaudiDimFSM::cancel()  {
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

StatusCode LHCb::GaudiDimFSM::configure()  {
  return m_appMgr->configure().isSuccess() ? DimTaskFSM::configure()
    : printErr(1,"Failed to configure Application manager.");
}

StatusCode LHCb::GaudiDimFSM::initialize()  {
  return m_appMgr->initialize().isSuccess() ? DimTaskFSM::initialize()
    : printErr(1,"Failed to initialize Application manager.");
}
  
/// Start the application
StatusCode LHCb::GaudiDimFSM::start()  {
  return m_appMgr->start().isSuccess() ? DimTaskFSM::start()
    : printErr(1,"Failed to start Application manager.");
}
  
StatusCode LHCb::GaudiDimFSM::nextEvent(int /* num_event */)  {
  //_asm int 3
  return m_appMgr->nextEvent(1).isSuccess()  ? DimTaskFSM::nextEvent(1)
    : printErr(1,"Failed to execute single shot.");
}
  
/// Stop the application
StatusCode LHCb::GaudiDimFSM::stop()  {
  return m_appMgr->stop().isSuccess() ? DimTaskFSM::stop()
    : printErr(1,"Failed to start Application manager.");
}
  
StatusCode LHCb::GaudiDimFSM::finalize()  {
  if ( !m_appMgr->finalize().isSuccess() )
    printErr(0,"Failed to finalize Application manager.");
  return DimTaskFSM::finalize();
}

StatusCode LHCb::GaudiDimFSM::terminate()  {
  if ( !m_appMgr->terminate().isSuccess() )
    printErr(0,"Failed to terminate Application manager.");
  return DimTaskFSM::terminate();
}
