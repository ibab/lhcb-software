/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtIntegralDb.hh,v 1.2 2004-01-12 13:50:26 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

/*
 * Interface to a flat file database of integrals.
 * File format:
 * 
 * KEY nItg itg error
 */

#ifndef INTEGRAL_DB_HH
#define INTEGRAL_DB_HH

#define MAX_ENTRIES 256
#include "EvtGenBase/EvtValError.hh"

class EvtIntegralDb {
public:

  EvtIntegralDb() {}
  ~EvtIntegralDb() {}

  void read(const char* file);
  int append(const char* key, int nitg, EvtValError x);
  int find(const char* key, int& nitg, EvtValError& x) const;
  int write() const;

  void print() const;
  
private:
  
  char _file[256];
  char _key[MAX_ENTRIES][256];
  int _nitg[MAX_ENTRIES];
  EvtValError _itg[MAX_ENTRIES];

  int _n;
};

#endif
