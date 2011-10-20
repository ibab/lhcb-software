// $Id: Pydatr.cpp,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
// access pythia common Pydatr
#include "LbPythia/Pydatr.h"

// set pointer to zero at start
Pydatr::PYDATR* Pydatr::s_pydatr =0;

// Constructor
Pydatr::Pydatr() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Pydatr::~Pydatr() { }

// access mrpy in common
int& Pydatr::mrpy(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMrpy()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pydatr->mrpy[n-1];
}

// access rrpy in common
double& Pydatr::rrpy(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenRrpy()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pydatr->rrpy[n-1];
}

