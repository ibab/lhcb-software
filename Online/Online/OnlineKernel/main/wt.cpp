#include "RTL/DllAccess.h"
#include <iostream>

using OnlineBase::FuncPtrCast;

int main (int argc, char** argv)  {
  void* handle = LOAD_LIB2("OnlineKernel");
  if ( 0 != handle )  {
    typedef int (*func_t)(int, char**);
    func_t fun = FuncPtrCast<func_t>(GETPROC(handle, argv[1]));
    if ( fun ) {
      return (*fun)(argc-1, &argv[1]);
    }
    std::cout << "Failed to access test procedure!" << std::endl;
  }
  std::cout << "Failed to load test library!" << std::endl;
  std::cout << "Error: " << DLERROR << std::endl;
  return 0;
}
