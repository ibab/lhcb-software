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
// Module: EvtGen/EvtRandom.hh
//
// Description:Class to generate random numbers. Single member
//             function random is expected to return a random
//             number in the range ]0..1[.
//
// Modification history:
//
//    RYD     December 25, 1999         Module created
//
//------------------------------------------------------------------------

#ifndef EVTRANDOMENGINE_HH
#define EVTRANDOMENGINE_HH

class EvtRandomEngine{

public:

  virtual double random();

private:

};

#endif


