// access pythia common Pyint6
#include "LbPythia/Pyint6.h"

// set pointer to zero at start
Pyint6::PYINT6 * Pyint6::s_pyint6 = 0 ;

// Constructor
Pyint6::Pyint6(){ }
 
// Destructor
Pyint6::~Pyint6() { }

// access proc in common
std::string Pyint6::proc(int n) {
  init(); // check COMMON is initialized
  if ( n < 0 || n >= lenProc()) return "" ;

  char tch[ 28 ] ;
  int i = 0 ;
  for ( i = 0 ; i < 28 ; ++i ) 
    tch[ i ] = s_pyint6 -> proc[ 28 * n + i ] ;

  return std::string( tch ) ;
}

