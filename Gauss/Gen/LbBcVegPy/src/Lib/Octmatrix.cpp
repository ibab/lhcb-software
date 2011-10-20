// $Id: Octmatrix.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Octmatrix
#include "LbBcVegPy/Octmatrix.h"

// set pointer to zero at start
Octmatrix::OCTMATRIX* Octmatrix::s_octmatrix =0;

// Constructor
Octmatrix::Octmatrix() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Octmatrix::~Octmatrix() { }

// access coeoct in common
double& Octmatrix::coeoct() {
  init(); // check COMMON is initialized
  return s_octmatrix->coeoct;
}
