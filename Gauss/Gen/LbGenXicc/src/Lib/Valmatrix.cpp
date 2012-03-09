// access GenXicc common Valmatrix
#include "LbGenXicc/Valmatrix.h"

// set pointer to zero at start
Valmatrix::VALMATRIX* Valmatrix::s_valmatrix =0;

// Constructor
Valmatrix::Valmatrix() { }

// Destructor
Valmatrix::~Valmatrix() { }

// access cmfactor in common
double& Valmatrix::cmfactor() {
  init(); // check COMMON is initialized
  return s_valmatrix->cmfactor;
}
