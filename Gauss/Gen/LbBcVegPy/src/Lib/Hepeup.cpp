// $Id: Hepeup.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Hepeup
#include "LbBcVegPy/Hepeup.h"

// set pointer to zero at start
Hepeup::HEPEUP* Hepeup::s_hepeup =0;

// Constructor
Hepeup::Hepeup() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Hepeup::~Hepeup() { }

// access nup in common
int& Hepeup::nup() {
  init(); // check COMMON is initialized
  return s_hepeup->nup;
}

// access idprup in common
int& Hepeup::idprup() {
  init(); // check COMMON is initialized
  return s_hepeup->idprup;
}

// access xwgtup in common
double& Hepeup::xwgtup() {
  init(); // check COMMON is initialized
  return s_hepeup->xwgtup;
}

// access scalup in common
double& Hepeup::scalup() {
  init(); // check COMMON is initialized
  return s_hepeup->scalup;
}

// access aqedup in common
double& Hepeup::aqedup() {
  init(); // check COMMON is initialized
   return s_hepeup->aqedup;
}

// access aqcdup in common
double& Hepeup::aqcdup() {
  init(); // check COMMON is initialized
  return s_hepeup->aqcdup;
}

//access idup in common
int& Hepeup::idup(int n){
  init(); // check COMMON is initialized
  if( n<1 || n> lenIdup() ){
    m_dummy=-999;
    return m_dummy;
  }
  return s_hepeup->idup[n-1];
}

// access istup in common
int& Hepeup::istup(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenIstup() ) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_hepeup->istup[n-1];
}

// access mothup in common
int& Hepeup::mothup(int i, int j) {
  init(); // check COMMON is initialized
  if( i < 1 || j > lenMothup() || j  < 1 || i > depthMothup()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_hepeup->mothup[j-1][i-1];
}

// access icolup in common
int& Hepeup::icolup(int i, int j) {
  init(); // check COMMON is initialized
  if( i < 1 || j > lenIcolup() || j  < 1 || i  > depthIcolup()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_hepeup->icolup[j-1][i-1];
}

// access pup in common
double& Hepeup::pup(int i, int j) {
  init(); // check COMMON is initialized
  if( i < 1 || j > lenPup() || j  < 1 || i  > depthPup()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_hepeup->pup[j-1][i-1];
}

// access vtimup in common
double& Hepeup::vtimup(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenVtimup() ) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_hepeup->vtimup[n-1];
}

// access spinup in common
double& Hepeup::spinup(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenSpinup() ) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_hepeup->spinup[n-1];
}





