// $Id: BooleMain.cpp,v 1.1.1.1 2003-07-16 11:53:44 cattanem Exp $
//------------------------------------------------------------------------------
//
//  Package    : Boole
//
//  Description: Main Program for Boole
//               Based on Rec/Brunel/v19r1/src/BrunelMain.cpp
//
//------------------------------------------------------------------------------
// Include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include <iostream>
#include <string>

//--- Example main program
int main ( int argc, char** argv ) {
  // Create an instance of an application manager
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( IID_IProperty, iface );
  SmartIF<IAppMgrUI>     appMgr  ( IID_IAppMgrUI, iface );

  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }

  // Check existence of LHCBDBASE environment variable
  std::string dbase = "";
  int posDbVers = 0;
  
  if(getenv("LHCBDBASE") != NULL) {
    dbase = std::string(getenv( "LHCBDBASE" ));
    posDbVers = dbase.find_last_of("/");
    if( 0 > posDbVers ) {
      // Could be the other way round (Windows)
      posDbVers = dbase.find_last_of("\\");
      if( 0 > posDbVers ) {
        std::cout << "Fatal error: LHCBDBASE path " << dbase
                  << " does not contain any / separator" << std::endl;
        return 1;
      }
    }
  }
  else {
    std::cout << "Fatal error: please set LHCBDBASE environment variable"
              << std::endl;
    return 1;
  }

  // Get the job options file name. Precedence is:
  //  1. argument
  //  2. JOBOPTPATH
  //  3. "$BOOLEOPTS/Boole"+dbver+".opts"

  std:: string opts = (argc>1) ? argv[1] :  "";

  if( NULL == getenv("JOBOPTPATH") && opts == "" )
    opts = "$BOOLEOPTS/Boole" + dbase.substr(posDbVers+1) + ".opts";

  propMgr->setProperty( "JobOptionsPath", opts );
  if( opts != "" && opts.substr( opts.length() - 3, 3 ) == ".py" ) {
    propMgr->setProperty( "EvtSel",         "NONE" );
    propMgr->setProperty( "JobOptionsType", "NONE" );
    propMgr->setProperty( "DLLs",           "['GaudiPython']" );
    propMgr->setProperty( "Runable",        "PythonScriptingSvc" );
  }

  // Run the application manager and process events
  appMgr->run();

  // Release Application Manager
  iface->release();

  // All done - exit
  return 0;
}
