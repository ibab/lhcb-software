// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DatapointIO.cpp,v 1.4 2007-03-01 21:53:58 frankb Exp $
//  ====================================================================
//  DatapointIO.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DatapointIO.cpp,v 1.4 2007-03-01 21:53:58 frankb Exp $
#include "DpIdentifierVar.hxx"
#include "DpIdValueList.hxx"
#include "DpIdentifier.hxx"
#include "IntegerVar.hxx"
#include "UIntegerVar.hxx"
#include "FloatVar.hxx"
#include "Bit32Var.hxx"
#include "TextVar.hxx"
#include "TimeVar.hxx"
#include "CharVar.hxx"
#include "BitVar.hxx"
#include "DynVar.hxx"
#include "PVSSManager/DatapointIO.h"

using namespace PVSS;

template <class T> static void simple_conversion(const Variable* var, T& val)  {
  switch(var->isA())  {
  case BIT_VAR:      val = T(((BitVar*)var)->getValue()==PVSS_TRUE ? 0 :1); return;
  case CHAR_VAR:     val = T(((CharVar*)var)->getValue());                  return;
  case INTEGER_VAR:  val = T(((IntegerVar*)var)->getValue());               return;
  case UINTEGER_VAR: val = T(((UIntegerVar*)var)->getValue());              return;
  case FLOAT_VAR:    val = T(((FloatVar*)var)->getValue());                 return;
  default: break;
  }
  throw "Invalid variable type for data conversion";
}

template <> static void simple_conversion<bool>(const Variable* var, bool& v)  {
  char v0;
  simple_conversion<char>(var,v0);
  v = v0 != 0;
}

template <class T> static void 
vector_conversion(const Variable* var, void (*ldf)(std::vector<T>&,T), std::vector<T>& val)  {
  if ( var->isDynVar() )    {
    T theVal;
    DynVar* theVar = (DynVar*)var;
    for (Variable* v=theVar->getFirst(); v; v=theVar->getNext())  {
      simple_conversion(v,theVal);
      (*ldf)(val,theVal);
    }
    return;
  }
  throw "Invalid variable type for DYN type data conversion";
}

#define MAKE_CONVERSION(x) \
void DatapointIO::value(const Variable* var,void (*ldf)(std::vector< x >&,x),std::vector<x>& val) \
{ vector_conversion<x>(var,ldf,val);  } \
void DatapointIO::value(const Variable* var, x& val) {  simple_conversion(var,val);  }

MAKE_CONVERSION(bool)
MAKE_CONVERSION(char)
MAKE_CONVERSION(unsigned char)
MAKE_CONVERSION(short)
MAKE_CONVERSION(unsigned short)
MAKE_CONVERSION(int)
MAKE_CONVERSION(unsigned int)
MAKE_CONVERSION(long)
MAKE_CONVERSION(unsigned long)
MAKE_CONVERSION(float)
MAKE_CONVERSION(double)

void DatapointIO::value(const Variable* var, void (*ldf)(std::string&,const char*), std::string& val) {
  if(var->isA() == TEXT_VAR)
    ldf(val,((TextVar*)var)->getValue());
  else
    throw "Invalid variable type for data conversion to std::string";
}

void DatapointIO::value(const Variable* var, void (*ldf)(std::vector<std::string>&,const char*),std::vector<std::string>& val) {
  if ( var->isDynVar() )    {
    DynVar* theVar = (DynVar*)var;
    for (Variable* v=theVar->getFirst(); v; v=theVar->getNext())  {
      if(v->isA() == TEXT_VAR)
        ldf(val,((TextVar*)v)->getValue());
      else
        throw "Invalid variable type for data conversion to std::string";
    }
    return;
  }
  throw "Invalid variable type for DYN type data conversion";
}

void DatapointIO::value(const Variable* var,DpIdentifier& val)  {
  if(var->isA() == DPIDENTIFIER_VAR)
    val = ((DpIdentifierVar*)var)->getValue();
  else
    throw "Invalid variable type for data conversion to DpIdentifier";
}

void DatapointIO::value(const Variable* var,void (*ldf)(std::vector<DpIdentifier>&,const DpIdentifier&),std::vector<DpIdentifier>& val)  {
  if ( var->isDynVar() )    {
    const DynVar* theVar = (const DynVar*)var;
    for (Variable* v=theVar->getFirst(); v; v=theVar->getNext())  {
      if(v->isA() == DPIDENTIFIER_VAR)
        (*ldf)(val,((DpIdentifierVar*)v)->getValue());
      else
        throw "Invalid variable type for data conversion to DpIdentifier";
    }
    return;
  }
  throw "Invalid variable type for DYN type data conversion";
}
