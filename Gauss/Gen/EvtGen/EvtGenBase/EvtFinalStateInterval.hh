//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998      Caltech, UCSB
//
//    Alexei Dvoretskii     December, 2001         Module created
//------------------------------------------------------------------------


// The phase-space for a final state of a decay.
// Purely abstract interface.

#ifndef EVT_FINAL_STATE_INTERVAL_HH
#define EVT_FINAL_STATE_INTERVAL_HH

#include <vector>
#include "EvtGenBase/EvtInterval.hh"
#include "EvtGenBase/EvtVector4R.hh"

template <class P>
class EvtFinalStateInterval : public EvtInterval<P> {

public:
  
  EvtFinalStateInterval() {}
  EvtFinalStateInterval(const EvtFinalStateInterval<P>& other) {}
  virtual ~EvtFinalStateInterval() {}


  // clone to initialize a pointer in a compound object
  
  virtual EvtInterval<P>* clone() const = 0;
  
 
  // Number of final state particles 
  virtual int nPart() const = 0;

  // Number of degrees of freedom

  virtual int nDoF() const = 0;


  // Initialize decay phase-space given the invariant coordinate. 
  // The rotation angles will be random.

#ifndef __CINT__
  virtual void initPhaseSpace(const P&,std::vector<EvtVector4R>& v) const = 0;
#endif
};

#endif









