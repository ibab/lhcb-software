// $Id: Intinif.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Intinif
#include "LbBcVegPy/Intinif.h"

// set pointer to zero at start
Intinif::INTINIF* Intinif::s_intinif =0;

// Constructor
Intinif::Intinif() : m_dummy( 0 ) , m_realdummy( 0. )  { }

// Destructor
Intinif::~Intinif() { }

// access iinif in common
int& Intinif::iinif() {
  init(); // check COMMON is initialized
  return s_intinif->iinif;
}
