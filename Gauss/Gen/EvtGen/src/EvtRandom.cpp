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
// Module: EvtRandom.cc
//
// Description: routines to get random numbers from 
//              random number generator.
//
// Modification history:
//
//    DJL/RYD   September 25, 1996           Module created
//
//------------------------------------------------------------------------
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "EvtGen/EvtRandomEngine.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtReport.hh"


EvtRandomEngine* EvtRandom::_randomEngine=0;

void EvtRandom::setRandomEngine(EvtRandomEngine* randomEngine){
  _randomEngine=randomEngine;
}


double EvtRandom::random(){

  if (_randomEngine==0){
    report(ERROR,"EvtGen") <<"No random engine available in "
			   <<"EvtRandom::random()."<<std::endl;
    ::abort();
  }

  return _randomEngine->random();

}


// Random number routine to generate numbers between
// min and max.  By djl on July 27, 1995.
double EvtRandom::Flat( double min, double max){

  if ( min > max ) {
    report(ERROR,"EvtGen") << "min>max in EvtRandomm:;Flat(min,max)"<<std::endl;
    ::abort();
  }

  return EvtRandom::random()*( max - min )+min;

} 

double EvtRandom::Flat(double max){

  return max*EvtRandom::random();

} 

double EvtRandom::Flat(){

  return EvtRandom::random();

} 



