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
// Module: EvtSymTable.cc
//
// Description: Class to hold the symbols that are defined
//              in the DECAY files.
// Modification history:
//
//    RYD     May 8, 1997         Module created
//
//------------------------------------------------------------------------
//
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include "EvtGen/EvtSymTable.hh"
#include "EvtGen/EvtReport.hh"


EvtSymTable::~EvtSymTable(){}

EvtSymTable::EvtSymTable() {

 nsym=0;
}

void EvtSymTable::Define(const EvtString& symname,double d) {

  int i;

  for(i=0;i<nsym;i++){
    if (name[i]==symname){
      report(INFO,"EvtGen") << "Symbol:"<<symname<<
	" redefined, old value:"<<value[i]<<" new value:"<<d<<std::endl;
      name[i]=symname;
      value[i]=d;
      return;
    }
  }

  if (nsym>=MAXSYM) {
    report(ERROR,"EvtGen") << "Out of space for symbols; can not store:"
			   << symname<<std::endl;
    return;
  }

  name[nsym]=symname;
  value[nsym]=d;

  nsym++;
  return;
}

double EvtSymTable::Get(const EvtString& symname,int& ierr) {

  int i;

  ierr=0;

  for(i=0;i<nsym;i++){
    if (symname==name[i]){
      return value[i];
    }
  }

  i=0;


  while(symname.value()[i]!=0){
    if (isalpha(symname.value()[i])&&symname.value()[i]!='e'){
      ierr=1;
      return 0.0;
    }
    i++;
  }

  char** strptr=0;

  return strtod(symname.value(),strptr);


}

