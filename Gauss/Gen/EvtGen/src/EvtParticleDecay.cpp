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
// Module: EvtDecayParm.cc
//
// Description: Store decay parameters for one decay.
//
// Modification history:
//
//    RYD     April 5, 1997         Module created
//
//------------------------------------------------------------------------
//
#include <iostream>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include "EvtGen/EvtParticleDecay.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtVectorT.hh"

void EvtParticleDecay::printSummary(){

  if (_decay!=0) _decay->printSummary();

}

void EvtParticleDecay::chargeConj(EvtParticleDecay *decay){

  _brfrsum=decay->_brfrsum;
  _massmin=decay->_massmin;

  _decay=decay->_decay->clone();

  int ndaug=decay->_decay->getNDaug();
  int narg=decay->_decay->getNArg();
  double brfr=decay->_decay->getBranchingFraction();
  EvtString name;
  decay->_decay->getName(name);
  EvtId ipar=EvtPDL::chargeConj(decay->_decay->getParentId());
  int i;
  EvtId* daug=new EvtId[ndaug];
  for(i=0;i<ndaug;i++){
    daug[i]=EvtPDL::chargeConj(decay->_decay->getDaug(i));
  }
  //Had to add 1 to make sure the vector is not empty!
  EvtVectorT<double> args(narg+1);
  for(i=0;i<narg;i++){
    args[i]=decay->_decay->getArg(i);
  }

  _decay->saveDecayInfo(ipar,ndaug,daug,narg,args,name,brfr);

  if (decay->_decay->getPHOTOS()) _decay->setPHOTOS();

  delete [] daug;

}







