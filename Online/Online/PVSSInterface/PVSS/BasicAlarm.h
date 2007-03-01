// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/BasicAlarm.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  BasicAlarm.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
// $Id: BasicAlarm.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
#ifndef ONLINE_PVSS_BASICALARM_H
#define ONLINE_PVSS_BASICALARM_H

// Framework include files
#include "PVSS/CfgSetting.h"

// C++ include files
#include <memory>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  class DataPoint;

  /** @class BasicAlarm   BasicAlarm.h  PVSS/BasicAlarm.h
    *
    *   Base class for all PVSS alarms.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class BasicAlarm : public CfgSetting  {
  protected:
    /// Apply alarm settings to datapoint identified by datapoint name
    virtual void i_apply(Transaction& tr, const std::string& dp_name);
    /// Apply alarm settings to datapoint identified by datapoint name
    virtual void i_remove(Transaction& tr, const std::string& dp_name);
  public:
    /// Default constructor
    BasicAlarm()  {}
    /// Copy constructor
    BasicAlarm(const BasicAlarm& copy) : CfgSetting(copy)  {}
    /// Initializing constructor
    BasicAlarm(const std::string& dp_name) : CfgSetting(dp_name) {}
    /// Standard destructor
    virtual ~BasicAlarm()  {}
    /// Assignment operator for insertion in STL containers
    BasicAlarm& operator=(const BasicAlarm& alm)  {
      CfgSetting::operator=(alm);  
      return *this;
    }
    /// Equality operator for insertion in STL containers
    bool operator==(const BasicAlarm& alm) const
    { return CfgSetting::operator==(alm);  }
    /// Operator less for insertion in STL associative classes
    bool operator<(const BasicAlarm& alm) const
    { return CfgSetting::operator<(alm);  }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_BASICALARM_H
