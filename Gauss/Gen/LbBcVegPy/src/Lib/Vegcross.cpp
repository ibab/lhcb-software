// $Id: Vegcross.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Vegcross
#include "LbBcVegPy/Vegcross.h"

// set pointer to zero at start
Vegcross::VEGCROSS* Vegcross::s_vegcross =0;

// Constructor
Vegcross::Vegcross() : m_dummy( 0 ) , m_realdummy( 0. ) { }

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
