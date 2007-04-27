#include "PVSSManager/APIManager.h"
#include "PVSSManager/MgrResources.h"
#include "PVSS/Internals.h"
#include <signal.h>

PVSS::IAPIManager* PVSS::pvss_create_manager(const char* dll, const char* fun)  {
  // Let Manager handle SIGINT and SIGTERM (Ctrl+C, kill)
  // Manager::sigHdl will call virtual function Manager::signalHandler 
	signal(SIGINT,  Manager::sigHdl);
	signal(SIGTERM, Manager::sigHdl);

  //_asm int 3
  int argc = 1;
  const char* argv1[] = {"PVSS00interactive"};
  const char* argv3[] = {"PVSS00interactive",dll,fun};
  char** argv = (char**)argv1;
  if ( dll && strlen(dll)>0 ) {
    if ( fun && strlen(fun)>0 )  {
      argc = 3;
      argv = (char**)argv3;
    }
  }
  int argcc = argc;

  // Initialize Resources, i.e. 
  //  - interpret commandline arguments
  //  - interpret config file
	MgrResources::init(argcc, argv);

  // Are we called with -helpDbg or -help ?
  if (MgrResources::getHelpDbgFlag())  {
    MgrResources::printHelpDbg();
    return 0;
  }

  if (MgrResources::getHelpFlag())  {
    MgrResources::printHelp();
    return 0;
  }

  PVSS::IAPIManager *mgr = 0;
  if ( argcc >= 3 ) mgr = new PVSS::APIManager(argv[1],argv[2]);
  else              mgr = new PVSS::APIManager("","");
  return mgr;
}

extern "C" int pvss_run_apimanager(int argc, char *argv[])   {
  // Now run our demo manager
  PVSS::IAPIManager *mgr = 0;
  for(int i=0; i<argc; ++i) {
    std::cout << "Argument [" << i << "]: " << argv[i] << std::endl;
  }
  if ( argc >= 3 ) mgr = PVSS::pvss_create_manager(argv[1],argv[2]);
  else             mgr = PVSS::pvss_create_manager("","");

  if ( 0 == mgr->initialize() )  {
    ::printf("Failed to initialize API manager.\n");
    return 1;
  }
  /// Run the manager
  mgr->run();
  // Exit gracefully :) 
  // Call Manager::exit instead of ::exit, so we can clean up
  mgr->exit(0);	
  // Just make the compilers happy...
  return 0;
}


//int main(int argc, char* argv[])  {
//  return pvss_apimanager(argc,argv);
//}
