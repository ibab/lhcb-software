/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtTwoBodyVertex.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

#include <iostream>
#include <math.h>
#include <assert.h>
#include "EvtGenBase/EvtMacros.hh"
#include "EvtGenBase/EvtTwoBodyVertex.hh"


// Default ctor can sometimes be useful

EvtTwoBodyVertex::EvtTwoBodyVertex()
  : _LL(0), _p0(0), _f(0)
{}

EvtTwoBodyVertex::EvtTwoBodyVertex(double mA, double mB, double mAB, int L)
  : _kine(), _LL(L), _p0(0), _f(0)
{
  // Kinematics is initialized only if the decay is above threshold

  if(mAB > mA + mB) {

    _kine = EvtTwoBodyKine(mA,mB,mAB);
    _p0 = _kine.p();
  }
}


EvtTwoBodyVertex::EvtTwoBodyVertex(const EvtTwoBodyVertex& other)
  : _kine(other._kine), _LL(other._LL), _p0(other._p0), 
  _f( (other._f) ? new EvtBlattWeisskopf(*other._f) : 0 )
{}

EvtTwoBodyVertex::~EvtTwoBodyVertex()
{
  if(_f) delete _f;
}


void EvtTwoBodyVertex::set_f(double R) 
{
  if(_f) delete _f;
  _f = new EvtBlattWeisskopf(_LL,R,_p0);
}


double EvtTwoBodyVertex::widthFactor(EvtTwoBodyKine x) const
{
  assert(_p0 > 0.);

  double p1 = x.p();
  double ff = formFactor(x);
  double factor = pow(p1/_p0,2*_LL+1)*mAB()/x.mAB() * ff * ff;

  return factor;
}


double EvtTwoBodyVertex::phaseSpaceFactor(EvtTwoBodyKine x,EvtTwoBodyKine::Index i) const
{
  double p1 = x.p(i);
  double factor = pow(p1,_LL);
  return factor;
}

double EvtTwoBodyVertex::formFactor(EvtTwoBodyKine x) const
{
  double ff = 1.;

  if(_f) {

    double p1 = x.p();
    ff = (*_f)(p1);
  }
  
  return ff;
}

void EvtTwoBodyVertex::print(std::ostream& os) const
{
  os << " mA = " << mA() << std::endl;
  os << " mB = " << mB() << std::endl;
  os << "mAB = " << mAB() << std::endl;
  os << "  L = " << _LL << std::endl;
  os << " p0 = " << _p0 << std::endl;
}


std::ostream& operator<<(std::ostream& os, const EvtTwoBodyVertex& v)
{
  v.print(os);
  return os;
}
