// $Header: $
// ====================================================================
//
// --------------------------------------------------------------------
//
// Author    : Markus Frank
//
// ====================================================================
#include "RTL/DllAccess.h"
#include <iostream>

/// Generic main program to start any process
/**
 *   Generic main program to start any process
 *   by loading a library and executing a 
 *   function within
 *
 *   \author  M.Frank
 *   \version 1.0
 */
int main (int argc, char** argv)  {
  if ( argc >= 3 )  {
    void* handle = LOAD_LIB( argv[1] );
    if ( handle )  {
      Function fun(GETPROC(handle, argv[2]));
      if ( fun.function ) {
	return (*fun.function)(argc-2, &argv[2]);
      }
      std::cout << "[ERROR] Failed to access test procedure: '" << argv[2] << "'" << std::endl;
      std::cout << "[ERROR] Error: " << DLERROR << std::endl;
      return EINVAL;
    }
    else {
      // The required function is not in the loaded image.
      // Try to load it directly from the executable
      Function f1(GETPROC(0, argv[2]));
      if ( f1.function ) {
	std::cout << "[WARNING] Executing function " << argv[2] 
		  << " from exe rather than image " << std::endl;
	return (*f1.function)(argc-2, &argv[2]);
      }
      std::cout << "[ERROR] Failed to access test procedure: '" << argv[2] << "'" << std::endl;
      std::cout << "[ERROR] Error: " << DLERROR << std::endl;
      return EINVAL;
    }
    std::cout << "[ERROR] Failed to load test library: '" << argv[1] << "'" << std::endl;
    std::cout << "[ERROR] Error: " << DLERROR << std::endl;
    return EINVAL;
  }
  std::cout << "[ERROR] usage: gentest.exe <library-name>  <function-name>  -arg [-arg]" << std::endl;
  return EINVAL;
}
