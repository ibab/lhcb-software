// access pythia common Pyint5
#include "LbPythia/Pyint5.h"

// set pointer to zero at start
Pyint5::PYINT5 * Pyint5::s_pyint5 = 0 ;

// Constructor
Pyint5::Pyint5() : m_dummy( 0 ) , m_realdummy( 0. )  { }
 
// Destructor
Pyint5::~Pyint5() { }

// access ngen in common
int& Pyint5::ngen( int n , int i ) {
  init(); // check COMMON is initialized
  if ( n < 0 || n >= lenNgen() || i < 1 || i > depthNgen()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pyint5->ngen[i-1][n];
}

// access xsec in common
double& Pyint5::xsec(int n, int i) {
  init(); // check COMMON is initialized
  if(n < 0 || n >= lenXsec() || i <1 || i > depthXsec()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pyint5->xsec[i-1][n];
}

