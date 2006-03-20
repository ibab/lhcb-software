#include "GaudiOnline/GaudiDimFSM.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/SmartIF.h"
#include "CPP/IocSensor.h"
#include "RTL/rtl.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>

extern "C" 
#ifdef _WIN32
__declspec(dllimport) 
#endif 
int GaudiMain(int, char**);

static void help() {}

typedef long (*func)(int, char**);

#ifdef _WIN32
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

static int error(const char* format, ...)  {
  va_list args;
  va_start( args, format );
  ::vprintf(format, args);
  return 0;
}

extern "C" int OnlineDeamon(int argc, char** argv)  {
  int result = 0;
  RTL::CLI cli(argc, argv, help);
  std::string dll = "", call = "";
  std::vector<char*> arg;
  cli.getopt("dll",3,dll);
  cli.getopt("call",4,call);
  if ( cli.getopt("debug",5) != 0 )
    ::lib_rtl_start_debugger();
  arg.push_back("OnlineDeamon");
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
      func fun = (func)GETPROC(handle, call.c_str());
      if ( fun ) {
        result = (*fun)(arg.size(), &arg[0]);
        if ( result&1 )  {
          printf("Starting DIM FSM....\n");
          LHCb::DimTaskFSM fsm;
          IOCSENSOR.send(&fsm, LHCb::DimTaskFSM::CONFIGURE);
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

extern "C" int GaudiOnline(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, help);
  std::string runable = "AppMgrRunable";
  std::string evtloop = "EventLoopMgr";
  std::string msgsvc  = "OnlineMessageSvc";
  std::string opts    = "jobOptions.txt";
  SmartIF<IProperty> p(Gaudi::createApplicationMgr());
  if(cli.getopt("runable",6,runable)) p->setProperty("Runable",runable);
  if(cli.getopt("evtloop",6,evtloop)) p->setProperty("EventLoop",evtloop);
  if(cli.getopt("msgsvc", 6,msgsvc) ) p->setProperty("MessageSvcType",msgsvc);
  if(cli.getopt("options",6,opts)   ) p->setProperty("JobOptionsPath",opts);

  if ( cli.getopt("debug",5) != 0 )  {
    ::lib_rtl_start_debugger();
  }
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
  IOCSENSOR.send(&fsm, LHCb::DimTaskFSM::CONFIGURE);
  return fsm.run();
}
