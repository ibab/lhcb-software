// $Id: cpyr.cpp,v 1.2 2005-11-08 00:07:34 robbep Exp $
//-----------------------------------------------------------------------------
// 06/05/2002 : Witold Pokorski
//-----------------------------------------------------------------------------

// plugs Gaudi RndmNumberSvc into Pythia

//=============================================================================

#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"

Rndm::Numbers randgaudi;

#ifdef WIN32
extern "C"  double __stdcall CPYR(int*)
#else
extern "C" double cpyr_(int*)
#endif
{  
  return randgaudi();
}
//=============================================================================
