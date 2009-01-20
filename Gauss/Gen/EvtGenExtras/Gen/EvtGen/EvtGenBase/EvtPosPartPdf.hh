/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtPosPartPdf.hh,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// PDF is the positive part of a real-valued function

#ifndef EVT_POS_PART_PDF_HH
#define EVT_POS_PART_PDF_HH

#include "EvtGenBase/EvtPdf.hh"

template <class Interval> class EvtPosPartPdf : public EvtPdf<Interval> {
public:
  
  typedef typename Interval::point_type Point;

  EvtPosPartPdf(const EvtRealValued<Interval>& f)
    : EvtPdf<Interval>(), _f(f.clone())
  {}
  
  EvtPosPartPdf(const EvtPosPartPdf<Interval>& other) 
    : EvtPdf<Interval>(other), COPY_PTR(_f)
  {}

  virtual ~EvtPosPartPdf() 
  { 
    if(_f) delete _f; 
  }

  virtual EvtPosPartPdf<Interval>* clone() const
  {
    return new EvtPosPartPdf<Interval>(*this);
  }
  
protected:

  virtual double pdf(const Point& p) const
  {
    double ret = (*_f)(p);
    if(ret < 0) ret = 0;
    return ret;
  }

  EvtRealValued<Interval>* _f;
}; 

#endif
