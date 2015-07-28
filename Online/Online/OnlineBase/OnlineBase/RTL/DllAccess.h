#ifndef ONLINEKERNEL_RTL_ALLACCESS_H
#define ONLINEKERNEL_RTL_ALLACCESS_H

typedef int (*func)(int, char**);
namespace {
  union Function {
    explicit Function(void* p) { ptr = p; }
    void* ptr;
    func  function;
  };
}
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
#include <limits.h>
//#define LOAD_LIB(x)  ::dlopen( x , RTLD_NOW)
inline void* LOAD_LIB(const std::string& x) {   return ::dlopen(x.c_str(),RTLD_NOW);  }
inline void* LOAD_LIB2(const std::string& x) {  return LOAD_LIB("lib"+x+".so"); }
inline void* GETPROC(void* h, const char* x) {  return ::dlsym(h,x); }
#define DLERROR      ::dlerror()
#endif

namespace OnlineBase {
  inline std::string currentCommand() {
    // see http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
    char buf[PATH_MAX] = {0}; // initialize buffer to 0s (see man 2 readlink)
    ssize_t status = readlink("/proc/self/exe", buf, PATH_MAX);
    if (status == -1) {
      return "";
    }
    return buf;
  }
  // Imported from GaudiKernel
  template <typename DESTPTR, typename SRCPTR>
  inline DESTPTR FuncPtrCast(SRCPTR ptr) {
    union {
      SRCPTR src;
      DESTPTR dst;
    } p2p;
    p2p.src = ptr;
    return p2p.dst;
  }
}

#endif // ONLINEKERNEL_RTL_ALLACCESS_H
