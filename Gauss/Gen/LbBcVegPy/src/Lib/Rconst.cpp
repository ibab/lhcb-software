// $Id: Rconst.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Rconst
#include "LbBcVegPy/Rconst.h"

// set pointer to zero at start
Rconst::RCONST* Rconst::s_rconst =0;

// Constructor
Rconst::Rconst() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Rconst::~Rconst() { }

// access pi in common
double& Rconst::pi() {
  init(); // check COMMON is initialized
  return s_rconst->pi;
}
