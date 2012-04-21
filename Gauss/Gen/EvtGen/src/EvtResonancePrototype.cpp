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
// Module: EvtGen/EvtGenericDalitz.hh
//
// Description: Model to describe a generic dalitz decay
//
// Modification history:
//
//    DCC     16 December, 2011         Module created
//
//------------------------------------------------------------------------

#include "EvtGenModels/EvtResonancePrototype.hh"
#include "EvtGenBase/EvtResonance.hh"
#include "EvtGenBase/EvtResonance2.hh"

EvtComplex EvtResonancePrototype::getAmp(const EvtVector4R& p4_p,
                                         const EvtVector4R& p4_d1,
                                         const EvtVector4R& p4_d2,
                                         const EvtVector4R& p4_d3) {

  double pi180inv = 1.0/EvtConst::radToDegrees;
  if(_nonRes) {
    return _factor * _ampl * EvtComplex(cos(_theta*pi180inv),sin(_theta*pi180inv));
  }

  EvtVector4R mom1, mom2;
  switch(_useDaughterPair) {
    case 1:
      mom1 = p4_d1;
      mom2 = p4_d2;
      break;
    case 2:
      mom1 = p4_d2;
      mom2 = p4_d3;
      break;
    case 3:
      mom1 = p4_d3;
      mom2 = p4_d1;
  }
  if(_useFlatte) {
    EvtFlatte res(p4_p, mom1, mom2, _ampl, _theta, _mass, _flatteParams);
    return _factor * res.resAmpl();
  } else if(_useRelBW) {
    EvtResonance res(p4_p, mom1, mom2, _ampl, _theta, _gamma, _mass, _spin);
    return _factor * res.relBrWig(_bwFactor);
  } else {
    EvtResonance2 res(p4_p, mom1, mom2, _ampl, _theta, _gamma, _mass, _spin, _angdenom);
    return _factor * res.resAmpl();
  }
}
