// access GenXicc common Wbstate
#include "LbGenXicc/Wbstate.h"

// set pointer to zero at start
Wbstate::WBSTATE* Wbstate::s_wbstate =0;

// Constructor
Wbstate::Wbstate() { }

// Destructor
Wbstate::~Wbstate() { }

// access ratiou in common
double& Wbstate::ratiou() {
  init(); // check COMMON is initialized
  return s_wbstate->ratiou;
}

// access ratiod in common
double& Wbstate::ratiod() {
  init(); // check COMMON is initialized
  return s_wbstate->ratiod;
}

// access ratios in common
double& Wbstate::ratios() {
  init(); // check COMMON is initialized
  return s_wbstate->ratios;
}

// access nbound in common
int& Wbstate::nbound() {
  init(); // check COMMON is initialized
  return s_wbstate->nbound;
}






