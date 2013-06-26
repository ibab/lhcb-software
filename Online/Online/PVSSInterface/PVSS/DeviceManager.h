// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DeviceManager.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  DeviceManager.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DEVICEMANAGER_H
#define ONLINE_PVSS_DEVICEMANAGER_H

// Framework include files
#include "PVSS/NamedIdentified.h"
#include "PVSS/Device.h"

// C++ include files
#include <map>
#include <set>
#include <memory>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class ControlsManager;
  class DevAnswer;
  class DevType;

  /** @class DeviceManager   DeviceManager.h  PVSS/DeviceManager.h
    *
    *  Manager for active (=known) PVSS devices.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DeviceManager  : public NamedIdentified {
  public:
    friend struct DevManip<DeviceManager>;

  protected:
    /// Access to manager
    ControlsManager *m_manager;

    /// Initializing constructor
    DeviceManager(ControlsManager* mgr, int i, const std::string& nam);
    /// Standard destructor
    virtual ~DeviceManager();

  public:

    /// Access to controls manager type
    ControlsManager* manager() const       {  return m_manager;   }
    /// Check if device exists
    bool exists(const std::string& nam)  const;
    /// Access device by name - if it's not active it is searched....
    std::auto_ptr<Device> device(const std::string& nam);
    /// Create a new device
    std::auto_ptr<Device> createDevice(const std::string& nam, const DevType* typ, bool wait=true);
    /// Create a new device
    std::auto_ptr<Device> createDevice(const std::string& nam, const std::string& dev_typ, bool wait=true);
    /// Delete a device
    bool deleteDevice(Device* device, bool wait=true);
    /// Delete a device
    bool deleteDevice(const std::string& dev_name, bool wait=true);
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DeviceMANAGER_H
