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
  void* handle = LOAD_LIB( argv[1] );
  if ( 0 != handle )  {
    func fun = (func)GETPROC(handle, argv[2] );
    if ( fun ) {
      return (*fun)(argc-2, &argv[2]);
    }
    std::cout << "Failed to access test procedure!" << std::endl;
  }
  std::cout << "Failed to load test library!" << std::endl;
  std::cout << "Error: " << DLERROR << std::endl;
  return 0;
}
