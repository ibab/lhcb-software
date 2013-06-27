// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpIdValueList.cpp,v 1.8 2009-02-10 19:32:35 frankb Exp $
//  ====================================================================
//  DpIdValueList.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DpIdValueList.cpp,v 1.8 2009-02-10 19:32:35 frankb Exp $
#include <memory>
#include <iostream>
#include <cstdio>
#include <stdint.h>

#include "PVSS/Internals.h"
#include "PVSS/Array.h"
#include "PVSS/DpID.h"
#include "DpIdentifierVar.hxx"
#include "DpIdValueList.hxx"
#include "DpElementType.hxx"
#include "UIntegerVar.hxx"
#include "IntegerVar.hxx"
#include "FloatVar.hxx"
#include "Bit32Var.hxx"
#include "TextVar.hxx"
#include "TimeVar.hxx"
#include "CharVar.hxx"
#include "BitVar.hxx"
#include "DynVar.hxx"

///=====
template <typename V, typename T> static 
std::auto_ptr<DynVar> get_dyn_var(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar);
  const char* p   = (const char*)arr->first;
  for(int i=0; i<arr->size; ++i, p+=arr->elemSize)  {
    T& o = *(T*)p;
    V  v(o);
    var->append(v);
  }
  return var;
}
std::auto_ptr<DynVar> get_dyn_var_dpid(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar);
  const DpIdentifier *p = (const DpIdentifier*)arr->first;
  for(int i=0; i<arr->size; ++i, ++p)
    var->append(DpIdentifierVar(*p));
  return var;
}
std::auto_ptr<DynVar> get_dyn_var_char(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar);
  const char *p = (const char*)arr->first;
  for(int i=0; i<arr->size; ++i, ++p)
    var->append(CharVar(*p));
  return var;
}
std::auto_ptr<DynVar> get_dyn_var_int(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar);
  const int *p = (const int*)arr->first;
  for(int i=0; i<arr->size; ++i, ++p)
    var->append(IntegerVar(*p));
  return var;
}
std::auto_ptr<DynVar> get_dyn_var_uint(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar);
  const unsigned int *p = (const unsigned int*)arr->first;
  for(int i=0; i<arr->size; ++i, ++p)
    var->append(UIntegerVar(*p));
  return var;
}
std::auto_ptr<DynVar> get_dyn_var_float(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar);
  const float *p = (const float*)arr->first;
  for(int i=0; i<arr->size; ++i, ++p)
    var->append(FloatVar(*p));
  return var;
}
std::auto_ptr<DynVar> get_dyn_var_time(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar);
  const time_t *p = (const time_t*)arr->first;
  for(int i=0; i<arr->size; ++i, ++p)
    var->append(TimeVar(BC_CTime(*p),0));
  return var;
}
std::auto_ptr<DynVar> get_dyn_var_text(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar(TEXT_VAR));
  if ( !arr->load )  {
    throw "String variables require an explicit load function when passed from VC6 to VC7 vv.";
  }
  for(int i=0; i<arr->size; ++i)  {
    TextVar* v = new TextVar((const char*)arr->load(arr,i));
    if ( !(var->append(v) == PVSS_TRUE) )  {
      ::puts("PVSS> Variables do not match type TextVar!\n");
    }
  }
  return var;
}
std::auto_ptr<DynVar> get_dyn_var_bool(const PVSS::Array* arr)  {
  std::auto_ptr<DynVar> var(new DynVar);
  if ( !arr->load )  {
    throw "BOOL array variables require an explicit load function when passed from VC6 to VC7 vv.";
  }
  for(int i=0; i<arr->size; ++i)  {
    var->append(BitVar(arr->load(arr,i) ? 1 : 0));
  }
  return var;
}
// ====
void PVSS::pvss_val_list_create(void*& context)    {
  context = new DpIdValueList;
}
void PVSS::pvss_val_list_drop(void*& context)    {
  if ( context )
    delete (DpIdValueList*)context;
  context = 0;
}

void PVSS::pvss_val_list_set(void* ctxt,void*& listctxt, int type,const DpID& id,const Values& val)  {
  DpIdValueList* rd = (DpIdValueList*)ctxt;
  DpVCItem *idx = (DpVCItem*)listctxt;
  DpVCItem *itm = (((void*)~0x0) == listctxt) ? rd->getFirstItem() : rd->getNextItem(idx);
  listctxt = itm;
  if ( !itm )  {
    pvss_val_list_add(ctxt,type,id,val);
    return;
  }
  itm->setDpIdentifier(id);
  switch(type)   {
  case DPELEMENT_DPID:
    itm->setValue(new DpIdentifierVar(*val.dpid));
    break;
  case DPELEMENT_CHAR:
    itm->setValue(new CharVar(*val.uchar));
    break;
  case DPELEMENT_INT:
    itm->setValue(new IntegerVar(*val.sint));
    break;
  case DPELEMENT_UINT:
    itm->setValue(new UIntegerVar(*val.uint));
    break;
  case DPELEMENT_FLOAT:
    itm->setValue(new FloatVar(*val.real));
    break;
  case DPELEMENT_TEXT:
    itm->setValue(new TextVar(val.text));
    break;
  case DPELEMENT_TIME:
    itm->setValue(new TimeVar(BC_CTime(*val.time),0));
    break;
  case DPELEMENT_BIT:
    itm->setValue(new BitVar(*val.boolean ? 1 : 0));
    break;
  case DPELEMENT_DYNDPID:
    itm->setValue(get_dyn_var_dpid(val.array).release());
    break;
  case DPELEMENT_DYNCHAR:
    itm->setValue(get_dyn_var_char(val.array).release());
    break;
  case DPELEMENT_DYNUINT:
    itm->setValue(get_dyn_var_uint(val.array).release());
    break;
  case DPELEMENT_DYNINT:
    itm->setValue(get_dyn_var_int(val.array).release());
    break;
  case DPELEMENT_DYNFLOAT:
    itm->setValue(get_dyn_var_float(val.array).release());
    break;
  case DPELEMENT_DYNTEXT:
    itm->setValue(get_dyn_var_text(val.array).release());
    break;
  case DPELEMENT_DYNTIME:
    itm->setValue(get_dyn_var_time(val.array).release());
    break;
  case DPELEMENT_DYNBIT:
    itm->setValue(get_dyn_var_bool(val.array).release());
    break;
  case DPELEMENT_32BIT:
  default:
    throw "pvss_val_list_dpset> Invalid data type";
    break;
  }
}

void PVSS::pvss_val_list_add(void*& ctxt,int type,const DpID& id,const Values& val)   {
  if ( 0 == ctxt ) ctxt = new DpIdValueList;
  DpIdValueList& list = *(DpIdValueList*)ctxt;
  switch(type)   {
  case DPELEMENT_DPID:
    list.appendItem(id, DpIdentifierVar(*val.dpid));
    break;
  case DPELEMENT_CHAR:
    list.appendItem(id, CharVar(*val.uchar));
    break;
  case DPELEMENT_INT:
    list.appendItem(id, IntegerVar(*val.sint));
    break;
  case DPELEMENT_UINT:
    list.appendItem(id, UIntegerVar(*val.uint));
    break;
  case DPELEMENT_FLOAT:
    list.appendItem(id, FloatVar(*val.real));
    break;
  case DPELEMENT_TEXT:
    list.appendItem(id, TextVar(val.text));
    break;
  case DPELEMENT_TIME:
    list.appendItem(id, TimeVar(BC_CTime(*val.time),0));
    break;
  case DPELEMENT_BIT:
    list.appendItem(id, BitVar(*val.boolean ? 1 : 0));
    break;
  case DPELEMENT_DYNDPID:
    list.appendItem(id, get_dyn_var_dpid(val.array).release());
    break;
  case DPELEMENT_DYNCHAR:
    list.appendItem(id, get_dyn_var_char(val.array).release());
    break;
  case DPELEMENT_DYNUINT:
    list.appendItem(id, get_dyn_var_uint(val.array).release());
    break;
  case DPELEMENT_DYNINT:
    list.appendItem(id, get_dyn_var_int(val.array).release());
    break;
  case DPELEMENT_DYNFLOAT:
    list.appendItem(id, get_dyn_var_float(val.array).release());
    break;
  case DPELEMENT_DYNTEXT:
    list.appendItem(id, get_dyn_var_text(val.array).release());
    break;
  case DPELEMENT_DYNTIME:
    list.appendItem(id, get_dyn_var_time(val.array).release());
    break;
  case DPELEMENT_DYNBIT:
    list.appendItem(id, get_dyn_var_bool(val.array).release());
    break;

  case DPELEMENT_32BIT:
  default:
    throw "pvss_val_list_dpset> Invalid data type";
    break;
  }
}
