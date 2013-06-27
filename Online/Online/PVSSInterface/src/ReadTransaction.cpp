// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/ReadTransaction.cpp,v 1.8 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  ReadTransaction.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: ReadTransaction.cpp,v 1.8 2007-10-01 14:46:55 frankm Exp $

// Framework include files
#include "PVSS/HotLinkCallback.h"
#include "PVSS/ReadTransaction.h"
#include "PVSS/DevTypeElement.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/DataPoint.h"
#include "PVSS/Internals.h"

#include <cstdio>

using namespace PVSS;

namespace PVSS { namespace ns_ReadTransaction {

  struct Actor : public HotLinkCallback  {
    typedef std::pair<int,void*>          Entry;
    typedef std::map<DpID,Entry>  Points;
    /// Container of datapoints in transaction
    Points     m_points;
    /// Transaction context (list)
    void*      m_context;

    /// Default constructor
    Actor() : m_context(0) {    }
    /// Default destructor
    virtual ~Actor()       {    }
    /// Add datapoint to read trabsaction
    void add(const DpID& dpid, int t,void* v) 
    {  m_points.insert(std::make_pair(dpid,Entry(t,v)));     }
    /// Execute transaction
    bool exec(DevAnswer* a) {
      pvss_list_create(m_context);
      for(Points::const_iterator i=m_points.begin(); i != m_points.end(); ++i)
        pvss_list_add(m_context,(*i).first);
      pvss_exec_read(m_context,a,this,false);
      return true;
    }
    /// Bind reading for a datapoint
    virtual void setValue(const DpID& dpid, int typ, const Variable* val) {
      Points::iterator i=m_points.find(dpid);
      if ( i != m_points.end() )
        genReadIO(val, typ, (*i).second.second);
      else
        ::printf("PVSS> Attempt to set datapoint, which is not contained in the transaction.\n");
    }
    /// Clear list after end of reading
    virtual void handleDataUpdate()  {
      if ( m_context ) pvss_list_drop(m_context);
      m_points.clear();
      m_context = 0;
    }
  };
}}

/// Initializing constructor
ReadTransaction::ReadTransaction(ControlsManager* mgr) 
  : m_manager(mgr), m_actor(new ns_ReadTransaction::Actor)
{
}

/// Standard destructor
ReadTransaction::~ReadTransaction()   {
  delete m_actor;
}

/// Execute dpset list
bool ReadTransaction::execute(bool wait)  {
  if ( wait )  {
    DevAnswer a;
    m_actor->exec(&a);
    if ( a.state() != DevAnswer::FINISHED )  {
      a.print();
    }
    return a.state() == DevAnswer::FINISHED;
  }
  m_actor->exec(0);
  return true;
}

/// Internal helper for vector IO
void ReadTransaction::i_getValue(int typ,const DpID& dp,void* v)
{  m_actor->add(dp,typ,v);                                            }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, bool& value)
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, char& value)
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, unsigned char& value)
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, int& value)
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, unsigned int& value)    
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, long& value)
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, unsigned long& value)    
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, float& value)    
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, double& value)    
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value 
void ReadTransaction::getValue(const DataPoint& dp, std::string& value)
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Get datapoint value by name
void ReadTransaction::getValue(const DataPoint& dp, DpID& value)    
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }

/// Set datapoint value
void ReadTransaction::getValue(const DataPoint& dp, DPTime& value)  
{  m_actor->add(dp.id(),Value::type_id(value),&value);                }
