// $Id: Genefull.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Genefull
#include "LbBcVegPy/Genefull.h"

// set pointer to zero at start
Genefull::GENEFULL* Genefull::s_genefull =0;

// Constructor
Genefull::Genefull() { }

// Destructor
Genefull::~Genefull() { }

// access generate in common
bool& Genefull::generate() {
  init(); // check COMMON is initialized
  return s_genefull->generate;
}
