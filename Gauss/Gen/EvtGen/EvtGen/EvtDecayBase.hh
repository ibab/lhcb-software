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
// Module: EvtGen/EvtDecayBase.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTDECAYBASE_HH
#define EVTDECAYBASE_HH

#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtVectorT.hh"
#include "EvtGen/EvtSpinType.hh"
#include <assert.h>

class EvtParticle;
class EvtDecayBase{

 public:

  //These pure virtual methods has to be implemented
  //by any derived class
  virtual void getName(EvtString& name)=0;
  virtual void decay(EvtParticle *p)=0;
  virtual void makeDecay(EvtParticle *p)=0;
  virtual EvtDecayBase* clone()=0;


  //These virtual methods can be implemented by the 
  //derived class to implement nontrivial functionality.
  virtual void init();
  virtual void initProbMax();
  virtual EvtString commandName();
  virtual void command(EvtString cmd);


  double getProbMax( double prob );
  double resetProbMax( double prob );

  EvtDecayBase();
  virtual ~EvtDecayBase();

  EvtId getParentId() {return _parent;}
  double getBranchingFraction() {return _brfr;}
  void disableCheckQ() {_chkCharge=0;};
  void checkQ();
  int getNDaug() {return _ndaug;}
  EvtId* getDaugs() {return _daug;}
  EvtId getDaug(int i) {return _daug[i];}
  int getNArg() {return _narg;}
  int getPHOTOS() {return _photos;}
  void setPHOTOS() {_photos=1;}
  void setVerbose() {_verbose=1;}
  void setSummary() {_summary=1;}
  double* getArgs() {return _args;}
  double getArg(int j) {return _args[j];}
  EvtString   getModelName() {return _modelname; }
  int getDSum() {return _dsum; }
  int summary() {return _summary; }
  int verbose() {return _verbose; }

  void saveDecayInfo(EvtId ipar, int ndaug,EvtId *daug,
		     int _narg, EvtVectorT<double>& _args, 
		     EvtString name, double brfr);
  void printSummary();

  
  //Does not really belong here but I don't have a better place.
  static void findMasses(EvtParticle *p, int ndaugs, 
                            EvtId daugs[10], double masses[10]);


  //Methods to set the maximum probability.
  void setProbMax(double prbmx);
  void noProbMax();

  void checkNArg(int a1, int a2=-1);
  void checkNDaug(int d1, int d2=-1);

  void checkSpinParent(EvtSpinType::spintype sp);
  void checkSpinDaughter(int d1, EvtSpinType::spintype sp);

protected:



private:


  int _photos;
  int _ndaug;
  EvtId _parent;
  int _narg;
  EvtId *_daug;
  double *_args;
  EvtString _modelname;
  double _brfr;
  int _dsum;
  int _summary;
  int _verbose;



  int defaultprobmax;
  double probmax;
  int ntimes_prob;

  //Should charge conservation be checked when model is 
  //created? 1=yes 0 no.
  int _chkCharge;


  //These are used for gathering statistics.
  double sum_prob;
  double max_prob;


};

#endif

