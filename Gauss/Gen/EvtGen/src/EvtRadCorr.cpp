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
// Module: EvtRadCorr.cc
//
// Description: RadCorr interface for EvtGen
//              
//
// Modification history:
//
//    Lange     April 27, 2002  - Created
//
//------------------------------------------------------------------------
//

#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <stdlib.h>
#include <iostream>
#include "EvtGenBase/EvtAbsRadCorr.hh"
#include "EvtGenBase/EvtRadCorr.hh"
#include "EvtGenBase/EvtReport.hh"


EvtAbsRadCorr* EvtRadCorr::_isrEngine=0;
bool EvtRadCorr::_alwaysRadCorr=false;
bool EvtRadCorr::_neverRadCorr=false;

EvtRadCorr::EvtRadCorr() {
  _isrEngine=0;
  _alwaysRadCorr=false;
  _neverRadCorr=false;
}

EvtRadCorr::~EvtRadCorr() {
  if ( _isrEngine ) delete _isrEngine;
  _isrEngine=0;
}

void EvtRadCorr::setRadCorrEngine(EvtAbsRadCorr* isrEngine){
  _isrEngine=isrEngine;
}


void EvtRadCorr::doRadCorr(EvtParticle *p){

  if (_isrEngine==0){
    report(ERROR,"EvtGen") <<"No RadCorr model available in "
			   <<"EvtRadCorr::doRadCorr()."<<std::endl;
    ::abort();
  }

  if ( !_neverRadCorr) _isrEngine->doRadCorr(p);
  return;
}


bool EvtRadCorr::alwaysRadCorr() {return _alwaysRadCorr;}
bool EvtRadCorr::neverRadCorr() {return _neverRadCorr;}

void EvtRadCorr::setAlwaysRadCorr() { _alwaysRadCorr=true; _neverRadCorr=false;}
void EvtRadCorr::setNeverRadCorr() { _alwaysRadCorr=false; _neverRadCorr=true;}
void EvtRadCorr::setNormalRadCorr() {_alwaysRadCorr=false; _neverRadCorr=false;}







