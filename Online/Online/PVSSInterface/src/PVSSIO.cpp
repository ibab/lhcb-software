// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/PVSSIO.cpp,v 1.6 2007-06-21 12:20:15 frankm Exp $
//  ====================================================================
//  PVSSIO.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: PVSSIO.cpp,v 1.6 2007-06-21 12:20:15 frankm Exp $

// Framework include files
#include "PVSS/Kernel.h"
#include "PVSS/PVSSIO.h"
#include "PVSS/Internals.h"
#include "PVSS/DataPoint.h"
#include "PVSSManager/DatapointIO.h"
#include "PVSS/DevTypeElement.h"

// C++ include files
#include <cstdio>
#include <stdexcept>

namespace PVSS {
  typedef void (*LoadFunction)(void *,const void *);
  union Loaders {
    void* ptr;
    LoadFunction func;
    template <class T> Loaders(T p) : ptr((void*)p) {}
  };

  static const void* str_load(const void* s, int i)  {
    return ((ArrayVector<std::string>*)s)->vec[i].c_str();
  }
  static const void* bool_load(const void* s, int i)  {
    return (const void*)(((ArrayVector<bool>*)s)->vec[i] ? 1 : 0);
  }

  template <typename T> static void insert_vector(std::vector<T>& v, const T& o)  { 
    v.push_back(o);  
  }
  static void insert_string(std::vector<std::string>& t,const char* s) {
    t.push_back(s);   
  }
  static void copy_string  (std::string& t,const char* s)        { t = s;                         }
  static void insert_bool  (std::vector<bool>& t,bool s)         { t.push_back(s);                }
  static void insert_char  (std::vector<char>& t,char s)         { t.push_back(s);                }
  //static void insert_short (std::vector<short>& t,short s)       { t.push_back(s);                }
  static void insert_int   (std::vector<int>& t,int s)           { t.push_back(s);                }
  static void insert_float (std::vector<float>& t,float s)       { t.push_back(s);                }
  static void insert_time_t(std::vector<time_t>& t,time_t s)     { t.push_back(s);                }
  static void insert_uint  (std::vector<unsigned int>& t,unsigned int s)        { t.push_back(s); }
  static void insert_dpid  (std::vector<DpID>& t,const DpID& s)  { t.push_back(s); }

  template <class T> void addWriteIO(void* context,int typ,const DpID& dp,const std::vector<T>& v) {
    ArrayVector<T> vec(v);
    pvss_val_list_add(context,typ,dp,Values(&vec));
  }

  template <> void addWriteIO(void* context,int typ,const DpID& dp,const std::vector<bool>& v)  {
    ArrayVector<bool> vec(v, bool_load); 
    pvss_val_list_add(context,typ,dp,Values(&vec));
  }

  template <> void addWriteIO(void* context,int typ,const DpID& dp,const std::vector<std::string>& v) {
    ArrayVector<std::string> vec(v, str_load);
    pvss_val_list_add(context,typ,dp,Values(&vec));
  }

  template <class T> void setWriteIO(void* context,void*& listCtxt,int typ,const DpID& dp,const std::vector<T>& v) {
    ArrayVector<T> vec(v);
    pvss_val_list_set(context,listCtxt,typ,dp,Values(&vec));
  }

  template <> void setWriteIO(void* context,void*& listCtxt,int typ,const DpID& dp,const std::vector<bool>& v) {
    ArrayVector<bool> vec(v, bool_load); 
    pvss_val_list_set(context,listCtxt,typ,dp,Values(&vec));
  }

  template <> void setWriteIO(void* context,void*& listCtxt,int typ,const DpID& dp,const std::vector<std::string>& v) {
    ArrayVector<std::string> vec(v, str_load);
    pvss_val_list_set(context,listCtxt,typ,dp,Values(&vec));
  }

  void setGenWriteIO(void* context,void*& listCtxt,const DpID& dp,const Value* val)  {
    DataPointValues v(val->ptr());
    int typ = val->type();
    switch(typ)  {
    case DevTypeElement::DYNDPID:
      setWriteIO(context,listCtxt,typ,dp,*v.identV);
      break;
    case DevTypeElement::DYNCHAR:
      setWriteIO(context,listCtxt,typ,dp,*v.charV);
      break;
    case DevTypeElement::DYNINT:
      setWriteIO(context,listCtxt,typ,dp,*v.intV);
      break;
    case DevTypeElement::DYNUINT:
      setWriteIO(context,listCtxt,typ,dp,*v.uintV);
      break;
    case DevTypeElement::DYNFLOAT:
      setWriteIO(context,listCtxt,typ,dp,*v.floatV);
      break;
    case DevTypeElement::DYNTEXT:
      setWriteIO(context,listCtxt,typ,dp,*v.stringV);
      break;
    case DevTypeElement::DYNTIME:
      setWriteIO(context,listCtxt,typ,dp,*v.timeV);
      break;
    case DevTypeElement::DYNBIT:
      setWriteIO(context,listCtxt,typ,dp,*v.boolV);
      break;
    case DevTypeElement::TEXT:
      if ( typeid(*val) == typeid(DataValue<std::string>) )  {
        DataValue<std::string>* sval = (DataValue<std::string>*)val;
        pvss_val_list_set(context,listCtxt,typ,dp,Values(sval->data().c_str()));
        break;
      }
    case DevTypeElement::DPID:
    case DevTypeElement::CHAR:
    case DevTypeElement::INT:
    case DevTypeElement::UINT:
    case DevTypeElement::FLOAT:
    case DevTypeElement::TIME:
    case DevTypeElement::BIT:
      pvss_val_list_set(context,listCtxt,typ,dp,Values(val->ptr()));
      break;
    default:
      DataPoint::invalidValue(typeid(void));
      break;
    }
  }

  void genReadIO(const Variable* variable,int typ, void* buff)  {
    DataPointValues v(buff);
    switch(typ)  {
    case DevTypeElement::CHAR:
      DatapointIO::value(variable,*v.schar);
      break;
    case DevTypeElement::INT:
      DatapointIO::value(variable,*v.sint);
      break;
    case DevTypeElement::UINT:
      DatapointIO::value(variable,*v.uint);
      break;
    case DevTypeElement::TIME:
      DatapointIO::value(variable,*v.time);
      break;
    case DevTypeElement::BIT:
      DatapointIO::value(variable,*v.boolean);
      break;
    case DevTypeElement::FLOAT:
      DatapointIO::value(variable,*v.real);
      break;
    case DevTypeElement::DPID:
      DatapointIO::value(variable,*v.dpid);
      break;
    case DevTypeElement::TEXT:
      DatapointIO::value(variable,copy_string,*v.string);
      break;
    case DevTypeElement::DYNCHAR:
      v.charV->clear();
      DatapointIO::value(variable,insert_char,*v.charV);
      break;
    case DevTypeElement::DYNINT:
      v.intV->clear();
      DatapointIO::value(variable,insert_int,*v.intV);
      break;
    case DevTypeElement::DYNUINT:
      v.uintV->clear();
      DatapointIO::value(variable,insert_uint,*v.uintV);
      break;
    case DevTypeElement::DYNTEXT:
      v.stringV->clear();
      DatapointIO::value(variable,insert_string,*v.stringV);
      break;
    case DevTypeElement::DYNTIME:
      v.timeV->clear();
      DatapointIO::value(variable,insert_time_t,*v.timeV);
      break;
    case DevTypeElement::DYNBIT:
      v.boolV->clear();
      DatapointIO::value(variable,insert_bool,*v.boolV);
      break;
    case DevTypeElement::DYNFLOAT:
      v.floatV->clear();
      DatapointIO::value(variable,insert_float,*v.floatV);
      break;
    case DevTypeElement::DYNDPID:
      v.identV->clear();
      DatapointIO::value(variable,insert_dpid,*v.identV);
      break;
    case DevTypeElement::BIT32:
    default:
      {
        char text[132];
        sprintf(text,"Invalid value type %d for data conversion [READ]",typ);
        throw std::runtime_error(text);
      }
      break;
    }
  }
#ifdef _WIN32

  #define __T(x) template ReadTransactionSetValue <x >;
  namespace {
    template <typename T> struct ReadTransactionSetValue {
      static void do1(std::vector<T>& t)  { void* p;PVSS::setWriteIO(0,p,0,DpID(0),t);  }
      static void dp1(std::vector<T>& t)  { PVSS::addWriteIO(0,0,DpID(0),t);            }
    };
  }
#else
  #define __T(x) template void addWriteIO(void* context,int typ,const DpID& dp,const std::vector< x >& v);

#endif
  #define INSTANTIATE_FUNCTIONS(x) \
    __T(char) __T(unsigned char) __T(int) __T(unsigned int) \
    __T(long) __T(unsigned long) __T(float) __T(double) __T(bool) \
    __T(std::string) __T(DpID) __T(DPTime)

    INSTANTIATE_FUNCTIONS(1)
}
