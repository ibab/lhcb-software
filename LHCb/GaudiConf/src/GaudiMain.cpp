//------------------------------------------------------------------------------
//
//  Package    : GaudiConf
//
//  Description: Main Program for Gaudi applications
//               Sets up default job options path as ../options/job.opts
//
//------------------------------------------------------------------------------
#define COMMON_MAIN_CPP

// Include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"

//--- Example main program
int main ( int argc, char** argv ) {
  StatusCode status = StatusCode::SUCCESS;
  // Create an instance of an application manager
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( IID_IProperty, iface );
  SmartIF<IAppMgrUI>     appMgr  ( IID_IAppMgrUI, iface );

  // Set properties of algorithms and services
  if ( propMgr == iface )    {
    std:: string opts = (argc>1) ? argv[1] : "../options/job.opts";
    status = propMgr->setProperty( StringProperty("JobOptionsPath", opts) );
  }
  else  {
    exit(0);
  }

  // Run the application manager and process events
  if ( appMgr )   {
    status = appMgr->run();
  }
  else  {
    return 0;
  }

  // All done - exit
  return 0;
}
