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
// Module: EvtGen/EvtAmp.hh
//
// Description:This is the (new) amplitude class.
//
// Modification history:
//
//    DJL/RYD     May 29, 1997         Module created
//
//------------------------------------------------------------------------

#ifndef EVTAMP_HH
#define EVTAMP_HH

#include "EvtGen/EvtComplex.hh"
#include "EvtGen/EvtSpinDensity.hh"

class EvtId;

class EvtAmp {

public:

  EvtAmp();
  EvtAmp(const EvtAmp& amp);

  void init(EvtId p,int ndaug,EvtId *daug);

  void setNDaug(int n);
  void setNState(int parent_states,int *daug_states);

  void setAmp(int *ind,const EvtComplex& amp);

  const EvtComplex& getAmp(int *ind)const;

  EvtSpinDensity getSpinDensity();

  EvtSpinDensity contract(int i,const EvtAmp& a);
  EvtAmp contract(int i, const EvtSpinDensity& rho);

  EvtSpinDensity getForwardSpinDensity(EvtSpinDensity *rho_list,int k);
  EvtSpinDensity getBackwardSpinDensity(EvtSpinDensity *rho_list);

  EvtAmp& operator=(const EvtAmp& amp);


  /**
  * sets the amplitudes calculated in the decay objects
  */
  void vertex(const EvtComplex& amp);

  /**
  * sets the amplitudes calculated in the decay objects
  */
  void vertex(int i1, const EvtComplex& amp);

  /**
  * sets the amplitudes calculated in the decay objects
  */
  void vertex(int i1, int i2, const EvtComplex& amp);

  /**
  * sets the amplitudes calculated in the decay objects
  */
  void vertex(int i1, int i2, int i3, const EvtComplex& amp);

  /**
  * sets the amplitudes calculated in the decay objects
  */
  void vertex(int *i1, const EvtComplex& amp);


  void dump();

private:

  friend class EvtDecayAmp;

  // the amplitudes
  EvtComplex _amp[125];

  // the number of daughters
  int _ndaug;

  // the number of states of the parent
  int _pstates;

  // number of states of the daughter
  int dstates[10];

  // the nontrivial index of the daughter
  int _dnontrivial[10];

  // number of nontrivial daugts+parent
  int _nontrivial;

  // compact nstates
  int _nstate[5];

};


#endif

