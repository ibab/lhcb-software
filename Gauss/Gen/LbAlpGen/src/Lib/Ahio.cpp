// access AlpGen common Ahio
#include "LbAlpGen/Ahio.h"

#include <cstdlib>

// set pointer to zero at start
Ahio::AHIO* Ahio::s_ahio =0;

// Constructor
Ahio::Ahio()  { }

// Destructor
Ahio::~Ahio() { }

// access nunit in common
int& Ahio::nunit( ) {
  init() ;
  return s_ahio -> nunit ;
}

// access nunitout in common
int& Ahio::nunitout( ) {
  init() ;
  return s_ahio -> nunitout ;
}

