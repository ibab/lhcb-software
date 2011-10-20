// $Id: Funtrans.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Funtrans
#include "LbBcVegPy/Funtrans.h"

// set pointer to zero at start
Funtrans::FUNTRANS* Funtrans::s_funtrans =0;

// Constructor
Funtrans::Funtrans() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Funtrans::~Funtrans() { }

// access nq2 in common
int& Funtrans::nq2() {
  init(); // check COMMON is initialized
  return s_funtrans->nq2;
}

// access npdfu in common
int& Funtrans::npdfu() {
  init(); // check COMMON is initialized
  return s_funtrans->npdfu;
}



