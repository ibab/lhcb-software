// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/ControlsManager.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  ControlsManager.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_CONTROLSMANAGER_H
#define ONLINE_PVSS_CONTROLSMANAGER_H

// Framework include files
#include "PVSS/NamedIdentified.h"
#include "PVSS/DeviceIO.h"
#include "PVSS/ReadTransaction.h"
#include "PVSS/WriteTransaction.h"

// C++ include files
#include <memory>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class CfgManager;
  class DeviceIO;
  class DeviceManager;
  class DevTypeManager;
  class ReadTransaction;
  class WriteTransaction;

  /** @class ControlsManager   ControlsManager.h  PVSS/ControlsManager.h
    *
    *  PVSS manager interface.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class ControlsManager  : public NamedIdentified {
  public:
    friend struct CfgManip<ControlsManager>;

  protected:
    /// Access to configuration manager
    CfgManager      *m_cfgMgr;
    /// Access to device manager
    DeviceManager   *m_devMgr;
    /// Device type manager
    DevTypeManager  *m_devTypeMgr;

  public:
    /// Initializing constructor
    ControlsManager(int i, const std::string nam);
    /// Standard destructor
    virtual ~ControlsManager();
    /// Access to controls manager
    CfgManager* configMgr() const          {  return m_cfgMgr;      }
    /// Access to device manager 
    DeviceManager* deviceMgr() const       {  return m_devMgr;      }
    /// Access to device manager 
    DevTypeManager* typeMgr() const        {  return m_devTypeMgr;  }
    /// Create device reader to read device data from PVSS
    std::auto_ptr<DeviceIO> devReader();
    /// Create device writer to write device data to PVSS
    std::auto_ptr<DeviceIO> devWriter();
    /// Create transaction object to read device data from PVSS
    std::auto_ptr<ReadTransaction> readTransaction();
    /// Create transaction object to write device data to PVSS
    std::auto_ptr<WriteTransaction> writeTransaction();
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_CONTROLSMANAGER_H
