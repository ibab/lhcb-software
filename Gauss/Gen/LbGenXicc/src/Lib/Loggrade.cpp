// access GenXicc common Loggrade
#include "LbGenXicc/Loggrade.h"

// set pointer to zero at start
Loggrade::LOGGRADE* Loggrade::s_loggrade =0;

// Constructor
Loggrade::Loggrade() { }

// Destructor
Loggrade::~Loggrade() { }

// access ievntdis in common
int& Loggrade::ievntdis() {
  init(); // check COMMON is initialized
  return s_loggrade->ievntdis;
}

// access igenerate in common
int& Loggrade::igenerate() {
  init(); // check COMMON is initialized
  return s_loggrade->igenerate;
}

// access ivegasopen in common
int& Loggrade::ivegasopen() {
  init(); // check COMMON is initialized
  return s_loggrade->ivegasopen;
}

// access igrade in common
int& Loggrade::igrade() {
  init(); // check COMMON is initialized
  return s_loggrade->igrade;
}

// access iusecurdir in common
int& Loggrade::iusecurdir() {
  init(); // check COMMON is initialized
  return s_loggrade->iusecurdir;
}




