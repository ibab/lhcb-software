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
// Module: EvtGen/EvtGen.hh
//
// Description:Main class to provide user interface to EvtGen.
//
// Modification history:
//
//    RYD     March 24, 1998     Module created
//
//    DJL     August 10, 1998    Additional Event member function added
//
//    RYD     December 25, 1999  Any application using EvtGen will need
//                               to instantiate an instance of this class
//                               and hold on to it untill done generating
//                               events. This class will now hold data used
//                               for the lifetime of the generator.
//------------------------------------------------------------------------

#ifndef EVTGEN_HH
#define EVTGEN_HH

#include "EvtGenBase/EvtPDL.hh"

class EvtParticle;
class EvtRandomEngine;
class HepLorentzVector;
class EvtVector4R;
class EvtStdHep;
class EvtSpinDensity;
class EvtAbsRadCorr;

class EvtGen{

public:

  EvtGen(const char* const decayName,const char* const pdtTableName,
	 EvtRandomEngine* randomEngine=0, EvtAbsRadCorr *isrEngine=0);
  ~EvtGen();

  void readUDecay(const char* const udecay_name);

  void generateDecay(int stdhepid, EvtVector4R P, EvtVector4R D,
		     EvtStdHep *evtStdHep,EvtSpinDensity *spinDensity=0);

  void generateDecay(EvtParticle *p);

  //These two methods are obsolete
  void generateEvent(int stdhepid, HepLorentzVector P, HepLorentzVector D);
  void generateEvent(EvtParticle *p,HepLorentzVector D);

  
private:

  EvtPDL _pdl;

};



#endif

