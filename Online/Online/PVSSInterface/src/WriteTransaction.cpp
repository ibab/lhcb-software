// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/WriteTransaction.cpp,v 1.7 2007-03-05 16:16:26 frankb Exp $
//  ====================================================================
//  WriteTransaction.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: WriteTransaction.cpp,v 1.7 2007-03-05 16:16:26 frankb Exp $

// Framework include files
#include "PVSS/DataPoint.h"
#include "PVSS/DevTypeElement.h"
#include "PVSS/WriteTransaction.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/Internals.h"
#include "PVSS/PVSSIO.h"

using namespace PVSS;

/// Initializing constructor
WriteTransaction::WriteTransaction(ControlsManager* mgr) 
: m_manager(mgr), m_context(0)
{
  pvss_val_list_create(m_context);
}

/// Standard destructor
WriteTransaction::~WriteTransaction()   {
  if ( m_context )
    execute();
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
void WriteTransaction::setValue(const DataPoint& dp, const bool& value)    
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);           }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const char& value)    
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);           }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const unsigned char& value)   
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);           }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const int& value) 
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);           }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const unsigned int& value)  
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);           }

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
void WriteTransaction::setValue(const DataPoint& dp, const float& value)  
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);           }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const double& value) {
  float val = (float)value;
  pvss_val_list_add(m_context,Value::type_id(val),dp.id(),&val);
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const std::string& value)  
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),value.c_str());    }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const char* value)  
{  pvss_val_list_add(m_context,Value::type_id(std::string()),dp.id(),value);    }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const DPRef& value)  {
  DataPoint ref(dp.manager(),value.name());
  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&ref.id());
}

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const DpIdentifier& value)  
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value);           }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const DataPoint& value)
{  pvss_val_list_add(m_context,Value::type_id(value.id()),dp.id(),&value.id()); }

/// Set datapoint value 
void WriteTransaction::setValue(const DataPoint& dp, const DPTime& value)  
{  pvss_val_list_add(m_context,Value::type_id(value),dp.id(),&value.time());    }
