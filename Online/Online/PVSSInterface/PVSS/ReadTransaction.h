// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/ReadTransaction.h,v 1.2 2007-03-02 00:33:37 frankb Exp $
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

  /** @class ReadTransaction   ReadTransaction.h  PVSS/ReadTransaction.h
    *
    *  PVSS ReadTransaction descriptor.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class ReadTransaction  {
    friend class ControlsManager;
    typedef std::map<DpIdentifier,DataPoint*> DataPoints;

    /// Reference to the controls manager
    ControlsManager *m_manager;
    /// Transaction context (dpID-value list)
    void            *m_context;

    /// Initializing constructor
    ReadTransaction(ControlsManager* mgr);

  public:
    /// Standard destructor
    virtual ~ReadTransaction();
    /// Access ReadTransaction type manager
    ControlsManager* manager() const    {  return m_manager;   }
    /// Execute dpset list
    bool execute(bool wait = true);
    /// Restart dpset list (scratch old list if present)
    bool start();

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
    void getValue(const DataPoint& dp, DpIdentifier& value);
    /// Get datapoint value by name
    void getValue(const DataPoint& dp, DPTime& value);
    /// Get datapoint value for dynamic items
    template <class T> void getValue(const DataPoint& dp, std::vector<T>& value);

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
