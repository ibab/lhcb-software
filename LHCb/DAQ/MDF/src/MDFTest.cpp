// $Id: MDFTest.cpp,v 1.2 2006-01-10 18:14:29 frankb Exp $
//------------------------------------------------------------------------------
//
//  Description: Main Program for Gaudi applications
//
//------------------------------------------------------------------------------
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include <iostream>

extern "C" int MDFTest(int argc,char **argv) {
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( iface );
  SmartIF<IAppMgrUI>     appMgr  ( iface );
  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }
  // Get the input configuration file from arguments
  propMgr->setProperty("JobOptionsPath",std::string((argc>1)?argv[1]:""));
  // Run the application manager and process events
  appMgr->run();
  // Release Application Manager
  iface->release();
  // All done - exit
  return 0;
}

extern "C" int GaudiTask(int argc,char **argv) {
  return MDFTest(argc, argv);
}
