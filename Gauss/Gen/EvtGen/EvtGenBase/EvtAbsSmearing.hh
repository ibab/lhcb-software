/*****************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtAbsSmearing.hh,v 1.1 2003-10-02 17:25:51 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *****************************************************************************/

/*
 * Change the input value. This can be the way to introduce detector 
 * resolution, or conversely, kinematic reconstruction.
 */ 

#ifndef EVT_ABS_SMEARING_HH
#define EVT_ABS_SMEARING_HH

template <class T> class EvtAbsSmearing {
public:

  EvtAbsSmearing() 
  {}
  EvtAbsSmearing(const EvtAbsSmearing<T>& other)
  {}
  virtual ~EvtAbsSmearing() 
  {}

  virtual EvtAbsSmearing<T>* clone() const = 0;
  virtual T smear(const T point) = 0;

};

#endif
