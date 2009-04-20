#ifndef EVTCOMPLEXEXTRAS_HH
#define EVTCOMPLEXEXTRAS_HH

#include <iostream>
#include <math.h>
#include "EvtGenBase/EvtConst.hh"
#include "EvtGenBase/EvtComplex.hh"

//extra functions using the GSL
EvtComplex atan(const EvtComplex& cmp);
EvtComplex chop(const EvtComplex& cmp);
EvtComplex log(const EvtComplex& cmp);
EvtComplex pow(const EvtComplex& cmp, const double index);
EvtComplex pow(const EvtComplex& cmp, const EvtComplex index);
EvtComplex sqrt(const EvtComplex& cmp);
EvtComplex sqrt_real(const double value);

std::istream& operator>>(std::istream& s, EvtComplex& c);
std::ostream& operator<<(std::ostream& s, const EvtComplex& c);

#endif
