//-----------------------------------------------------------------------
// File and Version Information: 
//      $Id: EvtDecayMode.cpp,v 1.1 2003-10-02 17:34:48 robbep Exp $
// 
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998 Caltech, UCSB
//
// Module creator:
//      Alexei Dvoretskii, Caltech, 2001-2002.
//-----------------------------------------------------------------------

// Parses a decay string to identify the name
// of the mother and of the daughters. The string should 
// be in standard format e.g. "B+ -> pi+ pi+ pi-"

#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include "EvtGenBase/EvtPatches.hh"
#include <assert.h>
#include <iostream>
#include "EvtGenBase/EvtDecayMode.hh"
#include "EvtGenBase/EvtReport.hh"

using std::string;
using std::vector;


EvtDecayMode::EvtDecayMode(std::string mother,vector<string> dau)
  : _mother(mother)
{
  unsigned i;
  for(i=0;i<dau.size();i++) {
    
    string s;
    s.append(dau[i]);
    _dau.push_back(s);
  }
}


EvtDecayMode::EvtDecayMode(const EvtDecayMode& other)
  : _mother(other._mother)
{
  unsigned i;
  for(i=0;i<other._dau.size();i++) {
    
    string s;
    s.append(other._dau[i]);
    _dau.push_back(s);
  }
}


EvtDecayMode::EvtDecayMode(const char* decay)
{
  // Parse the decay string, it should be in a standard 
  // format, e.g. "B+ -> pi+ pi+ pi-" with all spaces
  
  string s(decay);
  size_t i,j;

  // mother

  i = s.find_first_not_of(" ");
  j = s.find_first_of(" ",i);

  if(i == string::npos) {

    report(INFO,"EvtGen") << "No non-space character found" << std::endl;
    assert(0);
  }

  if(j == string::npos) {
    
    report(INFO,"EvtGen") << "No space before -> found" << std::endl;
    assert(0);
  }

  _mother = string(s,i,j-i);

  i = s.find_first_not_of(" ",j);
  j = s.find_first_of("->",j);
  if(i != j) {

    report(INFO,"EvtGen") << "Multiple mothers?" << i << "," << j << std::endl;
    assert(0);
  }
  j += 2;

  while(1) {

    i = s.find_first_not_of(" ",j);
    j = s.find_first_of(" ",i);

    if(i == string::npos) break;
    if(j == string::npos) {
      _dau.push_back(string(s,i,s.size()-i+1));
      break;
    } else {
      _dau.push_back(string(s,i,j-i));
    }
  }
}



EvtDecayMode::~EvtDecayMode()
{}


const char* EvtDecayMode::mother() const
{
  return _mother.c_str();
}


int EvtDecayMode::nD() const
{
  return _dau.size();
}


const char* EvtDecayMode::dau(int i) const
{
  assert(0<=i && i< (int) _dau.size());
  return _dau[i].c_str();
}

const char* EvtDecayMode::mode() const
{
  string ret = _mother + string(" -> ");
  unsigned int i;
  for(i=0;i<_dau.size()-1;i++) ret += string(_dau[i]) + string(" ");
  ret += _dau[_dau.size()-1];
  return ret.c_str();
}


std::ostream& EvtDecayMode::print(std::ostream& os) const
{
  os << _mother.c_str() << " ->";
  unsigned i;
  for(i=0;i<_dau.size();i++) os << " " << _dau[i].c_str();
  return os;
}


const char* EvtDecayMode::m(EvtCyclic3::Pair i) const
{
  string s("m(");
  s.append(dau(EvtCyclic3::first(i)));
  s.append(",");
  s.append(dau(EvtCyclic3::second(i)));
  s.append(")");
  return s.c_str();
}


const char* EvtDecayMode::q(EvtCyclic3::Pair i) const
{
  string s("q(");
  s.append(dau(EvtCyclic3::first(i)));
  s.append(",");
  s.append(dau(EvtCyclic3::second(i)));
  s.append(")");
  return s.c_str();
}


const char* EvtDecayMode::dal(EvtCyclic3::Pair i, EvtCyclic3::Pair j) const
{
  string s(q(i));
  s.append(":");
  s.append(q(j));
  return s.c_str();
}


std::ostream& operator<<(std::ostream& os, const EvtDecayMode& mode)
{
  mode.print(os);
  return os;
}
