/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtIntegralDb.cpp,v 1.1 2003-10-02 17:34:47 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <string.h>
#include <stdio.h>
#include "EvtGenBase/EvtIntegralDb.hh"

void EvtIntegralDb::read(const char* file)
{
  strcpy(_file,file);
  FILE* f = 0;
  int i = 0;
  _N = 0;
  
  if(NULL == (f = fopen(file,"r"))) return;
  else {    
    
    char key[256];
    float itg, err;
    while(EOF != fscanf(f,"%s %d %f %f",&_key[i],&_nitg[i],&itg,&err)) {

      _itg[i] = EvtValError(itg,err);
      i++;  
    }
  }
  _N = i;   
}

int EvtIntegralDb::append(const char* key, int nitg, EvtValError x)
{
  int i;
  for(i=0;i<_N;i++) {
    
    if(!strcmp(key,_key[i])) {

      _nitg[i] = nitg;
      _itg[i] = x;
      return 1;
    }
  }

  strcpy(_key[i],key);
  _nitg[_N] = nitg;
  _itg[_N] = x;
  _N++;
  return 0;
}

int EvtIntegralDb::find(const char* key, int& nitg, EvtValError& x) const
{
  int i;
  for(i=0;i<_N;i++) {
    
    if(!strcmp(key,_key[i])) {
      
      nitg = _nitg[i];
      x = _itg[i];
      return 1;
    }
  }
  return 0;
}

int EvtIntegralDb::write() const
{
  FILE* f = fopen(_file,"w");
  int i;
  for(i=0;i<_N;i++) {
    fprintf(f,"%20s %8d %10.4f %10.4f\n",_key[i],_nitg[i],_itg[i].value(),_itg[i].error());
  }  

  return 0;
}

void EvtIntegralDb::print() const
{
  int i;
  if(0 == _N) printf("Database empty\n");
  else {
    for(i=0;i<_N;i++) {
      printf("%20s %8d %10.4f %10.4f\n",_key[i],_nitg[i],_itg[i].value(),_itg[i].error());
    }
  }
}
