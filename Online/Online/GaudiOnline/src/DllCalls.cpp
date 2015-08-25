#include "GaudiOnline/GaudiDimFSM.h"
#include "GaudiOnline/GaudiTask.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/SmartIF.h"
#include "CPP/IocSensor.h"
#include "CPP/Interactor.h"
#include "RTL/rtl.h"

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cstdarg>

extern "C" 
#ifdef _WIN32
__declspec(dllimport) 
#endif 
int GaudiMain(int, char**);

//static void help() {}

typedef long (*func)(int, char**);

#ifdef _WIN32
  #define NOMSG
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #define LOAD_LIB(x)  ::LoadLibrary( x )
  #define GETPROC(h,x) ::GetProcAddress ( HMODULE(h), x )
  #define DLERROR      __dl_error()
  const char* __dl_error()  {
    static char s[1024] = "No error reporting implemented";
    void* lpMessageBuffer;
    ::FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      ::GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
      (LPTSTR) &lpMessageBuffer,
      0,
      NULL );
    strcpy(s, (const char*)lpMessageBuffer);
    ::LocalFree( lpMessageBuffer ); 
    return s;
  }
#else
  #include <dlfcn.h>
  #include <unistd.h>
  #define LOAD_LIB(x)  ::dlopen( x , RTLD_NOW)
  inline void* GETPROC(void* h, const char* x) {  return ::dlsym(h,x); }
  #define DLERROR      ::dlerror()
#endif

namespace {
  union function {
    func f;
    void* p;
    function(void* ptr) { p = ptr; }
  };
}

static int error(const char* format, ...)  {
  va_list args;
  va_start( args, format );
  ::vprintf(format, args);
  va_end(args);
  return 0;
}

extern "C" int OnlineStart(int argc, char** argv)  {
  void* handle = LOAD_LIB( argv[1] );
  if ( 0 != handle )  {
    function fun(GETPROC(handle, argv[2] ));
    if ( fun.f ) {
      return (*fun.f)(argc-2, &argv[2]);
    }
    std::cout << "Failed to access test procedure!" << std::endl;
  }
  std::cout << "Failed to load test library!" << std::endl;
  std::cout << "Error: " << DLERROR << std::endl;
  return 0;
}

static void help_OnlineDeamon() {
  std::cout << "usage: gentest.exe GaudiOnline.dll OnlineDeamon -option [-option]      " << std::endl; 
  std::cout << "    Invoke a routine hosted in a DLL                                   " << std::endl;
  std::cout << "                                                                       " << std::endl;
  std::cout << "    -dll=<dll-name>     DLL hosting the routine to be called.          " << std::endl;
  std::cout << "    -call=<call-name>   Name of the routine to be invoked.             " << std::endl;
  std::cout << "                        signature: int (*action)(int argc, char** ergv)" << std::endl;
  std::cout << "    -debug=yes          Invoke debugger at startup (WIN32)             " << std::endl;
  std::cout << "    -args ....          Any option will be passes to <call-name>       " << std::endl;
  std::cout << "    " << std::endl;
}

extern "C" int OnlineDeamon(int argc, char** argv)  {
  int result = 0;
  RTL::CLI cli(argc, argv, help_OnlineDeamon);
  std::string dll = "", call = "rtl_test_";
  std::vector<char*> arg;
  cli.getopt("dll",3,dll);
  cli.getopt("call",4,call);
  if ( cli.getopt("help",4)   != 0 )  {
    return 1;
  }
  if ( cli.getopt("debug",5) != 0 ) {
    ::lib_rtl_start_debugger();
  }
  arg.push_back((char*)"OnlineDeamon");
  for ( int i=0, use=0; i<argc; ++i ) {
    if ( 0 == strcmp(argv[i],"-args") && 0 == use ) {
      use = 1;
      continue;
    }
    if ( !use ) continue;
    std::cout << "Option:" << argv[i] << std::endl;
    arg.push_back(argv[i]);
  }
  if ( !dll.empty() && !call.empty() )  {
    void* handle = LOAD_LIB(dll.c_str());
    if ( 0 != handle )  {
      function fun(GETPROC(handle, call.c_str()));
      if ( fun.f ) {
        result = (*fun.f)(arg.size(), &arg[0]);
        if ( result&1 )  {
          printf("Starting DIM FSM....\n");
          LHCb::DimTaskFSM fsm(0);
          return fsm.run();
        }
        return error("Failed to execute procedure: %s\n",call.c_str());
      }
      return error("Failed to access procedure: %s\n",call.c_str());
    }
    return error("Failed to load library: %d\nError:%s",dll.c_str(),DLERROR);
  }
  return error("Invalid call. Usage is:\n    OnlineDeamon -dll=<name> -call=<name> [ -args <p1> ... <pn>]\n");
}

static void help_GaudiOnline() {
  std::cout << "usage: gentest.exe GaudiOnline.dll GaudiOnline -option [-option]" << std::endl;
  std::cout << "    -runable=<class-name>    Name of the gaudi runable to be executed" << std::endl;
  std::cout << "    -evtloop=<class-name>    Name of the event loop manager to be invoked" << std::endl;
  std::cout << "    -msgsvc=<class-name>     Name of the Gaudi message service to be installed" << std::endl;
  std::cout << "    -options=<file-name>     Job options file name" << std::endl;
  std::cout << "    -debug=yes               Invoke debugger at startup (WIN32)" << std::endl;
  std::cout << "    -loop                    Set event loop flag." << std::endl;
  std::cout << "    -auto[start]             Immediately go running without listening to DIM." << std::endl;
  std::cout << "    " << std::endl;
}

extern "C" int GaudiOnline(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, help_GaudiOnline);
  std::string runable = "AppMgrRunable";
  std::string evtloop = "EventLoopMgr";
  std::string msgsvc  = "MessageSvc";
  std::string opts    = "jobOptions.txt";
  bool autostart = cli.getopt("autostart",3) != 0;
  SmartIF<IProperty> p(Gaudi::createApplicationMgr());
  if ( cli.getopt("help",4)  ) return 1;
  if ( cli.getopt("debug",5) ) ::lib_rtl_start_debugger();
  if(cli.getopt("runable",6,runable)) p->setProperty(StringProperty("Runable",runable));
  if(cli.getopt("evtloop",6,evtloop)) p->setProperty(StringProperty("EventLoop",evtloop));
  if(cli.getopt("msgsvc", 6,msgsvc) ) p->setProperty(StringProperty("MessageSvcType",msgsvc));
  if(cli.getopt("options",6,opts)   ) p->setProperty(StringProperty("JobOptionsPath",opts));
  if ( p )  {
    size_t nargs = argc;
    char** args = argv;
    std::stringstream os;
    for ( size_t i=2; i<nargs; ++i ) {
      const char* cptr = *++args;
      if ( *cptr == '-' || *cptr == '/' )  {
        std::cout << "Option:" << cptr << std::endl;
        continue;
      }
      if ( *cptr++ == '"' )  {
        while(*cptr != '"' )  {
          if ( *cptr != '|' ) os << *cptr;
          if ( *cptr == ';' ) os << std::endl;
          cptr++;
        }
      }
    }
    if ( os.str().length() > 0 ) {
      std::fstream file;
#ifdef _WIN32
      std::string tmpnam = ::getenv("TMP") ? ::getenv("TMP") : ::getenv("TEMP");
#else
      std::string tmpnam = "/tmp";
#endif
      tmpnam += "/tmpopts.opts";
      file.open(tmpnam.c_str(), std::ios_base::out|std::ios_base::trunc);
      file << "#include \"" << opts << "\"" << std::endl << os.str() << std::endl;
      file.close();
      p->setProperty("JobOptionsPath",tmpnam);
    }
  }
  SmartIF<IAppMgrUI> ui(p);
  LHCb::GaudiDimFSM fsm(ui);
  if(cli.getopt("loop",4) != 0 )   {
    SmartIF<IProperty> prp(&fsm);
    prp->setProperty(BooleanProperty("HaveEventLoop",true));
  }
  if ( autostart )  {
    Interactor* actor = dynamic_cast<Interactor*>(&fsm);
    if ( actor )  {
      std::cout << "Commencing autostart sequence..." << std::endl;
      IOCSENSOR.send(actor,LHCb::DimTaskFSM::CONFIGURE);
      IOCSENSOR.send(actor,LHCb::DimTaskFSM::INITIALIZE);
      IOCSENSOR.send(actor,LHCb::DimTaskFSM::START);
    }
    else  {
      std::cout << "Autostart failed: LHCb::GaudiDimFSM is no Interactor!" << std::endl;
    }
  }
  return fsm.run();
}

static void help_OnlineTask() {
  std::cout << "usage: Gaudi.exe GaudiOnline.dll OnlineTask -option [-option]" << std::endl;
  std::cout << "    -run[able]=<class-name>    Name of the gaudi runable to be executed" << std::endl;
  std::cout << "    -evt[loop]=<class-name>    Name of the event loop manager to be invoked" << std::endl;
  std::cout << "    -msg[svc]=<class-name>     Name of the Gaudi message service to be installed" << std::endl;
  std::cout << "    -mai[noptions]=<file-name> 1st job options file name (usually empty)" << std::endl;
  std::cout << "    -opt[ions]=<file-name>     2nd level job options file name" << std::endl;
  std::cout << "    -loop                      Set event loop flag." << std::endl;
  std::cout << "    -debug=yes                 Invoke debugger at startup (WIN32 only)" << std::endl;
  std::cout << "    -auto[start]               Immediately go running without listening to DIM." << std::endl;
  std::cout << "    " << std::endl;
}

extern "C" int OnlineTask(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, help_OnlineTask);
  int milli = 0;
  std::string dll     = "";
  std::string type    = "LHCb::Class1Task";
  std::string runable = "LHCb::OnlineRunable/Runable";
  std::string evtloop = "LHCb::OnlineRunable/EmptyEventLoop";
  std::string msgsvc  = "MessageSvc";
  std::string opts    = "";
  std::string optopts = "";
  bool evtLoop    = cli.getopt("loop", 4) != 0;
  bool autostart  = cli.getopt("autostart", 3) != 0;
  bool checkpoint = cli.getopt("checkpoint", 1) != 0;
  bool do_finalize = cli.getopt("no_finalize",6) == 0;
  cli.getopt("dll", 3, dll);
  cli.getopt("dbg", 3, milli);
  cli.getopt("tasktype", 8, type);
  cli.getopt("runable", 3, runable);
  cli.getopt("evtloop", 3, evtloop);
  cli.getopt("msgsvc",  3, msgsvc);
  cli.getopt("mainoptions", 3, opts);
  cli.getopt("options", 3, optopts);
  if ( cli.getopt("help", 4) )    return 1;
  if ( cli.getopt("debug", 5) ) ::lib_rtl_start_debugger();
  if ( milli > 1000 && milli < 900000 ) {
    std::cout << "[ERROR] Connect debugger:   gdb --pid " << (int)::lib_rtl_pid() << std::endl;
    while(milli > 0) {
      ::lib_rtl_sleep(10);
      milli -= 10;
    }
  }
  SmartIF<IProperty> p(Gaudi::createInstance("", type, dll));
  if ( p )  {
    p->setProperty(StringProperty("JobOptionsPath", opts));
    p->setProperty(StringProperty("MessageSvcType", msgsvc));
    p->setProperty(StringProperty("EventLoop", evtloop));
    p->setProperty(StringProperty("Runable", runable));
    p->setProperty(StringProperty("OptionalOptions", optopts));
    p->setProperty(BooleanProperty("HaveEventLoop", evtLoop));
    p->setProperty(BooleanProperty("ExecuteFinalize", do_finalize));
    p->setProperty(IntegerProperty("AutoStart", checkpoint ? 1 : (autostart ? 2 : 0)));
    SmartIF<IRunable> runner(p);
    if ( runner )  {
      return runner->run();
    }
  }
  return 0;
}
