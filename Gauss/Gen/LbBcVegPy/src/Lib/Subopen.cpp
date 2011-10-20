// $Id: Subopen.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Subopen
#include "LbBcVegPy/Subopen.h"

// set pointer to zero at start
Subopen::SUBOPEN* Subopen::s_subopen =0;

// Constructor
Subopen::Subopen() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Subopen::~Subopen() { }

// access subfactor in common
double& Subopen::subfactor() {
  init(); // check COMMON is initialized
  return s_subopen->subfactor;
}

// access subenergy in common
double& Subopen::subenergy() {
  init(); // check COMMON is initialized
  return s_subopen->subenergy;
}

// access isubonly in common
int& Subopen::isubonly() {
  init(); // check COMMON is initialized
  return s_subopen->isubonly;
}






