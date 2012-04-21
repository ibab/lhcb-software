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

#ifndef EVTDALITZDECAYINFO_HH
#define EVTDALITZDECAYINFO_HH

#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenModels/EvtResonancePrototype.hh"
#include <vector>

class EvtDalitzDecayInfo {
public:

  EvtDalitzDecayInfo(EvtId d1, EvtId d2, EvtId d3)
    : _d1(d1), _d2(d2), _d3(d3), _probMax(0.) {}
  ~EvtDalitzDecayInfo() {}

  void addResonance(EvtResonancePrototype res) {_resonances.push_back(res);}
  void setProbMax(double probMax) {_probMax = probMax;}

  const std::vector<EvtResonancePrototype>& getResonances() const {return _resonances;}
  double getProbMax() const {return _probMax;}

  inline const EvtId& daughter1() const {return _d1;}
  inline const EvtId& daughter2() const {return _d2;}
  inline const EvtId& daughter3() const {return _d3;}

private:

  EvtId _d1, _d2, _d3;
  std::vector<EvtResonancePrototype> _resonances;
  double _probMax;

};

#endif
