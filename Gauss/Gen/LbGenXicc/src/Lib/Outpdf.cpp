// access GenXicc common Outpdf ! F. Zhang 01-04-11
#include "LbGenXicc/Outpdf.h"

// set pointer to zero at start
Outpdf::OUTPDF* Outpdf::s_outpdf =0;

// Constructor
Outpdf::Outpdf() { }

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



