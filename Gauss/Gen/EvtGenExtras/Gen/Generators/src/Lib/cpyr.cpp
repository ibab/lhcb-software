// $Id: cpyr.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
//-----------------------------------------------------------------------------
// 06/05/2002 : Witold Pokorski
//-----------------------------------------------------------------------------

// plugs Gaudi RndmNumberSvc into Pythia

//=============================================================================

#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"
#include "Generators/RandomForGenerator.h"

#ifdef WIN32
extern "C"  double __stdcall CPYR(int*)
#else
extern "C" double cpyr_(int*)
#endif
{  
  return (RandomForGenerator::getNumbers())() ;
}
//=============================================================================
