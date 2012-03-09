// access GenXicc common Pydat1
#include "LbGenXicc/Counter.h"

// set pointer to zero at start
Counter::COUNTER* Counter::s_counter =0;

// Constructor
Counter::Counter() { }

// Destructor
Counter::~Counter() { }

// access ibcstate in common
//int& Counter::ibcstate() {
int& Counter::ixiccstate() {
  init(); // check COMMON is initialized
  return s_counter->ixiccstate;
}

// access nev in common
int& Counter::nev() {
  init(); // check COMMON is initialized
  return s_counter->nev;
}
