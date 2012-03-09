// access GenXicc common Funtrans
#include "LbGenXicc/Funtrans.h"

// set pointer to zero at start
Funtrans::FUNTRANS* Funtrans::s_funtrans =0;

// Constructor
Funtrans::Funtrans() { }

// Destructor
Funtrans::~Funtrans() { }

// F. Zhang 01-04-11
// access nq2 in common
//int& Funtrans::nq2() {
//  init(); // check COMMON is initialized
//  return s_funtrans->nq2;
//}

// access npdfu in common
int& Funtrans::npdfu() {
  init(); // check COMMON is initialized
  return s_funtrans->npdfu;
}



