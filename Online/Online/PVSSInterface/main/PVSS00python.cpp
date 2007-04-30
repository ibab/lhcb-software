// $Id: PVSS00python.cpp,v 1.1 2007-04-30 12:52:22 frankb Exp $
//  ====================================================================
//  PVSS00python.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================

// C++ include files
typedef long (*func)(int, char**);

#ifdef _WIN32
  #include <process.h>
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

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

static void loadEnv(const char* fname)  {
  typedef std::vector<std::string> EnvV;
  std::ifstream in(fname,std::ifstream::in);
  std::string line;
  if ( in.is_open() )  {
    do {
      line="";
      std::getline(in,line,'\n');
      for(size_t idx=line.find("  "); idx!=std::string::npos; idx = line.find("  "))
        line.replace(idx,2," ");
      if (strnicmp(line.c_str(),"set ",4)==0)  {
        if ( ::putenv(line.c_str()+4) != 0 )  {
          std::string err = "Failed to set environment:"+line;
          throw std::runtime_error(line);
        }
        std::cout << "Env:" << line.c_str()+4 << std::endl;
      }
    }
    while(in.good());
    return;
  }
  line = "Failed to open environment file.";
  line += fname;
  throw std::runtime_error(line);
}

int main(int argc, char** argv)  {
  typedef std::vector<char*> ArgV;
  ArgV args;
  char *env = 0, *env_val = 0;
  char *exe = 0, *exe_val = 0;
  char *dll = 0, *dll_val = 0;
  char *fun = 0, *fun_val = 0;
  for(int i=0; i<argc;++i)  {
    if (strncmp("-ENV",argv[i],4)==0) {
      env = argv[i];
      env_val = argv[++i];
    }
    else if (strncmp("-EXE",argv[i],4)==0) {
      exe = argv[i];
      exe_val = argv[++i];
    }
    else if (strncmp("-DLL",argv[i],4)==0) {
      dll = argv[i];
      dll_val = argv[++i];
    }
    else if (strncmp("-FUN",argv[i],4)==0) {
      fun = argv[i];
      fun_val = argv[++i];
    }
    else  {
      args.push_back(argv[i]);
    }
  }
  if ( env && env_val )  {
    try  {
      if ( dll ) {
        args.push_back(dll);
        args.push_back(dll_val);
      }
      if ( fun ) {
        args.push_back(fun);
        args.push_back(fun_val);
      }
      args.push_back(0);
      char** new_args = &args.front();
      char*  new_exe = exe ? exe_val : argv[0];
      if ( exe ) new_args[0] = new_exe;
      loadEnv(env_val);
      std::cout << "execv...." << _getpid() << std::endl;
      intptr_t p = execv(new_exe,new_args);
      if ( p == (intptr_t)-1 )  {
        printf("Failed to start process:%s\n",new_args[0]);
        return -1;
      }
    }
    catch(const std::exception& e)  {
      std::cout << "Failed to execute:" << e.what() << std::endl;
      return -1;
    }
  }
  else  {
    _asm int 3
    std::cout << "Executing...." << _getpid() << std::endl;
    void* handle = LOAD_LIB( dll );
    if ( 0 != handle )  {
      func function = (func)GETPROC(handle, fun );
      if ( function ) {
        return (*function)(args.size(), &args.front());
      }
      std::cout << "Failed to access test procedure!" << std::endl;
    }
    std::cout << "Failed to load test library!" << std::endl;
    std::cout << "Error: " << DLERROR << std::endl;
    return 0;
  }
  return -1;
}
