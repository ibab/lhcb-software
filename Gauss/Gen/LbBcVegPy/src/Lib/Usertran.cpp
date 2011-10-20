// $Id: Usertran.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Usertran
#include "LbBcVegPy/Usertran.h"

// set pointer to zero at start
Usertran::USERTRAN* Usertran::s_usertran =0;

// Constructor
Usertran::Usertran() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Usertran::~Usertran() { }

// access ishower in common
int& Usertran::ishower() {
  init(); // check COMMON is initialized
  return s_usertran->ishower;
}

// access idpp in common
int& Usertran::idpp() {
  init(); // check COMMON is initialized
  return s_usertran->idpp;
}



