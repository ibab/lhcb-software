// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/WriteTransaction.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  WriteTransaction.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DEVTRANSACTION_H
#define ONLINE_PVSS_DEVTRANSACTION_H

// Framework include files
#include "PVSS/Kernel.h"
#include "PVSS/DataPoint.h"

// C++ include files
#include <vector>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class ControlsManager;
  class DataPoint;
  class DPRef;
  class DPTime;

  /** @class WriteTransaction WriteTransaction.h PVSS/WriteTransaction.h
    *
    *  PVSS WriteTransaction descriptor.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class WriteTransaction  {
    friend class ControlsManager;

    /// Reference to the controls manager
    ControlsManager *m_manager;
    /// Transaction context (dpID-value list)
    void            *m_context;
    /// Initializing constructor
    WriteTransaction(ControlsManager* mgr);
  public:
    /// Standard destructor
    virtual ~WriteTransaction();
    /// Access WriteTransaction type manager
    ControlsManager* manager() const    {  return m_manager;   }

    /// Execute dpset list
    bool execute(bool wait = true);
    /// Restart dpset list (scratch old list if present)
    bool start();

    /// Set datapoint value 
    void setValue(const DataPoint& dp, const bool& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const char& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const unsigned char& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const short& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const unsigned short& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const int& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const unsigned int& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const long& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const unsigned long& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const float& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const double& value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const char* value);
    /// Set datapoint value 
    void setValue(const DataPoint& dp, const std::string& value);
    /// Set datapoint reference value by name
    void setValue(const DataPoint& dp, const DPRef& value);
    /// Set datapoint reference value by name
    void setValue(const DataPoint& dp, const DpIdentifier& value);
    /// Set datapoint reference value by name
    void setValue(const DataPoint& dp, const DataPoint& value);
    /// Set datapoint value by name
    void setValue(const DataPoint& dp, const DPTime& value);
    /// Set datapoint value for dynamic items
    template <class T> void setValue(const DataPoint& dp, const std::vector<T>& value);

    /// Set datapoint value by name
    template <class T> void setValue(const std::string& dp_name, const T& value)
    {  setValue(DataPoint(manager(),dp_name), value);  }
    /// Set original value
    template <class T> void setOriginal(const std::string& s, const T& value)
    { setValue(DataPoint::original(s),value);          }
    /// Set original value
    template <class T> void setOriginal(const DataPoint& dp, const T& value)
    { setValue(DataPoint::original(dp.name()),value);  }
    /// Set online Value
    template <class T> void setOnline(const std::string& s, const T& value)
    { setValue(DataPoint::online(s),value);            }
    /// Set online Value
    template <class T> void setOnline(const DataPoint& dp, const T& value)
    { setValue(DataPoint::online(dp.name()),value);    }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DEVTRANSACTION_H
