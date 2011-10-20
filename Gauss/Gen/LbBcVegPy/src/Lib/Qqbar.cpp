// $Id: Qqbar.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Qqbar
#include "LbBcVegPy/Qqbar.h"

// set pointer to zero at start
Qqbar::QQBAR* Qqbar::s_qqbar =0;

// Constructor
Qqbar::Qqbar() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Qqbar::~Qqbar() { }

// access iqqbar in common
int& Qqbar::iqqbar() {
  init(); // check COMMON is initialized
  return s_qqbar->iqqbar;
}

// access iqcode in common
int& Qqbar::iqcode() {
  init(); // check COMMON is initialized
  return s_qqbar->iqcode;
}



