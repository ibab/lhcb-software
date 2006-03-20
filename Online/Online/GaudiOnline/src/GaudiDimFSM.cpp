#include "GaudiOnline/GaudiDimFSM.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/MsgStream.h"
#include "CPP/IocSensor.h"

LHCb::GaudiDimFSM::GaudiDimFSM(IAppMgrUI* ui) : DimTaskFSM(true), m_appMgr(ui)  {
  if (m_appMgr) m_appMgr->addRef();
}

LHCb::GaudiDimFSM::~GaudiDimFSM()  {
  if ( m_appMgr ) m_appMgr->release();
  m_appMgr= 0;
}

StatusCode LHCb::GaudiDimFSM::cancel()  {
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

StatusCode LHCb::GaudiDimFSM::config()  {
  return m_appMgr->configure().isSuccess() ? DimTaskFSM::config()
    : printErr(1,"Failed to configure Application manager.");
}

StatusCode LHCb::GaudiDimFSM::init()  {
  return m_appMgr->initialize().isSuccess() ? DimTaskFSM::init()
    : printErr(1,"Failed to initialize Application manager.");
}
	
StatusCode LHCb::GaudiDimFSM::process()  {
  return m_appMgr->nextEvent(1).isSuccess()  ? DimTaskFSM::process()
    : printErr(1,"Failed to execute single shot.");
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
