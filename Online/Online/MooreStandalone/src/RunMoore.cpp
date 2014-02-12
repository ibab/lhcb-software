#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Property.h"
#include "MooreMainMenu.h"
#include "CPP/Interactor.h"
#include "CPP/IocSensor.h"
extern "C" int log_write_message(const char* text);

void MooreTest::MooreMainMenu::finishExecution()   {
  if ( m_svcLocator )   {
    SmartIF<IService> svc = m_svcLocator->service("Runable",false);
    if ( svc ) {
      Interactor* a = dynamic_cast<Interactor*>(svc.get());
      if ( a )   {
	m_ioc->send(a,1);
	write_message("Successfully send STOP command to tester.");
	return;
      }
    }
  }
  write_message("CANNOT send STOP command to tester. [Invalid Gaudi-state]");
}


int MooreTest::MooreMainMenu::runMoore(const std::string& opts) {
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( iface );
  SmartIF<IAppMgrUI>     appMgr  ( iface );

  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }

  propMgr->setProperty( "JobOptionsPath", opts ).ignore();
  propMgr->setProperty( "MessageSvcType","LHCb::GenericMessageSvc");

  // Run the application manager and process events
  StatusCode sc = StatusCode::SUCCESS;
  {
    SmartIF<ISvcLocator> loc(iface);
    m_svcLocator = loc.get();
    sc = appMgr->run();
    m_svcLocator = 0;
  }
  IntegerProperty returnCode("ReturnCode", 0);
  propMgr->getProperty(&returnCode).ignore();
  // Release Application Manager
  iface->release();
  // All done - exit
  if (sc.isFailure() && returnCode.value() == 0) {
    // propagate a valid error code in case of failure
    returnCode.setValue(1);
  }
  return returnCode.value();
}

