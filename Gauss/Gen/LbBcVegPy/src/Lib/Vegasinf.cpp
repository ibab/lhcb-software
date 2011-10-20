// $Id: Vegasinf.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Vegasinf
#include "LbBcVegPy/Vegasinf.h"

// set pointer to zero at start
Vegasinf::VEGASINF* Vegasinf::s_vegasinf =0;

// Constructor
Vegasinf::Vegasinf() : m_dummy( 0 ) , m_realdummy( 0. ) { }

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



