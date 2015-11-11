// access GenXicc common Subopen
#include "LbGenXicc/Subopen.h"

// set pointer to zero at start
Subopen::SUBOPEN* Subopen::s_subopen =0;

// Constructor
Subopen::Subopen() { }

// Destructor
Subopen::~Subopen() { }

// access subfactor in common
double& Subopen::subfactor() {
  init(); // check COMMON is initialized
  return s_subopen->subfactor;
}

// access subenergy in common
double& Subopen::subenergy() {
  init(); // check COMMON is initialized
  return s_subopen->subenergy;
}

// access isubonly in common
int& Subopen::isubonly() {
  init(); // check COMMON is initialized
  return s_subopen->isubonly;
}

// access ichange in common
int& Subopen::ichange() {
  init(); // check COMMON is initialized
  return s_subopen->ichange;
}

// access iconsbarnum in common
int& Subopen::iconsbarnum() {
  init(); // check COMMON is initialized
  return s_subopen->iconsbarnum;
}






