// access GenXicc common MTYPEOFXI 
#include "LbGenXicc/Mtypeofxi.h"

// set pointer to zero at start
Mtypeofxi::MTYPEOFXI* Mtypeofxi::s_mtypeofxi =0;

// Constructor
Mtypeofxi::Mtypeofxi() { }

// Destructor
Mtypeofxi::~Mtypeofxi() { }

// access mgenxi in common
int& Mtypeofxi::mgenxi() {
  init(); // check COMMON is initialized
  return s_mtypeofxi->mgenxi;
}
