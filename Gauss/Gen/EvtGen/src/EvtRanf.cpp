//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: EvtRanf.cpp,v 1.3 2004-07-12 16:13:32 robbep Exp $
//
// Description:
//	subroutine evtranf_.
//      Provides FORTRAN calable interface to EvtRandom::Flat()
//      Can be used as EVTRANF instead of RANF in FORTRAN programs
//      or as evtranf_ instead of ranf_ in C/C++ programs.
//      No header file is provided, as C++ programs should use EvtRandom
//
//
//------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "EvtGenBase/EvtRandom.hh"
#ifdef WIN32
extern "C" {
  float __stdcall EVTRANF( ) 
  {
    return ( (float) EvtRandom::Flat() ) ;
  }
}
#else
extern "C" {
  float evtranf_( ) 
  {
    return ( (float) EvtRandom::Flat() ) ;
  }
}
#endif
