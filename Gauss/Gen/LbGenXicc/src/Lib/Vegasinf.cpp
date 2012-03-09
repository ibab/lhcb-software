// access GenXicc common Vegasinf
#include "LbGenXicc/Vegasinf.h"

// set pointer to zero at start
Vegasinf::VEGASINF* Vegasinf::s_vegasinf =0;

// Constructor
Vegasinf::Vegasinf() { }

// Destructor
Vegasinf::~Vegasinf() { }

// access number in common
int& Vegasinf::number() {
  init(); // check COMMON is initialized
  return s_vegasinf->number;
}

// access nitmx in common
int& Vegasinf::nitmx() {
  init(); // check COMMON is initialized
  return s_vegasinf->nitmx;
}



