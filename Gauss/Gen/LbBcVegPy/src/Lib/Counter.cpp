// $Id: Counter.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Pydat1
#include "LbBcVegPy/Counter.h"

// set pointer to zero at start
Counter::COUNTER* Counter::s_counter =0;

// Constructor
Counter::Counter() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Counter::~Counter() { }

// access ibcstate in common
int& Counter::ibcstate() {
  init(); // check COMMON is initialized
  return s_counter->ibcstate;
}

// access nev in common
int& Counter::nev() {
  init(); // check COMMON is initialized
  return s_counter->nev;
}
