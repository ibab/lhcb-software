// $Id: PVSS00python.cpp,v 1.2 2007-05-02 14:46:19 frankm Exp $
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
  #define strncasecmp strnicmp  

#else
  #include <dlfcn.h>
  #include <unistd.h>
  #define LOAD_LIB(x)  ::dlopen( x , RTLD_NOW)
  inline void* GETPROC(void* h, const char* x) {  return ::dlsym(h,x); }
  inline const char* __DLERROR()  {
    const char* err=::dlerror(); 
    return err ? err : "Unknown DLL error"; 
  }
  #define DLERROR __DLERROR()
#endif

#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


static int loadEnv(const char* fname)  {
  FILE* fd = fopen(fname,"r");
  if ( fd ) {
    char* line = 0;
    ssize_t rd;
    size_t  len = 0;
    while((rd=::getline(&line,&len,fd)) != -1) {
      if (strncasecmp(line,"set ",4)==0)  {
        line[rd-1] = 0;
        ::printf("Env: %s\n",line+4);
	/*
	if ( ::putenv(line+4) != 0 )  {
	  printf("Failed to set environment:%s",line+4);
	  free(line);
	  return 0;
	}
	*/
      }
    }
    if ( line ) ::free(line);
    fclose(fd);
    return 1;
  }
  ::printf("Failed to open environment file:%s\n",fname);
  return 0;
}

int main(int argc, char** argv)  {
  char *env = 0, *exe = 0, *dll = 0, *fun = 0;
  char** args = new char*[argc+2];
  bool run = false;
  size_t narg = 0;
  for(int i=0; i<argc;++i)  {
    args[i] = 0;
    ::printf(" Arg[%2d] = %s\n",i,argv[i]);
    if (strncasecmp("-BLA",argv[i],4)==0) {
      env = argv[i];
    }
    else if (strncasecmp("-exe",argv[i],4)==0) {
      exe = argv[i];
    }
    else if (strncasecmp("-dll",argv[i],4)==0) {
      dll = argv[i];
    }
    else if (strncasecmp("-fun",argv[i],4)==0) {
      fun = argv[i];
    }
    else if (strncasecmp("-run",argv[i],4)==0) {
      run = true;
    }
    args[narg++]=argv[i];
  }
  printf("Environment:%s\n",env ? env : "None");
  if ( !run && env )  {
    if ( dll ) args[narg++] = dll;
    if ( fun ) args[narg++] = fun;
    args[narg++] = "-run";
    args[narg] = 0;
    args[0] = exe ? exe+5 : argv[0];
    ::printf("Loading environment:%s\n",env+5);
    if ( 1 == loadEnv(env+5) ) {
      ::printf("Switching executable ....\n");
      intptr_t p = execv(args[0],args);
      if ( p == (intptr_t)-1 )  {
	::printf("Failed to start process:%s\n",args[0]);
	return -1;
      }
    }
  }
  else if ( dll && fun ) {
    ::printf("Running DLL %s Narg:%d\n",dll+5,narg);
    void* handle = LOAD_LIB( dll+5 );
    if ( 0 != handle )  {
      func function = (func)GETPROC(handle, fun+5 );
      if ( function ) {
        return (*function)(narg, args);
      }
      ::printf("Failed to access procedure:%s\n",fun+5);
    }
    ::printf("Failed to load library:%s\n",dll+5);
    ::printf("Error: %s\n",DLERROR);
    return 0;
  }
  return -1;
}
