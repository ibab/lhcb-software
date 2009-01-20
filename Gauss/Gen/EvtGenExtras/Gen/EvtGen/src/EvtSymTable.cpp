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
#include <fstream>
#include <stdlib.h>
#include <ctype.h>
#include "EvtGenBase/EvtSymTable.hh"
#include "EvtGenBase/EvtReport.hh"
#include <string>


EvtSymTable::~EvtSymTable(){}

EvtSymTable::EvtSymTable() {

}

void EvtSymTable::Define(const std::string& symname,std::string d) {

  //  int i;

  if ( _symMap.find(symname)!=_symMap.end() ) {
    report(INFO,"EvtGen") << "Symbol:"<<symname.c_str()<<
      " redefined, old value:"<<_symMap[symname].c_str()<<" new value:"<<d.c_str()<<std::endl;
    _symMap[symname]=d;
    return;
  }

  _symMap[symname]=d;
  return;
}

//double EvtSymTable::Get(const std::string& symname,int& ierr) {
std::string EvtSymTable::Get(const std::string& symname,int& ierr) {

  //  int i;

  ierr=0;

  if ( _symMap.find(symname)!=_symMap.end() ) return _symMap[symname];

  // If no matching symbol found just return the string

  return symname;
}

