// Include files
// local
#include "Generators/LbTaula.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LbTaula
//
// 2013-05-31 : Patrick Robbe
//-----------------------------------------------------------------------------

extern "C" {
#ifdef WIN32
  void __stdcall SETTAULAOUTPUTUNIT( int * ) ;
#else
  void settaulaoutputunit_( int * ) ;
#endif
}

void LbTaula::setOutputUnit( int outputUnit ) {
#ifdef WIN32
  SETTAULAOUTPUTUNIT( &outputUnit ) ;
#else
  settaulaoutputunit_( &outputUnit ) ;
#endif
}
