// $Id: Loggrade.cpp,v 1.2 2009-07-23 19:45:37 jhe Exp $
// access BcGen common Loggrade
#include "LbBcVegPy/Loggrade.h"

// set pointer to zero at start
Loggrade::LOGGRADE* Loggrade::s_loggrade =0;

// Constructor
Loggrade::Loggrade() : m_dummy( 0 ) , m_realdummy( 0. ) { }

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



