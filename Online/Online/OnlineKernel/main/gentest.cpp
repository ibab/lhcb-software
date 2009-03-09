#include "RTL/DllAccess.h"
#include <iostream>

int main (int argc, char** argv)  {
  void* handle = LOAD_LIB( argv[1] );
  if ( 0 != handle )  {
    Function fun(GETPROC(handle, argv[2]));
    if ( fun.function ) {
      return (*fun.function)(argc-2, &argv[2]);
    }
    std::cout << "Failed to access test procedure!" << std::endl;
  }
  std::cout << "Failed to load test library!" << std::endl;
  std::cout << "Error: " << DLERROR << std::endl;
  return 0;
}
