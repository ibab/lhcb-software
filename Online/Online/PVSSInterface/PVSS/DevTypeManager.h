// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DevTypeManager.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  DevTypeManager.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DevTypeManager_H
#define ONLINE_PVSS_DevTypeManager_H

// Framework include files
#include "PVSS/NamedIdentified.h"

// C++ include files
#include <map>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class ControlsManager;
  class DeviceManager;
  class DevType;
  class Device;

  /** @class DevTypeManager   DevTypeManager.h  PVSS/DevTypeManager.h
    *
    *  Manager for active (=known) PVSS devices.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DevTypeManager  : public NamedIdentified {
  public:
    friend struct DevManip<DevTypeManager>;

    /// Internal type definitions: configurations container
    typedef std::map<std::string,DevType*> DeviceTypes;
    typedef std::map<int,DevType*>         DeviceTypesInt;

  protected:
    /// Access to manager
    ControlsManager *m_manager;
    /// Access to device manager
    DeviceManager   *m_devMgr;
    /// Device type container indexed by name
    DeviceTypes      m_types;
    /// Device type container indexed by identifier
    DeviceTypesInt   m_typesInt;

    /// Initializing constructor
    DevTypeManager(ControlsManager* mgr, DeviceManager* devmgr, int i, const std::string& nam);
    /// Standard destructor
    virtual ~DevTypeManager();

    /// Add new active device
    bool add(DevType* dev);

  public:

    /// Access to controls manager type
    ControlsManager* manager() const       {  return m_manager;   }
    /// Access to device manager
    DeviceManager*   devManager() const    {  return m_devMgr;    }
    /// Client access to attributes
    const DeviceTypes& types()  const      {  return m_types;     }
    /// Access device by name - if it's not active it is searched....
    const DevType* type(const std::string& nam)  const;
    /// Access device by id
    const DevType* type(int type_id)  const;
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DevTypeManager_H
