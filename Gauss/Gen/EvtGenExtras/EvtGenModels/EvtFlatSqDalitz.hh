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
// Module: EvtFlatSqDalitz.cc
//
// Description: B->3 body flat in square dalitz vairiables
// the square dalitz is the plane m12 theta12
//
// Modification history:
//
//    Aurelien Martens, LPNHE Paris, 23/06/2012    Module created
//
//
//------------------------------------------------------------------------

#ifndef EVTFLATSQDALITZ_HH
#define EVTFLATSQDALITZ_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtFlatSqDalitz: public  EvtDecayProb  {

public:

  EvtFlatSqDalitz() {}
  virtual ~EvtFlatSqDalitz();

  std::string getName();
  EvtDecayBase* clone();

  void init();
  void initProbMax();

  void decay(EvtParticle *p);
};

#endif

