#include "GaudiKernel/Algorithm.h"

static std::string never(Algorithm* p) {
   return p ? p->name() : "";
}   

#ifndef LOAD_LIBRARY
#define THE_LOAD_LIBRARY argv[1]
#else
#define THE_LOAD_LIBRARY #LOAD_LIBRARY
#endif

#ifndef EXEC_FUNCTION
#define THE_EXEC_FUNCTION argv[2]
#else
#define THE_EXEC_FUNCTION #EXEC_FUNCTION
#endif

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
#include <string>
#include <map>
#include <iostream>

int main (int argc, char** argv)  {
  never(0);
  void* handle = LOAD_LIB( THE_LOAD_LIBRARY );
  if ( 0 != handle )  {
    func fun = (func)GETPROC(handle, THE_EXEC_FUNCTION );
    if ( fun ) {
      return (*fun)(argc-2, &argv[2]);
    }
    if ( argc>2 ) std::cout << "Failed to access procedure:" << THE_EXEC_FUNCTION << std::endl;
    else          std::cout << "Failed to access procedure." << std::endl;
  }
  std::cout << "Failed to load library:";
  if ( argc>1 ) std::cout << THE_LOAD_LIBRARY << std::endl;
  else          std::cout << "<Unknown: No argument given>" << std::endl;
  std::cout << "Error: " << DLERROR << std::endl;
  return 0;
}
