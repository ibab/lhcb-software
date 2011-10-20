// $Id: Mixevnt.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Mixevnt
#include "LbBcVegPy/Mixevnt.h"

// set pointer to zero at start
Mixevnt::MIXEVNT* Mixevnt::s_mixevnt =0;

// Constructor
Mixevnt::Mixevnt() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Mixevnt::~Mixevnt() { }

// access xbcsec in common
double& Mixevnt::xbcsec(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenXbcsec()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_mixevnt->xbcsec[n-1];
}

// access imix in common
int& Mixevnt::imix() {
  init(); // check COMMON is initialized
  return s_mixevnt->imix;
}

// access imixtype in common
int& Mixevnt::imixtype() {
  init(); // check COMMON is initialized
  return s_mixevnt->imixtype;
}
