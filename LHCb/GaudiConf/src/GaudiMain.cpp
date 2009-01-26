// $Id: GaudiMain.cpp,v 1.9 2009-01-26 12:24:10 cattanem Exp $
//------------------------------------------------------------------------------
//
//  Package    : GaudiConf
//
//  Description: Main Program for Gaudi applications
//               Sets up default job options path as ../options/job.opts
//
//------------------------------------------------------------------------------
// Include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/StatusCode.h"
#include <iostream>

//--- Example main program
int main ( int argc, char** argv ) {
  // Create an instance of an application manager
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( iface );
  SmartIF<IAppMgrUI>     appMgr  ( iface );

  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }

  // Get the input configuration file from arguments
  std:: string opts = (argc>1) ? argv[1] :  "";

  propMgr->setProperty( "JobOptionsPath", opts );

  // Set the program name and version using macros defined in compilation flags
#if defined APPNAME
  propMgr->setProperty( "AppName",    APPNAME );
#endif
#if defined APPVERS
  propMgr->setProperty( "AppVersion", APPVERS );
#endif

  if( opts != "" && opts.substr( opts.length() - 3, 3 ) == ".py" ) {
    propMgr->setProperty( "EvtSel",         "NONE" );
    propMgr->setProperty( "JobOptionsType", "NONE" );
    propMgr->setProperty( "DLLs",           "['GaudiPython']" );
    propMgr->setProperty( "Runable",        "PythonScriptingSvc" );
  }

  // Run the application manager and process events
  StatusCode sc=appMgr->run();

  // Release Application Manager
  iface->release();

  // All done - exit
  return (sc==StatusCode::SUCCESS)?0:1;  
}
