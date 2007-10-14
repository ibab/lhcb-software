/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtPartPropDb.cpp,v 1.3 2007-10-14 18:50:08 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include "EvtGenBase/EvtPartPropDb.hh"

std::vector<std::string> EvtPartPropDb::_names;
std::vector<double>      EvtPartPropDb::_masses;
std::vector<double>      EvtPartPropDb::_widths;
std::vector<int>         EvtPartPropDb::_stdhepid;
bool EvtPartPropDb::_init = false;

EvtPartPropDb::EvtPartPropDb()
{}

EvtPartPropDb::EvtPartPropDb(const char* fname)
{
  readPDT(fname);
}

EvtPartPropDb::~EvtPartPropDb()
{}

void EvtPartPropDb::readPDT(const char* fname)
{
  // Read evt.pdl file. Most code borrowed from EvtPDL

  std::ifstream indec(fname);
  
  char cmnd[100], xxxx[100], pname[100];
  int  stdhepid;
  double mass, pwidth, pmaxwidth;
  int    chg3, spin2;
  double ctau;
  int    lundkc;
  
  if (!indec) {
    printf("Could not open: %s\n",fname);
    assert(0);
  }
  
  do{
    
    char ch,ch1;
    
    do{
      
      indec.get(ch);  
      if (ch=='\n') indec.get(ch);
      if (ch!='*') {
	indec.putback(ch);
      }
      else{
	while (indec.get(ch1),ch1!='\n');
      }
    } while(ch=='*');

    indec >> cmnd;

    if (strcmp(cmnd,"end")){
      
      if (!strcmp(cmnd,"add")){
	
	indec >> xxxx >> xxxx >> pname >> stdhepid;
	indec >> mass >> pwidth >> pmaxwidth >> chg3 >> spin2 >> ctau >> lundkc;

	_stdhepid.push_back(stdhepid);
	_names.push_back(pname);
	_masses.push_back(mass);
	_widths.push_back(pwidth);
      }
    }
  }while(strcmp(cmnd,"end"));

  _init = true;
}

int EvtPartPropDb::getStdHepID(const char* name)
{
  if(!_init) { printf("EvtPartPropDb not initialized\n"); exit(0); }
  
  unsigned i;
  for(i = 0; i < _names.size(); i++) 
    if(0 == strcmp(_names[i].c_str(),name)) return _stdhepid[i];
  
  return 0;
}

double EvtPartPropDb::getMass(const char* name)
{
  if(!_init) { printf("EvtPartPropDb not initialized\n"); exit(0); }
  
  unsigned i;
  for(i = 0; i < _names.size(); i++) 
    if(0 == strcmp(_names[i].c_str(),name)) return _masses[i];
  
  printf("No particle %s found in table\n",name);
  assert(0);
}

double EvtPartPropDb::getWidth(const char* name) 
{
  if(!_init) { printf("EvtPartPropDb not initialized\n"); exit(0); }

  unsigned i;
  for(i = 0; i < _names.size(); i++) if(0 == strcmp(_names[i].c_str(),name)) return _widths[i];
  
  printf("No particle %s found in table\n",name);
  assert(0);
}

bool EvtPartPropDb::initialized() { 
  return _init; 
}
