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
// Module: EvtGen/EvtString.cc
//
// Description:Class for string manipulation in EvtGen.
//
// Modification history:
//
//    RYD         Sept 11, 1998         Created
//
//------------------------------------------------------------------------

#include "EvtGen/EvtString.hh"
#include <iostream.h>
#include <string.h>

EvtString::EvtString(const char* cptr){

  _ptr=new char[strlen(cptr)+1];
  strcpy(_ptr,cptr);

}

EvtString::EvtString(const EvtString& string){

  _ptr=new char[strlen(string._ptr)+1];
  strcpy(_ptr,string._ptr);

}

ostream& operator<<(ostream& s, const EvtString& str){

  s << str._ptr;
  return s;

}  

istream& operator>>(istream& s, EvtString& str){

  char buf[1024];
  s >> buf;
  delete [] str._ptr;
  str._ptr=new char [strlen(buf)+1];
  strcpy(str._ptr,buf);

  return s;

}

EvtString::~EvtString(){

  delete [] _ptr;

}

EvtString EvtString::operator=(const EvtString& string){

  delete [] _ptr;
  _ptr=new char [strlen(string._ptr)+1];
  strcpy(_ptr,string._ptr);
  
  return EvtString(_ptr);

}

int EvtString::operator==(const EvtString& string)const{

  return !strcmp(_ptr,string._ptr);

}


int EvtString::operator!=(const EvtString& string)const{

  return strcmp(_ptr,string._ptr);

}




int EvtString::operator==(const char* string){

  return !strcmp(_ptr,string);

}


int EvtString::operator!=(const char* string){

  return strcmp(_ptr,string);

}




