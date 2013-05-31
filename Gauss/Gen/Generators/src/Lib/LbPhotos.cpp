// Include files
// local
#include "Generators/LbPhotos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LbPhotos
//
// 2013-05-31 : Patrick Robbe
//-----------------------------------------------------------------------------

extern "C" {
#ifdef WIN32
  void __stdcall SETPHOTOSOUTPUTUNIT( int * ) ;
#else
  void setphotosoutputunit_( int * ) ;
#endif
}

void LbPhotos::setOutputUnit( int outputUnit ) {
#ifdef WIN32
  SETPHOTOSOUTPUTUNIT( &outputUnit ) ;
#else
  setphotosoutputunit_( &outputUnit ) ;
#endif
}
