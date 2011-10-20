// $Id: Wavezero.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Wavezero
#include "LbBcVegPy/Wavezero.h"

// set pointer to zero at start
Wavezero::WAVEZERO* Wavezero::s_wavezero =0;

// Constructor
Wavezero::Wavezero() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Wavezero::~Wavezero() { }

// access fbc in common
double& Wavezero::fbc() {
  init(); // check COMMON is initialized
  return s_wavezero->fbc;
}
