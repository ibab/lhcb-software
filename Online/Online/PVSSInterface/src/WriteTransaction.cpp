// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/WriteTransaction.cpp,v 1.5 2007-03-02 12:19:02 frankb Exp $
//  ====================================================================
//  WriteTransaction.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: WriteTransaction.cpp,v 1.5 2007-03-02 12:19:02 frankb Exp $

// Framework include files
#include "PVSS/DataPoint.h"
#include "PVSS/DevTypeElement.h"
#include "PVSS/WriteTransaction.h"
#include "PVSS/Internals.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/Array.h"

using namespace PVSS;

/// Initializing constructor
WriteTransaction::WriteTransaction(ControlsManager* mgr) 
: m_manager(mgr), m_context(0)
{
  pvss_val_list_create(m_context);
}

/// Standard destructor
WriteTransaction::~WriteTransaction()   {
  if ( m_context )  {
    execute();
  }
}

/// Execute dpset list
bool WriteTransaction::execute(bool wait)  {
  if ( wait )  {
    DevAnswer a;
    pvss_exec_dpset(m_context,&a,false);
    if ( a.state() != DevAnswer::FINISHED )  {
      a.print();
    }
    return a.state() == DevAnswer::FINISHED;
  }
  pvss_exec_dpset(m_context,0,false);
  return true;
}

/// Restart dpset list (scratch old list if present)
bool WriteTransaction::start()  {
  if ( m_context ) pvss_val_list_drop(m_context);
  pvss_val_list_create(m_context);
  return true;
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const bool& value)    {
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);    
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const char& value)    {
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);    
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const unsigned char& value)   {
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);    
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const int& value) {
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);    
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const unsigned int& value)  {
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);    
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const long& value) {
  int val = value;
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&val);
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const unsigned long& value) {
  int val = value;
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&val);
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const float& value)  {
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);    
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const double& value) {
  float val = (float)value;
  pvss_val_list_add(m_context,Value::type_id(val),dp.id(),&val);
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const std::string& value)  {
  const char* c = value.c_str();
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),c);
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const char* value)  {
  pvss_val_list_add(m_context,Value::type_id(std::string()),dp.id(),value);
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const DPRef& value)  {
  DataPoint ref(dp.manager(),value.name());
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&ref.id());
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const DpIdentifier& value)  {
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const DataPoint& value)  {
  pvss_val_list_add(m_context,Value::type_id(value.id()),dp.id(),&value.id());
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const DPTime& value)  {
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value.time());
}

/// Set datapoint value for dynamic items
template <typename T> 
void WriteTransaction::setValue(const DataPoint& dp, const std::vector<T>& value)    {
  Vector<T> v(value);
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&v);
}

static void str_load(const void* s, int i, const void** v)  {
  Vector<std::string>* vec = (Vector<std::string>*)s;
  *v = vec->vec[i].c_str();
}
static void bool_load(const void* s, int i, const void** v)  {
  static bool value;
  Vector<bool>* vec = (Vector<bool>*)s;
  value = vec->vec[i];
  *v = &value;
}

/// Set datapoint value for dynamic items
template <> 
void WriteTransaction::setValue<std::string>(const DataPoint& dp, const std::vector<std::string>& value)    {
  Vector<std::string> v(value,str_load);
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&v);
}

/// Set datapoint value for dynamic items
template <> 
void WriteTransaction::setValue<bool>(const DataPoint& dp, const std::vector<bool>& value)    {
  Vector<bool> v(value,bool_load);
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&v);
}
#define __T(x)  template WriteTransactionSetValue<x >;
#define INSTANTIATE_FUNCTIONS(T)  \
      __T(char)  __T(unsigned char) \
      __T(int)   __T(unsigned int)  __T(long)  __T(unsigned long)\
      __T(float) __T(double) __T(bool) __T(std::string) \
      __T(DpIdentifier) __T(DPRef) __T(DPTime)

namespace {
  template <typename T> struct WriteTransactionSetValue {
    static void do1(const std::vector<T>& t)  { ((WriteTransaction*)0)->setValue("",t); }
    static void dp1(const std::vector<T>& t)  { ((WriteTransaction*)0)->setValue(DataPoint(0,""),t); }
    static void do2(const std::vector<T>& t)  { ((WriteTransaction*)0)->setOriginal("",t); }
    static void do3(const std::vector<T>& t)  { ((WriteTransaction*)0)->setOnline("",t); }
    static void do1(const T& t)               { ((WriteTransaction*)0)->setValue("",t); }
    static void do2(const T& t)               { ((WriteTransaction*)0)->setOriginal("",t); }
    static void do3(const T& t)               { ((WriteTransaction*)0)->setOnline("",t); }
  };
  INSTANTIATE_FUNCTIONS(1)
}
