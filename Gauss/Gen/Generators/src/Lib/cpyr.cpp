// $Id: cpyr.cpp,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
//-----------------------------------------------------------------------------
// 06/05/2002 : Witold Pokorski
//-----------------------------------------------------------------------------

// plugs Gaudi RndmNumberSvc into Pythia

//=============================================================================

#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"

extern Rndm::Numbers randgaudi;

#ifdef WIN32
extern "C"  double __stdcall CPYR(int*)
#else
extern "C" double cpyr_(int*)
#endif
{  
  return randgaudi();
}
//=============================================================================
