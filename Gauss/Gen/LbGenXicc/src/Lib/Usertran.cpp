// access GenXicc common Usertran ! F. Zhang 01-04-11
#include "LbGenXicc/Usertran.h"

// set pointer to zero at start
Usertran::USERTRAN* Usertran::s_usertran =0;

// Constructor
Usertran::Usertran() { }

// Destructor
Usertran::~Usertran() { }

// F. Zhang 01-04-11
// access ishower in common
//int& Usertran::ishower() {
//  init(); // check COMMON is initialized
//  return s_usertran->ishower;
//}

// access idpp in common
int& Usertran::idpp() {
  init(); // check COMMON is initialized
  return s_usertran->idpp;
}



