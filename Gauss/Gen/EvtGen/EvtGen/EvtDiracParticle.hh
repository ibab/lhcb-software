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
// Module: EvtGen/EvtDiracParticle.hh
//
// Description:EvtDiracParticle particles i.e. spin 1/2 particles.
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------

#ifndef EVTDIRACPARTICLE_HH
#define EVTDIRACPARTICLE_HH

#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtId.hh"

class EvtVector4R;

class EvtDiracParticle:public EvtParticle {

public:

  
  EvtDiracParticle();
  virtual ~EvtDiracParticle();
  void init(EvtId part_n,const EvtVector4R& p4);
  EvtDiracSpinor spParent(int) const;
  EvtDiracSpinor sp(int) const;   
  EvtSpinDensity rotateToHelicityBasis() const;
  EvtSpinDensity rotateToHelicityBasis(double alpha,
				       double beta,
				       double gamma) const;

private:

  EvtDiracSpinor spinor1_rest,spinor2_rest;
  EvtDiracSpinor spinor1_parent,spinor2_parent;
  EvtDiracParticle(const EvtDiracParticle& d);
  EvtDiracParticle& operator=(const EvtDiracParticle& d);

};
#endif

