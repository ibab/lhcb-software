// access GenXicc common Confine
#include "LbGenXicc/Confine.h"

// set pointer to zero at start
Confine::CONFINE* Confine::s_confine =0;

// Constructor
Confine::Confine() { }

// Destructor
Confine::~Confine() { }

// access ptcut in common
double& Confine::ptcut() {
  init(); // check COMMON is initialized
  return s_confine->ptcut;
}

// access etacut in common
double& Confine::etacut() {
  init(); // check COMMON is initialized
  return s_confine->etacut;
}

// access psetacut in common
double& Confine::psetacut() {      //F. zhang 01-04-11
  init(); // check COMMON is initialized
  return s_confine->psetacut;
}





