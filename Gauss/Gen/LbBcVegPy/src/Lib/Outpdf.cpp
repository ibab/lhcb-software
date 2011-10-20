// $Id: Outpdf.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Outpdf
#include "LbBcVegPy/Outpdf.h"

// set pointer to zero at start
Outpdf::OUTPDF* Outpdf::s_outpdf =0;

// Constructor
Outpdf::Outpdf() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Outpdf::~Outpdf() { }

// access ioutpdf in common
int& Outpdf::ioutpdf() {
  init(); // check COMMON is initialized
  return s_outpdf->ioutpdf;
}

// access ipdfnum in common
int& Outpdf::ipdfnum() {
  init(); // check COMMON is initialized
  return s_outpdf->ipdfnum;
}



