#include "PVSSManager/APIManager.h"
#include "PVSSManager/MgrResources.h"
#include "PVSS/Internals.h"
#include "StartDpInitSysMsg.hxx"
#include "ConfigTypes.hxx"
#include <signal.h>

using namespace PVSS;
static int  g_doDebug = 1;
static bool g_doExit = false;
static bool g_isReady = false;

typedef long (*interface_function)(void (*exit_call)(int));

static void pvss_exit_manager(int)  {
  g_doExit = true;
}

static void log(const std::string& msg) {
#ifdef _WIN32
  printf("----------------> %s\n",msg.c_str());
#else
  std::cout << "----------------> " << msg << std::endl;
#endif
}

static void pvss_interface_run(void* arg)  {
  pvss_sleep(100);
  interface_function fun = interface_function(arg);
  int ret = (*fun)(pvss_exit_manager);
  pvss_end_thread(ret);
}

static void s_pvss_manager_run(void* arg)  {
  APIManager* m = (APIManager*)arg;
  int ret = 0;
  pvss_sleep(1000);
  if ( 0 != m->initialize() )  {
    m->run();
    m->exit(0);	
  }
  else {
    log("Failed to initialize API manager.");
    ret = 1;
  }
  pvss_end_thread(ret);
  g_isReady = true;
}

/// The constructor defines Manager type (API_MAN) and Manager number
APIManager::APIManager(const std::string& dll, const std::string& call)
: Manager(ManagerIdentifier(API_MAN, Resources::getManNum())),
  m_dll(dll), m_call(call)
{
}

/// Handle incoming hotlinks. This function is called from our hotlink object
void APIManager::handleHotLink(const DpHLGroup &/* group */)   {
  // Print Debug information
  if (Resources::isDbgFlag(Resources::DBG_API_USR1))
    log("Receiving HotLink\n");
}

/// Run the manager
int APIManager::initialize()   {
  static bool inited = false;
  if ( !inited ) {
    inited = true;
    long sec, usec;
    if ( g_doDebug > 1 ) log("Initializing...");
    // First connect to Data manager.
    // We want Typecontainer and Identification so we can resolve names
    // This call succeeds or the manager will exit
    connectToData(StartDpInitSysMsg::TYPE_CONTAINER | StartDpInitSysMsg::DP_IDENTIFICATION);
  
    // While we are in STATE_INIT we are initialized by the Data manager
    while (getManagerState() == STATE_INIT)  {
      // Wait max. 1 second in select to receive next message from data.
      sec = 1;
      usec = 0;
      dispatch(sec, usec);
    }
  
    // We are now in STATE_ADJUST and can connect to Event manager
    // This call will succeed or the manager will exit
    connectToEvent();  
  }
  if ( g_doDebug > 1 ) log("Initializing...done");
  return 1;
}

/// Optional execution through loaded library
int APIManager::exec(bool threaded) {
  // We are now in STATE_RUNNING.
  if ( !m_dll.empty() )  {
    interface_function fun = (interface_function)pvss_load_function(m_dll.c_str(),m_call.c_str());
    if ( !fun ) {
      log("Failed to access procedure:"+m_call+" id library:"+m_dll);
      return 0;
    }
    if ( g_doDebug > -1 ) log("Executing function "+m_call+" from "+m_dll);
    if ( threaded )  {
      pvss_start_thread(pvss_interface_run,(void*)fun);
    }
    else {
      pvss_interface_run((void*)fun);
    }
  }
  else {
    if ( g_doDebug > 0 ) log("No function to be executed");
  }
  return 1;
}

/// Execute PVSS dispatch loop
void APIManager::run()  {
  static bool running = false;
  if ( g_doDebug > 1 ) log("Entering dispatch loop...");
  if ( !running ) {
    running = true;
    log("Start dispatch loop.");
    while (1) {       // Now loop until we are finished
      if (g_doExit)  {  // Exit flag set ?
        return;
      }
      long sec = 0, usec = 2000;
      dispatch(sec, usec);
      g_isReady = true;
    }
    if ( g_doDebug > 1 ) log("Leaving dispatch loop...");
    return;
  }
  log("Manager already running.");
}

/// Execute PVSS dispatch loop in a seperate thread and return to command line
void APIManager::start()  {
  static bool started = false;
  if ( !started ) {
    started = true;
    pvss_start_thread( s_pvss_manager_run, this);
    while( !g_isReady ) pvss_sleep(10);
    log("PVSS API manager ready....control back to user.");
    return;
  }
  log("Manager already started.");
}

/// Stop PVSS dispatch loop
void APIManager::stop()  {
  log("APIManager: Received stop request.");
  g_doExit = true;
}

/// Graceful exit with manager cleanup
void APIManager::exit(int exit_code)  {
  Manager::exit(exit_code);
}

// Receive Signals. We are interested in SIGINT and SIGTERM. 
void APIManager::signalHandler(int sig)  {
  if ( (sig == SIGINT) || (sig == SIGTERM) )
    g_doExit = true;
  else
    Manager::signalHandler(sig);
}

