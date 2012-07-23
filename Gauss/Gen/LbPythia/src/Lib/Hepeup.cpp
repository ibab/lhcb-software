// access pythia common Hepeup
#include "LbPythia/Hepeup.h"

// set pointer to zero at start
Hepeup::HEPEUP* Hepeup::s_hepeup = 0 ;

// Constructor
Hepeup::Hepeup() : m_dummy( 0 ) , m_realdummy( 0. )  { }

// Destructor
Hepeup::~Hepeup() { }

int& Hepeup::nup() {
  init() ;
  return s_hepeup -> nup ;
}

int& Hepeup::idprup() {
  init() ;
  return s_hepeup -> idprup ;
}

int& Hepeup::idup( int n ) {
  init() ;
  if ( n < 1 || n > lenHepeup() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  return s_hepeup -> idup[ n - 1 ] ;
}

int& Hepeup::istup( int n ) {
  init() ;
  if ( n < 1 || n > lenHepeup() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  return s_hepeup -> istup[ n - 1 ] ;  
}

int& Hepeup::mothup( int i , int n ) {
  init() ;
  if ( i < 1 || i > 2 || n < 1 || n > lenHepeup() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  return s_hepeup -> mothup[ n - 1 ][ i - 1 ] ;
}

int& Hepeup::icolup( int i , int n ) {
  init() ;
  if ( i < 1 || i > 2 || n < 1 || n > lenHepeup() ) {
    m_dummy = -999 ;
    return m_dummy ;
  }
  return s_hepeup -> icolup[ n - 1 ][ i - 1 ] ;
}
   
double& Hepeup::xwgtup() {
  init() ;
  return s_hepeup -> xwgtup ;
}

double& Hepeup::scalup() {
  init() ;
  return s_hepeup -> scalup ;
}

double& Hepeup::aqedup() {
  init() ;
  return s_hepeup -> aqedup ;
}

double& Hepeup::aqcdup() {
  init() ;
  return s_hepeup -> aqcdup ;
}

double& Hepeup::pup( int i , int n ) {
  init() ;
  if ( i < 1 || i > 2 || n < 1 || n > lenHepeup() ) {
    m_realdummy = -999. ;
    return m_realdummy ;
  }
  return s_hepeup -> pup[ n - 1 ][ i - 1 ];
}

double& Hepeup::vtimup( int n ) {
  init() ;
  if ( n < 1 || n > lenHepeup() ) {
    m_realdummy = -999. ;
    return m_realdummy ;
  }
  return s_hepeup -> vtimup[ n - 1 ] ;
}

double& Hepeup::spinup( int n ) {
  init() ;
  if ( n < 1 || n > lenHepeup() ) {
    m_realdummy = -999. ;
    return m_realdummy ;
  }
  return s_hepeup -> spinup[ n - 1 ] ;
}
