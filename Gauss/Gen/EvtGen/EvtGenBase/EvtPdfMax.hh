/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtPdfMax.hh,v 1.1 2003-10-02 17:25:54 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Pdf maximum and its location

#ifndef EVT_PDF_MAX_HH
#define EVT_PDF_MAX_HH

#include "EvtMacros.hh"

// PDF maximum - helper class

template <class Point>
class EvtPdfMax {

public:

  EvtPdfMax() 
    :  _value(-1), _valueKnown(false), _locKnown(false) 
  {}
  EvtPdfMax(double value)  
    : _value(value), _valueKnown(true), _locKnown(false) 
  {}
  EvtPdfMax(Point p, double value) 
    :  _value(value), _loc(p), _valueKnown(true), _locKnown(true) 
  {}
  EvtPdfMax(const EvtPdfMax& other)     
    :  COPY_MEM(_value), COPY_MEM(_loc), COPY_MEM(_valueKnown), COPY_MEM(_locKnown)
  {}
  ~EvtPdfMax() {}
  
  bool valueKnown() const { return _valueKnown; }
  double value() const { assert(_valueKnown); return _value; }
  bool locKnown() const { return _locKnown; }
  Point loc() const { assert(_locKnown); return _loc; }

private:

  double _value;
  Point  _loc;
  bool   _valueKnown;
  bool   _locKnown;

};

#endif
