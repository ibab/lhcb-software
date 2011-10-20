// $Id: Pypars.cpp,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
// access pythia common Pypars
#include "LbPythia/Pypars.h"
// set pointer to zero at start
Pypars::PYPARS* Pypars::s_pypars =0;

// Constructor
Pypars::Pypars() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Pypars::~Pypars() { }

// access mstp in common
int& Pypars::mstp(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMstp()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pypars->mstp[n-1];
}

// access parp in common
double& Pypars::parp(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenParp()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pypars->parp[n-1];
}

// access msti in common
int& Pypars::msti(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMsti()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pypars->msti[n-1];
}

// access pari in common
double& Pypars::pari(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenPari()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pypars->pari[n-1];
}

