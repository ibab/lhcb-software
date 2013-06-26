// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/ReadTransaction.h,v 1.4 2007-03-12 09:04:12 frankb Exp $
//  ====================================================================
//  ReadTransaction.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_READTRANSACTION_H
#define ONLINE_PVSS_READTRANSACTION_H

// Framework include files
#include "PVSS/Kernel.h"
#include "PVSS/DataPoint.h"
#include "PVSS/PVSSIO.h"
#include <map>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class ControlsManager;
  class DataPoint;
  class DPRef;
  class DPTime;
  namespace ns_ReadTransaction {  struct Actor;    }

  /** @class ReadTransaction   ReadTransaction.h  PVSS/ReadTransaction.h
    *
    *  PVSS ReadTransaction descriptor.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class ReadTransaction  {
    friend class ControlsManager;
    typedef std::map<DpID,DataPoint*> DataPoints;

    /// Reference to the controls manager
    ControlsManager *m_manager;
    /// Transaction context/actor (dpID-value list)
    ns_ReadTransaction::Actor           *m_actor;

    /// Initializing constructor
    ReadTransaction(ControlsManager* mgr);

    /// Internal helper for vector IO
    void i_getValue(int typ,const DpID& dp,void* v);

  public:
    /// Standard destructor
    virtual ~ReadTransaction();
    /// Access ReadTransaction type manager
    ControlsManager* manager() const    {  return m_manager;   }
    /// Execute  data point list read action
    bool execute(bool wait = true);

    /// Get datapoint value
    void getValue(const DataPoint& dp, bool& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, char& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, unsigned char& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, int& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, unsigned int& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, long& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, unsigned long& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, float& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, double& value);
    /// Get datapoint value 
    void getValue(const DataPoint& dp, std::string& value);
    /// Get datapoint value by name
    void getValue(const DataPoint& dp, DpID& value);
    /// Get datapoint value by name
    void getValue(const DataPoint& dp, DPTime& value);
    /// Get datapoint value for dynamic items
    template <class T> void getValue(const DataPoint& dp, std::vector<T>& value)
    { i_getValue(Value::type_id(value),dp.id(),&value); }
    /// Set datapoint value by name
    template <class T> void getValue(const std::string& dp_name, T& value)
    { getValue(DataPoint(manager(),dp_name), value);    }
    /// Set original value
    template <class T> void getOriginal(const std::string& s, T& value)
    { getValue(DataPoint::original(s),value);          }
    /// Set original value
    template <class T> void getOriginal(const DataPoint& dp, T& value)
    { getValue(DataPoint::original(dp.name()),value);  }
    /// Set online Value
    template <class T> void getOnline(const std::string& s, T& value)
    { getValue(DataPoint::online(s),value);            }
    /// Set online Value
    template <class T> void getOnline(const DataPoint& dp, T& value)
    { getValue(DataPoint::online(dp.name()),value);    }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_READTRANSACTION_H
