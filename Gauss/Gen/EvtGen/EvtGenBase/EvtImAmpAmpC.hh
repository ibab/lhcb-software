/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtImAmpAmpC.hh,v 1.1 2003-10-02 17:25:53 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Real part of amp1xamp2*

#ifndef EVT_IM_AMP_AMPC_HH
#define EVT_IM_AMP_AMPC_HH

#include "EvtGenBase/EvtMacros.hh"
#include "EvtGenBase/EvtRealValued.hh"

template <class Interval> 
class EvtImAmpAmpC : public EvtRealValued<Interval> {
public:
  
  typedef typename Interval::point_type Point;
  
  EvtImAmpAmpC(const EvtAmplitude<Interval>& amp1, 
	     const EvtAmplitude<Interval>& amp2) 
    : EvtRealValued<Interval>(),
    _amp1(amp1.clone()),
    _amp2(amp2.clone())
  {}
  
  EvtImAmpAmpC(const EvtImAmpAmpC<Interval>& other)
    : EvtRealValued<Interval>(other)
    ,COPY_PTR(_amp1), COPY_PTR(_amp2)
  {}

  virtual ~EvtImAmpAmpC() 
  {
    if(_amp1) delete _amp1;
    if(_amp2) delete _amp2;
  }
  virtual EvtRealValued<Interval>* clone() const
  {
    return new EvtImAmpAmpC<Interval>(*this);
  }
  
  virtual double operator()(const Point& p) const
  {
    EvtComplex c = _amp1->evaluate(p) * _amp2->evaluate(p);
    return imag(c);
  }

protected:

  EvtAmplitude<Interval>* _amp1;
  EvtAmplitude<Interval>* _amp2;
}; 

#endif




