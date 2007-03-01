// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/CfgSetting.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  CfgSetting.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_CFGSETTING_H
#define ONLINE_PVSS_CFGSETTING_H

// Framework include files
#include "PVSS/WriteTransAction.h"
#include "PVSS/DataPoint.h"

// C++ include files
#include <memory>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  class DataPoint;

  /** @class CfgSetting   CfgSetting.h  PVSS/CfgSetting.h
    *
    *   Base class for all PVSS Configs.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class CfgSetting {
  protected:
    /// Datapoint name (if supplied by constructor
    std::string m_dpName;
    /// Definition of transaction handle
    typedef const std::auto_ptr<WriteTransaction> Transaction;

    /// Apply alarm settings to datapoint identified by datapoint name
    virtual void i_apply(Transaction& tr, const std::string& dp_name) = 0;
    /// Apply alarm settings to datapoint identified by datapoint name
    virtual void i_remove(Transaction& tr, const std::string& dp_name) = 0;
  public:
    /// Default constructor
    CfgSetting()  {}
    /// Copy constructor
    CfgSetting(const CfgSetting& copy);
    /// Initializing constructor
    CfgSetting(const std::string& dp_name);
    /// Standard destructor
    virtual ~CfgSetting()  {}
    /// Access datapoint name (if present)
    const std::string& dpName()  const  {   return m_dpName; }
    /// Apply alarm to datapoint supplied in constructor
    virtual void apply(Transaction& tr);
    /// Apply alarm settings to datapoint identified by datapoint structure
    virtual void apply(Transaction& tr, const DataPoint& dp);
    /// Apply alarm settings to datapoint identified by datapoint name
    virtual void apply(Transaction& tr, const std::string& dp_name);
    /// Remove range alarm from datapoint
    virtual void remove(Transaction& tr);
    /// Apply alarm settings to datapoint identified by datapoint structure
    virtual void remove(Transaction& tr, const DataPoint& dp);
    /// Apply alarm settings to datapoint identified by datapoint name
    virtual void remove(Transaction& tr, const std::string& dp_name);
    /// Assignment operator for insertion in STL containers
    CfgSetting& operator=(const CfgSetting& setting);
    /// Equality operator for insertion in STL containers
    bool operator==(const CfgSetting& setting) const 
    { return m_dpName == setting.m_dpName;    }
    /// Operator less for insertion in STL associative classes
    bool operator<(const CfgSetting& setting) const
    { return m_dpName < setting.m_dpName;     }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_CFGSETTING_H
