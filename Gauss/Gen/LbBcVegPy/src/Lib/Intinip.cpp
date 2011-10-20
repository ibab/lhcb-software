// $Id: Intinip.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Intinip
#include "LbBcVegPy/Intinip.h"

// set pointer to zero at start
Intinip::INTINIP* Intinip::s_intinip =0;

// Constructor
Intinip::Intinip() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Intinip::~Intinip() { }

// access iinip in common
int& Intinip::iinip() {
  init(); // check COMMON is initialized
  return s_intinip->iinip;
}
