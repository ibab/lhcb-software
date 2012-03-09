// access GenXicc common Upcom
#include "LbGenXicc/Upcom.h"  // F. Zhang 01-04-11

// set pointer to zero at start
Upcom::UPCOM* Upcom::s_upcom =0;

// Constructor
Upcom::Upcom() { }

// Destructor
Upcom::~Upcom() { }

// access ecm in common
double& Upcom::ecm() {
  init(); // check COMMON is initialized
  return s_upcom->ecm;
}

// access pmxicc in common
double& Upcom::pmxicc() { // F. Zhang 01-04-11
  init(); // check COMMON is initialized
  return s_upcom->pmxicc;
}

// access pmb in common
double& Upcom::pmb() {
  init(); // check COMMON is initialized
  return s_upcom->pmb;
}

// access pmc in common
double& Upcom::pmc() {
  init(); // check COMMON is initialized
  return s_upcom->pmc;
}

// access fxicc in common
double& Upcom::fxicc() { // F. Zhang 01-04-11
  init(); // check COMMON is initialized
  return s_upcom->fxicc;
}

// access pmomup in common
double& Upcom::pmomup(int i, int j) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenPmomup() || j  < 1 || j  > depthPmomup()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_upcom->pmomup[j-1][i-1];
}

// access colmat in common
std::complex<double>& Upcom::colmat(int i, int j) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenColmat() || j  < 1 || j  > depthColmat()) {
    m_complexdummy = std::complex< double >(-999.,-999.);
    return m_complexdummy;
  }
  return s_upcom->colmat[j-1][i-1];
}

// access bundamp in common
std::complex<double>& Upcom::bundamp(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenBundamp()) {
    m_complexdummy = std::complex< double >(-999.,-999);
    return m_complexdummy;
  }
  return s_upcom->bundamp[n-1];
}

// access pmomzero in common
double& Upcom::pmomzero(int i, int j) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenPmomzero() || j  < 1 || j  > depthPmomzero()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_upcom->pmomzero[j-1][i-1];
}

