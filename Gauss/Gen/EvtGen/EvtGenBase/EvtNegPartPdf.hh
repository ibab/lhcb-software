/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtNegPartPdf.hh,v 1.1 2003-10-02 17:25:54 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// PDF is the minus negative part of a real-valued function

#ifndef EVT_NEG_PART_PDF_HH
#define EVT_NEG_PART_PDF_HH

#include "EvtGenBase/EvtPdf.hh"

template <class Interval> class EvtNegPartPdf : public EvtPdf<Interval> {
public:
  
  typedef typename Interval::point_type Point;

  EvtNegPartPdf(const EvtRealValued<Interval>& f)
    : EvtPdf<Interval>(), _f(f.clone())
  {}
  
  EvtNegPartPdf(const EvtNegPartPdf<Interval>& other) 
    : EvtPdf<Interval>(other), COPY_PTR(_f)
  {}

  virtual ~EvtNegPartPdf() 
  { 
    if(_f) delete _f; 
  }

  virtual EvtNegPartPdf<Interval>* clone() const
  {
    return new EvtNegPartPdf<Interval>(*this);
  }
  
protected:

  virtual double pdf(const Point& p) const
  {
    double ret = -(*_f)(p);
    if(ret < 0) ret = 0;
    return ret;
  }

  EvtRealValued<Interval>* _f;
}; 

#endif
