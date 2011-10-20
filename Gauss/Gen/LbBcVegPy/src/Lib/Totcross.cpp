// $Id: Totcross.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Totcross
#include "LbBcVegPy/Totcross.h"

// set pointer to zero at start
Totcross::TOTCROSS* Totcross::s_totcross =0;

// Constructor
Totcross::Totcross() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Totcross::~Totcross() { }

// access appcross in common
double& Totcross::appcross() {
  init(); // check COMMON is initialized
  return s_totcross->appcross;
}
