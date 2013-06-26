// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DatapointIO.cpp,v 1.8 2010-05-18 09:09:31 frankb Exp $
//  ====================================================================
//  DatapointIO.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DatapointIO.cpp,v 1.8 2010-05-18 09:09:31 frankb Exp $
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

template <class T> static void pvss_simple_conversion(const Variable* var, T& val)  {
  switch(var->isA())  {
  case BIT_VAR:      val = T(((BitVar*)var)->getValue()==PVSS_TRUE ? 1 : 0); return;
  case CHAR_VAR:     val = T(((CharVar*)var)->getValue());                   return;
  case INTEGER_VAR:  val = T(((IntegerVar*)var)->getValue());                return;
  case UINTEGER_VAR: val = T(((UIntegerVar*)var)->getValue());               return;
  case FLOAT_VAR:    val = T(((FloatVar*)var)->getValue());                  return;
  case TIME_VAR:     val = T(((TimeVar*)var)->getSeconds());                 return;
  default: break;
  }
  throw "Invalid variable type for data conversion";
}

template <> void pvss_simple_conversion<bool>(const Variable* var, bool& v)  {
  char v0;
  pvss_simple_conversion<char>(var,v0);
  v = v0 != 0;
}

template <class T> static void 
vector_conversion(const Variable* var, void (*ldf)(std::vector<T>&,T), std::vector<T>& val)  {
  if ( var->isDynVar() )    {
    T theVal;
    DynVar* theVar = (DynVar*)var;
    for (Variable* v=theVar->getFirst(); v; v=theVar->getNext())  {
      pvss_simple_conversion(v,theVal);
      (*ldf)(val,theVal);
    }
    return;
  }
  throw "Invalid variable type for DYN type data conversion";
}

#define MAKE_CONVERSION(x) \
void DatapointIO::value(const Variable* var,void (*ldf)(std::vector< x > &,x),std::vector< x > & val) \
{ vector_conversion<x>(var,ldf,val);  } \
void DatapointIO::value(const Variable* var, x& val) {  pvss_simple_conversion(var,val);  }

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

void DatapointIO::value(const Variable* var,DpID& val)  {
  if(var->isA() == DPIDENTIFIER_VAR)
    val = ((DpIdentifierVar*)var)->getValue();
  else
    throw "Invalid variable type for data conversion to DpID";
}

void DatapointIO::value(const Variable* var,void (*ldf)(std::vector<DpID>&,const DpID&),std::vector<DpID>& val)  {
  if ( var->isDynVar() )    {
    const DynVar* theVar = (const DynVar*)var;
    for (Variable* v=theVar->getFirst(); v; v=theVar->getNext())  {
      if(v->isA() == DPIDENTIFIER_VAR)
        (*ldf)(val,(DpID&)((DpIdentifierVar*)v)->getValue());
      else
        throw "Invalid variable type for data conversion to DpID";
    }
    return;
  }
  throw "Invalid variable type for DYN type data conversion";
}
