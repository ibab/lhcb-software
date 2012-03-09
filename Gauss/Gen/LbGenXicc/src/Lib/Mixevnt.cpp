// access GenXicc common Mixevnt
#include "LbGenXicc/Mixevnt.h"

// set pointer to zero at start
Mixevnt::MIXEVNT* Mixevnt::s_mixevnt =0;

// Constructor
Mixevnt::Mixevnt() { }

// Destructor
Mixevnt::~Mixevnt() { }

// F. Zhang 03-04-11
// access xbcsec in common
//double& Mixevnt::xbcsec(int n) {
//  init(); // check COMMON is initialized
//  if(n < 1 || n > lenXbcsec()) {
//    m_realdummy = -999.;
//    return m_realdummy;
//  }
//  return s_mixevnt->xbcsec[n-1];
//}

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
