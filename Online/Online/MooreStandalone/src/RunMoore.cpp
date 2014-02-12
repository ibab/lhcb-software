#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"

int runMoore(const std::string& opts) {
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( iface );
  SmartIF<IAppMgrUI>     appMgr  ( iface );

  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }

  propMgr->setProperty( "JobOptionsPath", opts ).ignore();
  propMgr->setProperty( "MessageSvcType","LHCb::UpiMessageSvc");

  // Run the application manager and process events
  StatusCode sc = appMgr->run();
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

