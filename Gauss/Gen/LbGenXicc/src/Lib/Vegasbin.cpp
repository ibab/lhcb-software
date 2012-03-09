// access GenXicc common VEGASBIN 
#include "LbGenXicc/Vegasbin.h"

// set pointer to zero at start
Vegasbin::VEGASBIN* Vegasbin::s_vegasbin =0;

// Constructor
Vegasbin::Vegasbin() { }

// Destructor
Vegasbin::~Vegasbin() { }

// access nvbin in common
int& Vegasbin::nvbin() {
  init(); // check COMMON is initialized
  return s_vegasbin->nvbin;
}
