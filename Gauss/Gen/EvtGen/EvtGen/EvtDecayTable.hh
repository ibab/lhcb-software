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
// Module: EvtGen/EvtDecayTable.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------

#ifndef EVTDECAYTABLE_HH
#define EVTDECAYTABLE_HH

#include "EvtGen/EvtDecayBase.hh"
#include "EvtGen/EvtId.hh"

//Class to read in and handle the decays available
//to EvtGen for each particle, and the model to be
//used for each one.

typedef EvtDecayBase* EvtDecayBasePtr;

class EvtDecayTable{

public:

  static int getNMode(int ipar);

  static void readDecayFile(const EvtString dec_name);

  static int findChannel(EvtId parent,EvtString model,int ndaug, EvtId *daugs,
		     int narg, double *args);
  
  static int inChannelList(EvtId parent, int ndaug, EvtId *daugs);

  static EvtDecayBase *GetDecayFunc(EvtParticle *);

  static void printSummary();

  static bool isJetSet(EvtId evtnum);

private:



};

#endif


