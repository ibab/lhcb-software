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
// Module: EvtGen/EvtSymTable.hh
//
// Description: Class to hold the symbols that are defined
//              in the DECAY files.
//
// Modification history:
//
//    RYD     May 8, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTSYMTABLE_HH
#define EVTSYMTABLE_HH

#include "EvtGen/EvtString.hh"

const int MAXSYM=100;

class EvtSymTable {

public:

  EvtSymTable(); 
  virtual ~EvtSymTable();

  void Define(const EvtString& name,double d);

  double Get(const EvtString& name,int &ierr);

private:

  double value[MAXSYM];
  EvtString  name[MAXSYM];

  int nsym;
  
}; 

#endif

