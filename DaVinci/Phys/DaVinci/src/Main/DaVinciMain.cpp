// $Id: DaVinciMain.cpp,v 1.6 2004-08-25 09:59:41 pkoppenb Exp $
//------------------------------------------------------------------------------
//
//  Package    : DaVinci
//
//  Description: Main Program for DaVinci
//               Based on Rec/Brunel/v9r0/src/BrunelMain.cpp
//
//------------------------------------------------------------------------------
// Include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include <iostream>
#include <string>

//--- Main program
int main ( int argc, char** argv ) {
  // Create an instance of an application manager
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( IID_IProperty, iface );
  SmartIF<IAppMgrUI>     appMgr  ( IID_IAppMgrUI, iface );

  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }

  // Print a program header ( name and version )
  std::string version = (std::string)getenv("APPVERSION");
  std::cout << "**********************************************************" 
            << std::endl;
  std::cout << "*                                                        *" 
            << std::endl;
  std::cout << "*                                                        *" 
            << std::endl;
  if (version.size()==5){   
    std::cout << "*                 DaVinci version " << version 
              << "                  *" << std::endl;
  } else {
    std::cout << "*                 DaVinci version " << version 
              << "                 *" << std::endl;
  }  
    std::cout << "*                                                        *"
            << std::endl;
  std::cout << "*                                                        *"
            << std::endl;
  std::cout << "**********************************************************"
            << std::endl;

  // Get the job options file name. Precedence is:
  //  1. argument
  //  2. JOBOPTPATH
  //  3. "$DAVINCIROOT/options/DaVinci.opts"

  std:: string opts = (argc>1) ? argv[1] :  "";

  if( NULL == getenv("JOBOPTPATH") && opts == "" )
    opts = "$DAVINCIROOT/options/DaVinci.opts";

  propMgr->setProperty( "JobOptionsPath", opts );
  if( opts != "" && opts.substr( opts.length() - 3, 3 ) == ".py" ) {
    propMgr->setProperty( "EvtSel",         "NONE" );
    propMgr->setProperty( "JobOptionsType", "NONE" );
    propMgr->setProperty( "DLLs",           "['GaudiPython']" );
    propMgr->setProperty( "Runable",        "PythonScriptingSvc" );
  }

  // Run the application manager and process events
  appMgr->run();

  // All done - exit
  return 0;
}
