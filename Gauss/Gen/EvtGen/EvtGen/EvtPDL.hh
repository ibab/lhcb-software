//--------------------------------------------------------------------------
// 
// Environment: 
// This software is part of the EvtGen package developed jointly 
// for the BaBar and CLEO collaborations.  If you use all or part 
// of it, please give an appropriate acknowledgement.
// 
// Copyright Information: See EvtGen/COPYRIGHT 
// Copyright (C) 1998 Caltech, UCSB 
// 
// Module: EvtGen/EvtPDL.hh 
// 
// Description:Class to keep track of particle properties.  
// 
// Modification history: 
//
// DJL/RYD September 25, 1996 Module created 
//
//------------------------------------------------------------------------

#ifndef EVTPDL_HH
#define EVTPDL_HH

#include "EvtGen/EvtParticleNum.hh"
#include "EvtGen/EvtPartProp.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtSpinType.hh"
#include "EvtGen/EvtStringHash.hh"

const int MAX_PART = 1600;
const int SPIN_NAME_LENGTH = 100;

class EvtPDL {

public:

  EvtPDL();  

  ~EvtPDL();

  void readPDT(const EvtString fname);

  
  static double getNominalMass(EvtId i ); 
  static double getMass(EvtId i );

  static double getMaxMass(EvtId i );
  static double getMinMass(EvtId i );
  static double getWidth(EvtId i );
  static double getctau(EvtId i );
  static int getStdHep(EvtId id );
  static int getLundKC(EvtId id );
  static EvtId evtIdFromStdHep(int stdhep );
  static EvtId chargeConj(EvtId id );
  static int chg3(EvtId i );
  static EvtSpinType::spintype getSpinType(EvtId i );
  static EvtId getId(const EvtString& name );
  static EvtString name(EvtId i);
  static void alias(EvtId num,const EvtString& newname);
  static void aliasChgConj(EvtId a,EvtId abar);
  static int entries() { return MAX_PART;}
  static void reSetMass(EvtId i, double mass);
  static void reSetWidth(EvtId i, double width);

private:

  void setUpConstsPdt();

  static int    _lastAlias;
  static int    _nentries;

  static EvtPartProp *_partlist;
  static EvtStringHash<EvtPartProp>* _particleNameHash;
  
}; // EvtPDL.h

#endif


