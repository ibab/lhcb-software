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
// Module: EvtGen/EvtString.hh
//
// Description:Class for string manipulation in EvtGen.
//
// Modification history:
//
//    RYD         Sept 11, 1998         Created
//
//------------------------------------------------------------------------

#ifndef EVTSTRING_HH
#define EVTSTRING_HH

#include <iostream>

class EvtString {

public:

  //need a default constructor
  EvtString() {_ptr=0;}

  EvtString(const char* cptr);

  EvtString(const EvtString& string);

  friend std::ostream& operator<<(std::ostream& s, const EvtString& str);  
  friend std::istream& operator>>(std::istream& s, EvtString& str);  

  virtual ~EvtString();

  EvtString operator=(const EvtString& string);

  int operator==(const EvtString& string) const;
  int operator!=(const EvtString& string) const;

  int operator==(const char* string);
  int operator!=(const char* string);

  char* value() const {return _ptr;}

private:

  char* _ptr;
  
}; 

#endif

