// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/CfgSetting.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $
//  ====================================================================
//  RangeAlarm.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: CfgSetting.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $

// Framework include files
#include "PVSS/CfgType.h"
#include "PVSS/DataPoint.h"
#include "PVSS/CfgSetting.h"

using namespace PVSS;

/// Copy constructor
CfgSetting::CfgSetting(const CfgSetting& c) : m_dpName(c.m_dpName) 
{                                                         }

/// Initializing constructor
CfgSetting::CfgSetting(const std::string& dp_name) : m_dpName(dp_name) 
{                                                         }

/// Apply alarm settings to datapoint identified by datapoint structure
void CfgSetting::apply(Transaction& tr, const DataPoint& dp)    
{  apply(tr, dp.name());                                  }

/// Apply alarm settings to datapoint identified by name
void CfgSetting::apply(Transaction& tr, const std::string& dp_name)    
{  return i_apply(tr, dp_name);                           }

/// Apply alarm settings to datapoint identified by datapoint structure
void CfgSetting::apply(Transaction& tr)  
{  apply(tr, m_dpName);                                   }

/// Remove range alarm from datapoint
void CfgSetting::remove(Transaction& tr)
{  remove(tr, m_dpName);                                  }

/// Apply alarm settings to datapoint identified by datapoint structure
void CfgSetting::remove(Transaction& tr, const DataPoint& dp)
{  remove(tr, dp.name());                                 }

/// Apply alarm settings to datapoint identified by datapoint name
void CfgSetting::remove(Transaction& tr, const std::string& dp_name)
{  i_remove(tr, dp_name);                                 }

/// Assignment operator for insertion in STL containers
CfgSetting& CfgSetting::operator=(const CfgSetting& setting) {
  m_dpName = setting.m_dpName; 
  return *this;
}
