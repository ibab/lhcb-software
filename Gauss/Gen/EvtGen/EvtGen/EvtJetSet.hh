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
// Module: EvtGen/EvtJetSet.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTJETSET_HH
#define EVTJETSET_HH

#include "EvtGen/EvtDecayIncoherent.hh"
#include "EvtGen/EvtParticle.hh"

typedef EvtDecayBase* EvtDecayBasePtr;

class EvtJetSet:public  EvtDecayIncoherent  {

public:

  EvtJetSet();
  virtual ~EvtJetSet();

  void getName(EvtString& name);
  EvtDecayBase* clone();
  void decay(EvtParticle *p); 

  EvtString commandName();
  void command(EvtString cmd);

  void init();

  void initProbMax();

  //initialize jetset; sets up decay table and
  //paramters. Static so it can be invoked from
  //from EvtJscont.
  static void jetSetInit();

private:

  void store(EvtDecayBase* jsdecay);
  void fixPolarizations(EvtParticle* p);
  static void MakeJetSetFile(char* fname);
  static void WriteJetSetParticle(ofstream &outdec,EvtId ipar,EvtId iparname,int &first);
  static void WriteJetSetEntryHeader(ofstream &outdec, int lundkc,
			       EvtId evtnum,EvtString name,
			       int chg, int cchg, int spin2,double mass,
			       double width, double maxwidth,double ctau,
			       int stable,double rawbrfrsum);

  static int njetsetdecays;
  static EvtDecayBasePtr* jetsetdecays;
  static int ntable;

  static int ncommand;
  static int lcommand;
  static EvtString* commands;

};

#endif




