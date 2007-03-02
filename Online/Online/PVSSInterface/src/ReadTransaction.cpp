// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/ReadTransaction.cpp,v 1.3 2007-03-02 00:33:37 frankb Exp $
//  ====================================================================
//  ReadTransaction.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: ReadTransaction.cpp,v 1.3 2007-03-02 00:33:37 frankb Exp $

// Framework include files
#include "PVSS/ReadTransaction.h"
#include "PVSS/DevTypeElement.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/DataPoint.h"
#include "PVSS/Internals.h"

using namespace PVSS;
typedef void (*LoadFunction)(void *,const void *);

/// Initializing constructor
ReadTransaction::ReadTransaction(ControlsManager* mgr) 
: m_manager(mgr), m_context(0)
{
  pvss_start_dpget(m_context);
}

/// Standard destructor
ReadTransaction::~ReadTransaction()   {
  if ( m_context )  {
    execute();
  }
}

/// Execute dpset list
bool ReadTransaction::execute(bool wait)  {
  if ( wait )  {
    DevAnswer a;
    pvss_exec_dpget(m_context,&a);
    if ( a.state() != DevAnswer::FINISHED )  {
      a.print();
    }
    return a.state() == DevAnswer::FINISHED;
  }
  pvss_exec_dpget(m_context,0);
  return true;
}

/// Restart dpset list (scratch old list if present)
bool ReadTransaction::start()  {
  if ( m_context ) pvss_drop_dpget(m_context);
  pvss_start_dpget(m_context);
  return true;
}

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, bool& value)
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, char& value)
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);   }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, unsigned char& value)
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);   }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, int& value)
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, unsigned int& value)    
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, long& value)
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, unsigned long& value)    
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, float& value)    
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, double& value)    
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, std::string& value)
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Get datapoint value by name
void ReadTransaction::getValue(const DataPoint& dp, DpIdentifier& value)    
{  pvss_add_dpget(m_context,Value::type_id(value),0,dp.id(),&value);    }

/// Set datapoint value
void ReadTransaction::getValue(const DataPoint& dp, DPTime& value)  
{  pvss_add_dpget(m_context,DevTypeElement::TIME,0,dp.id(),&value.time()); }

template <typename T> static void insert_vector(std::vector<T>& v, const T& o)  { v.push_back(o);  }
static void insert_string(std::vector<std::string>& t,const char* s) { t.push_back(s);   }

union Loaders {
  void* ptr;
  LoadFunction func;
  template <class T> Loaders(T p) : ptr((void*)p) {}
};

/// Get datapoint value for dynamic items
template <class T> void ReadTransaction::getValue(const DataPoint& dp, std::vector<T>& value) {  
  Loaders ldr(insert_vector<T>);
  pvss_add_dpget(m_context,Value::type_id(value),ldr.func,dp.id(),&value);    
}

/// Get datapoint value for dynamic items
template <> void ReadTransaction::getValue<std::string>(const DataPoint& dp, std::vector<std::string>& value) {  
  Loaders ldr(insert_string);
  pvss_add_dpget(m_context,Value::type_id(value),ldr.func,dp.id(),&value);    
}

#define __T(x) template ReadTransactionSetValue <x >;
#define INSTANTIATE_FUNCTIONS(x) \
  __T(char) __T(unsigned char) __T(int) __T(unsigned int) \
  __T(long) __T(unsigned long) __T(float) __T(double) __T(bool) \
  __T(std::string) __T(DpIdentifier) __T(DPTime)

namespace {

  template <typename T> struct ReadTransactionSetValue {
    static void do1(std::vector<T>& t)  { ((ReadTransaction*)0)->getValue("",t); }
    static void dp1(std::vector<T>& t)  { ((ReadTransaction*)0)->getValue(DataPoint(0,""),t); }
    static void do2(std::vector<T>& t)  { ((ReadTransaction*)0)->getOriginal("",t); }
    static void do3(std::vector<T>& t)  { ((ReadTransaction*)0)->getOnline("",t); }
    static void do1(T& t)               { ((ReadTransaction*)0)->getValue("",t); }
    static void do2(T& t)               { ((ReadTransaction*)0)->getOriginal("",t); }
    static void do3(T& t)               { ((ReadTransaction*)0)->getOnline("",t); }
  };
  INSTANTIATE_FUNCTIONS(1)
}

