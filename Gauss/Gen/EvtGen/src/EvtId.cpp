//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtId.cc
//
// Description: Class for particle Id used in EvtGen.
//
// Modification history:
//
//    RYD     May 26, 1998        Module created
//
//------------------------------------------------------------------------
//

#include "EvtGen/EvtId.hh"
#include <iostream>


ostream& operator<<(ostream& s, const EvtId& id){

  s<<"(Id="<<id._id<<" Alias="<<id._alias<<")";

  return s;

}




