// access GenXicc common Vegcross
#include "LbGenXicc/Vegcross.h"

// set pointer to zero at start
Vegcross::VEGCROSS* Vegcross::s_vegcross =0;

// Constructor
Vegcross::Vegcross() { }

// Destructor
Vegcross::~Vegcross() { }

// access vegsec in common
double& Vegcross::vegsec() {
  init(); // check COMMON is initialized
  return s_vegcross->vegsec;
}

// access vegerr in common
double& Vegcross::vegerr() {
  init(); // check COMMON is initialized
  return s_vegcross->vegerr;
}

// access iveggrade in common
int& Vegcross::iveggrade() {
  init(); // check COMMON is initialized
  return s_vegcross->iveggrade;
}
