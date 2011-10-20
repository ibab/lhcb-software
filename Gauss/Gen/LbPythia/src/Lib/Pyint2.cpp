// $Id: Pyint2.cpp,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
// access pythia common Pyint2
#include "LbPythia/Pyint2.h"

// set pointer to zero at start
Pyint2::PYINT2* Pyint2::s_pyint2 =0;

// Constructor
Pyint2::Pyint2() : m_dummy( 0 ) , m_realdummy( 0. )  { }
 
// Destructor
Pyint2::~Pyint2() { }

// access iset in common
int& Pyint2::iset(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenIset()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pyint2->iset[n-1];
}

// access kfpr in common
int& Pyint2::kfpr(int n, int i) {
  init(); // check COMMON is initialized
  if ( n < 1 || n > lenKfpr() || i <1 || i >depthKfpr()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pyint2->kfpr[i-1][n-1];
}

// access coef in common
double& Pyint2::coef(int n, int i) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenCoef() || i <1 || i > depthCoef()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pyint2->coef[i-1][n-1];
}

// access icol in common
int& Pyint2::icol(int n, int i ,int j) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenIcol() || i <1 || i > depthIcol() || j < 1 || 
     j > widthIcol() ) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pyint2->icol[j-1][i-1][n-1];
}

