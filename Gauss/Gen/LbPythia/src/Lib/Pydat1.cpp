// $Id: Pydat1.cpp,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
// access pythia common Pydat1
#include "LbPythia/Pydat1.h"

// set pointer to zero at start
Pydat1::PYDAT1* Pydat1::s_pydat1 =0;

// Constructor
Pydat1::Pydat1() { }

// Destructor
Pydat1::~Pydat1() { }

// access mstu in common
int& Pydat1::mstu(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMstu()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pydat1->mstu[n-1];
}

// access paru in common
double& Pydat1::paru(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenParu()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pydat1->paru[n-1];
}

// access mstj in common
int& Pydat1::mstj(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMstj()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pydat1->mstj[n-1];
}

// access parj in common
double& Pydat1::parj(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenParj()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pydat1->parj[n-1];
}
