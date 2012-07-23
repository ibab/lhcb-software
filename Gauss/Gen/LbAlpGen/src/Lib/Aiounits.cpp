// access AlpGen common Aiounits
#include "LbAlpGen/Aiounits.h"

#include <cstdlib>

// set pointer to zero at start
Aiounits::AIOUNITS* Aiounits::s_aiounits =0;

// Constructor
Aiounits::Aiounits()  { }

// Destructor
Aiounits::~Aiounits() { }

// access niosta in common
int& Aiounits::niosta( ) {
  init() ;
  return s_aiounits -> niosta ;
}

// access niopar in common
int& Aiounits::niopar( ) {
  init() ;
  return s_aiounits -> niopar ;
}

// access niowgt in common
int& Aiounits::niowgt( ) {
  init() ;
  return s_aiounits -> niowgt ;
}

// access niounw in common
int& Aiounits::niounw( ) {
  init() ;
  return s_aiounits -> niounw ;
}


