// plugs Gaudi RndmNumberSvc into B->3Pi model

//=============================================================================

#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"

#include <iostream>

extern Rndm::Numbers randgaudi;

#ifdef WIN32

extern "C"  double __stdcall CRANF( )

#else

extern "C" double cranf_( ) 

#endif

{
  return randgaudi();
}
//=============================================================================
