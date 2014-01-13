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

#include <cstdlib>
#include <iostream>
#include "GaudiKernel/System.h"
#include "GaudiKernel/StatusCode.h"

int main (int argc, char** argv)  {
  volatile int *x = new int; //force loading of tcmalloc
  *x = 0;
  System::ImageHandle h = 0;
  std::string env = std::string("__STARTUP__=")+THE_LOAD_LIBRARY;
  ::putenv((char*)env.c_str());
  StatusCode sc = System::loadDynamicLib("__STARTUP__",&h);
  if ( sc.isSuccess() ) {
    System::EntryPoint e;
    sc = System::getProcedureByName(h,THE_EXEC_FUNCTION,&e);
    if ( sc.isSuccess() ) {
      func fun = (func)e;
      if ( fun ) {
        return (*fun)(argc-2, &argv[2]);
      }
    }
    if ( argc>2 ) std::cout << "Failed to access procedure:" << THE_EXEC_FUNCTION << std::endl;
  }
  std::cout << "Failed to load library:";
  if ( argc>1 ) std::cout << THE_LOAD_LIBRARY << std::endl;
  else          std::cout << "<Unknown: No argument given>" << std::endl;
  std::cout << "Error: " << System::getLastErrorString() << std::endl;
  return 0;
}
