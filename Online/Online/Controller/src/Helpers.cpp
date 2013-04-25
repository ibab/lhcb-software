/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
// Framework include files
#include "FiniteStateMachine/config.h"
#include <stdexcept>
#include <cstdlib>

using namespace std;

/// Utility function to access the package ROOT directory
string FiniteStateMachine::controller_root()   {
  const char* env = ::getenv("CONTROLLERROOT");
  if ( env )  return env;
  throw runtime_error("The ROOT environment is not defined for the package Controller!");
}

/// Utility function to access the package binary directory
string FiniteStateMachine::controller_bindir()   {
  const char* env = ::getenv("CMTCONFIG");
  if ( env )  {
    string dir = controller_root() + "/" + env;
    return dir;
  }
  throw runtime_error("The CMTCONFIG environment is not defined for the package Controller!");
}

/// Utility function to access the package library directory
string FiniteStateMachine::controller_libdir()   {
  return controller_bindir();
}

/// Path to gentest.exe file
string FiniteStateMachine::gentest_path()    {
  const char* env = ::getenv("GENTEST_EXE");
  if ( env ) return env;
  throw runtime_error("The GENTEST_EXE environment is not defined for the package Controller!");
}

