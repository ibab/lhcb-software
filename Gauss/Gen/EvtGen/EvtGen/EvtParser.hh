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
// Module: EvtGen/EvtParser.hh
//
// Description:
//
// Modification history:
//
//    RYD     Febuary 11, 1997         Module created
//
//------------------------------------------------------------------------

#ifndef EVTPARSER_HH
#define EVTPARSER_HH

#include "EvtGen/EvtString.hh"

class EvtParser {
public:
  EvtParser();
  ~EvtParser();

  void Read(const EvtString filename);
  int getNToken();
  const EvtString& getToken(int i);
  int getLineofToken(int i);

private:

  int _ntoken;
  EvtString* _tokenlist;
  int * _linelist;
  int _lengthoftokenlist;

  void addToken(int line,const EvtString& string);

}; 

#endif

