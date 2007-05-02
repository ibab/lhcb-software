#include "PVSSManager/APIManager.h"
#include "PVSSManager/MgrResources.h"
#include "PVSS/Internals.h"
#include <signal.h>

static PVSS::IAPIManager* s_mgr = 0;
static PVSS::IAPIManager* create_manager(int argc,const char** argv)
{
  if ( 0 == s_mgr )  {
    s_mgr = new PVSS::APIManager(argc, argv);
    if ( 0 == s_mgr->initialize() )  {
      ::printf("Failed to initialize API manager.\n");
      return 0;
    }
  }
  return s_mgr;
}

int PVSS::pvss_initialize(int& argc,const char** argv)
{
  static bool inited = false;
  if ( !inited ) {
    inited = true;
    // Let Manager handle SIGINT and SIGTERM (Ctrl+C, kill)
    // Manager::sigHdl will call virtual function Manager::signalHandler 
    signal(SIGINT,  Manager::sigHdl);
    signal(SIGTERM, Manager::sigHdl);

    // Initialize Resources, i.e. 
    //  - interpret commandline arguments
    //  - interpret config file
    MgrResources::init(argc, (char**)argv);

    // Are we called with -helpDbg or -help ?
    if (MgrResources::getHelpDbgFlag())  {
      MgrResources::printHelpDbg();
      return 0;
    }
    if (MgrResources::getHelpFlag())  {
      MgrResources::printHelp();
      return 0;
    }
  }
  return 1;
}


PVSS::IAPIManager* 
PVSS::pvss_create_manager(const char* name, const char* dll, const char* fun)
{
  const char* argv[] = {name,dll,fun};
  int argc = 3;
  if ( !pvss_initialize(argc,argv) ) {
    return 0;
  }
  return create_manager(argc,argv);
}

int PVSS::pvss_exec_manager(int argc,const char** argv)  {
  if ( !PVSS::pvss_initialize(argc,argv) ) {
    return 0;
  }
  PVSS::IAPIManager *mgr = create_manager(argc, argv);
  if ( mgr )   {
    int result = mgr->exec(false);
    if ( !result ) {
      mgr->exit(0);	
      return 0;
    }
    mgr->run();
  }
  return 1;
}

extern "C" int pvss_run_apimanager(int argc,const char *argv[])
{
  PVSS::pvss_exec_manager(argc,argv);
  PVSS::IAPIManager *mgr = s_mgr;
  if ( mgr ) {
    /// Run the manager
    mgr->run();
    // Exit gracefully :) 
    // Call Manager::exit instead of ::exit, so we can clean up
    mgr->exit(0);	
    // Just make the compilers happy...
    return 1;
  }
  return 0;
}
