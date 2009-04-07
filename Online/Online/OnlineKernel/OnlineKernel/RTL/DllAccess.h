#ifndef ONLINEKERNEL_RTL_ALLACCESS_H
#define ONLINEKERNEL_RTL_ALLACCESS_H

typedef long (*func)(int, char**);
union Function {
  Function(void* p) { ptr = p; }
  void* ptr;
  func  function;
};

#include <string>

#ifdef _WIN32
  #include <windows.h>
  inline void* LOAD_LIB(const std::string& x) {   return ::LoadLibrary(x.c_str());  }
  inline void* LOAD_LIB2(const std::string& x) {  return LOAD_LIB(x+".dll"); }
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
  //#define LOAD_LIB(x)  ::dlopen( x , RTLD_NOW)
  inline void* LOAD_LIB(const std::string& x) {   return ::dlopen(x.c_str(),RTLD_NOW);  }
  inline void* LOAD_LIB2(const std::string& x) {  return LOAD_LIB("lib"+x+".so"); }
  inline void* GETPROC(void* h, const char* x) {  return ::dlsym(h,x); }
  #define DLERROR      ::dlerror()
#endif

#endif // ONLINEKERNEL_RTL_ALLACCESS_H
