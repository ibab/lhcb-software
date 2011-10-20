// $Id: Confine.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Confine
#include "LbBcVegPy/Confine.h"

// set pointer to zero at start
Confine::CONFINE* Confine::s_confine =0;

// Constructor
Confine::Confine() : m_dummy( 0 ) , m_realdummy( 0. ) { }

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






