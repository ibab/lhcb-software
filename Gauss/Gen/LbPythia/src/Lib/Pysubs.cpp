// $Id: Pysubs.cpp,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
// access Pythia common Pysubs
#include "LbPythia/Pysubs.h"

#include <cstdlib>

// set pointer to zero at start
Pysubs::PYSUBS* Pysubs::s_pysubs =0;

// Constructor
Pysubs::Pysubs() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Pysubs::~Pysubs() { }

// access msel in common
int& Pysubs::msel() {
  init(); // check COMMON is initialized
  return s_pysubs->msel;
}

// access msub in common
int& Pysubs::msub(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMsub()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pysubs->msub[n-1];
}

// access kfin in common
int& Pysubs::kfin(int iside, int jflav) {
  init(); // check COMMON is initialized
  if(abs(jflav) > lenKfin()/2 || iside < 1 || iside > depthKfin()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pysubs->kfin[jflav+lenKfin()/2][iside-1];
}

// access ckin in common
double& Pysubs::ckin(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenCkin()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pysubs->ckin[n-1];
}

