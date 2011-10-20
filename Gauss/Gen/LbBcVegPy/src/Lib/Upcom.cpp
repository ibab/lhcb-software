// $Id: Upcom.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Upcom
#include "LbBcVegPy/Upcom.h"

// set pointer to zero at start
Upcom::UPCOM* Upcom::s_upcom =0;

// Constructor
Upcom::Upcom() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Upcom::~Upcom() { }

// access ecm in common
double& Upcom::ecm() {
  init(); // check COMMON is initialized
  return s_upcom->ecm;
}

// access pmbc in common
double& Upcom::pmbc() {
  init(); // check COMMON is initialized
  return s_upcom->pmbc;
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

// access fbcc in common
double& Upcom::fbcc() {
  init(); // check COMMON is initialized
  return s_upcom->fbcc;
}

// access pmomup in common
double& Upcom::pmomup(int i, int j) {
  init(); // check COMMON is initialized
  if( i < 1 || j > lenPmomup() || j  < 1 || i  > depthPmomup()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_upcom->pmomup[j-1][i-1];
}

// access colmat in common
std::complex<double>& Upcom::colmat(int i, int j) {
  init(); // check COMMON is initialized
  if( i < 1 || j > lenColmat() || j  < 1 || i  > depthColmat()) {
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
  if( i < 1 || j > lenPmomzero() || j  < 1 || i  > depthPmomzero()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_upcom->pmomzero[j-1][i-1];
}

