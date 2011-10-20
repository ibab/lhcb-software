// $Id: Coloct.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Coloct
#include "LbBcVegPy/Coloct.h"

// set pointer to zero at start
Coloct::COLOCT* Coloct::s_coloct =0;

// Constructor
Coloct::Coloct() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Coloct::~Coloct() { }

// access ioctet in common
int& Coloct::ioctet() {
  init(); // check COMMON is initialized
  return s_coloct->ioctet;
}
