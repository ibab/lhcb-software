// $Id: Colflow.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Colflow
#include "LbBcVegPy/Colflow.h"

// set pointer to zero at start
Colflow::COLFLOW* Colflow::s_colflow = 0;

// Constructor
Colflow::Colflow() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Colflow::~Colflow() { }

// access amp2cf in common
double& Colflow::amp2cf(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenAmp2cf()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_colflow->amp2cf[n-1];
}

// access smatval in common
double& Colflow::smatval() {
  init(); // check COMMON is initialized
  return s_colflow->smatval;
}
