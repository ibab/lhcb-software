#include "GaudiOnline/GaudiDeamon.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartIF.h"
#include "WT/wt_facilities.h"
#include "WT/wtdef.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiDeamon)

LHCb::GaudiDeamon::GaudiDeamon(IInterface*) : DimTaskFSM(0), m_appMgr(0)  {
  propertyMgr().declareProperty("Runable",        m_runable     = "LHCb::OnlineRunable");
  propertyMgr().declareProperty("EventLoop",      m_evtloop     = "MinimalEventLoopMgr");
  propertyMgr().declareProperty("MessageSvcType", m_msgsvcType  = "MessageSvc");
  propertyMgr().declareProperty("JobOptionsPath", m_mainOptions = "jobOptions.txt");
}

LHCb::GaudiDeamon::~GaudiDeamon()  {
  if ( m_appMgr ) m_appMgr->release();
  m_appMgr= 0;
}

StatusCode LHCb::GaudiDeamon::run()  {
  SmartIF<IAppMgrUI> ui(Gaudi::createInstance("ApplicationMgr","ApplicationMgr","GaudiSvc"));
  if ( ui )  {
    SmartIF<IProperty> ip(ui);
    if ( ip )  {
      ip->setProperty(StringProperty("Runable",        m_runable));
      ip->setProperty(StringProperty("EventLoop",      m_evtloop));
      ip->setProperty(StringProperty("MessageSvcType", m_msgsvcType));
      ip->setProperty(StringProperty("JobOptionsPath", m_mainOptions));
      m_appMgr = ui;
      return m_appMgr->run();
    }
  }
  return DimTaskFSM::unload();
}

StatusCode LHCb::GaudiDeamon::unload()  {
  m_appMgr->finalize();
  m_appMgr->terminate();
  return DimTaskFSM::unload();
}
