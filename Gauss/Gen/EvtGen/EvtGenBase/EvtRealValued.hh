/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtRealValued.hh,v 1.1 2003-10-02 17:25:55 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Complex-valued amplitude

#ifndef EVT_REAL_VALUED_HH
#define EVT_REAL_VALUED_HH

template <class Interval> class EvtRealValued {
public:
  
  typedef typename Interval::point_type Point;
  EvtRealValued() {}
  EvtRealValued(const EvtRealValued&) {}
  virtual ~EvtRealValued() {}
  virtual EvtRealValued<Interval>* clone() const = 0;
  
  virtual double operator()(const Point& p) const = 0;
}; 

#endif





