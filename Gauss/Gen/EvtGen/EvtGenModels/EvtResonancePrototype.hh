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

#ifndef EVTRESONANCEPROTOTYPE_HH
#define EVTRESONANCEPROTOTYPE_HH

#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtFlatte.hh"

class EvtResonancePrototype {
public:

  EvtResonancePrototype()
    : _factor(0.), _ampl(0.), _theta(0.), _gamma(0.), _mass(0.),
      _spin(0), _useDaughterPair(0), _bwFactor(0), _useRelBW(false),
      _angdenom(false), _useFlatte(false) {}

//NonRes
  EvtResonancePrototype(EvtComplex ampFactor, double ampl, double theta)
    : _factor(ampFactor), _ampl(ampl), _theta(theta), _gamma(0),
      _mass(0), _spin(0), _useDaughterPair(0),
      _bwFactor(0), _useRelBW(0), _angdenom(0), _useFlatte(0), _nonRes(true) {}

//BW
  EvtResonancePrototype(EvtComplex ampFactor, double ampl, double theta,
                        double gamma, double mass, int spin, int daughterPair,
                        bool invmass_angdenom)
    : _factor(ampFactor), _ampl(ampl), _theta(theta), _gamma(gamma),
      _mass(mass), _spin(spin), _useDaughterPair(daughterPair),
      _bwFactor(0), _useRelBW(false), _angdenom(invmass_angdenom),
      _useFlatte(false), _nonRes(false) {}

//RelBW
  EvtResonancePrototype(EvtComplex ampFactor, double ampl, double theta,
                        double gamma, double mass, int spin, int daughterPair,
                        int bwFactor)
    : _factor(ampFactor), _ampl(ampl), _theta(theta), _gamma(gamma),
      _mass(mass), _spin(spin), _useDaughterPair(daughterPair),
      _bwFactor(bwFactor), _useRelBW(true), _angdenom(false),
      _useFlatte(false), _nonRes(false) {}

//Flatte
  EvtResonancePrototype(EvtComplex ampFactor, double ampl, double theta,
                        double mass, int daughterPair)
    : _factor(ampFactor), _ampl(ampl), _theta(theta), _gamma(0),
      _mass(mass), _spin(0), _useDaughterPair(daughterPair),
      _bwFactor(0), _useRelBW(false), _angdenom(false),
      _useFlatte(true), _nonRes(false) {}

  ~EvtResonancePrototype() {}
  EvtComplex getAmp(const EvtVector4R& p4_p, const EvtVector4R& p4_d1,
                    const EvtVector4R& p4_d2, const EvtVector4R& p4_d3);

  void addFlatteParam(const EvtFlatteParam& param) { _flatteParams.push_back(param); }

private:

  EvtComplex _factor;
  double _ampl, _theta, _gamma, _mass;
  int _spin, _useDaughterPair, _bwFactor;
  bool _useRelBW, _angdenom, _useFlatte, _nonRes;
  /* PRIORITY OF FLAGS
   * if _useFlatte is set then we will produce the amplitude from an EvtFlatte
   * else if _useBW is set we will produce a Breitt-Wigner with an EvtResonance
   * else we will use an EvtResonance2 (and flag _angDenom will be used)
   */
  std::vector<EvtFlatteParam> _flatteParams;

};

#endif
