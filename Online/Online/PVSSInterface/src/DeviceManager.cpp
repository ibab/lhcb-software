// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DeviceManager.cpp,v 1.5 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  DeviceManager.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DeviceManager.cpp,v 1.5 2007-10-01 14:46:55 frankm Exp $

// Framework include files
#include "PVSS/ControlsManager.h"
#include "PVSS/DevTypeManager.h"
#include "PVSS/DeviceManager.h"
#include "PVSS/DevType.h"
#include "PVSS/Device.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/Internals.h"
#include <algorithm>
#include <cstdio>

namespace PVSS {
  template <> struct DevManip<Device>  {
    static inline Device* create(DeviceManager* m,const DevType* typ, const std::string& n)  
    {  return new Device(m,typ,n);                 }
    inline void operator()(const std::pair<std::string,Device*>& dev)
    {      if ( dev.second ) delete dev.second;    }
  };
}

using namespace PVSS;

/// Initializing constructor
DeviceManager::DeviceManager(ControlsManager* mgr, int i, const std::string& nam)
: NamedIdentified(i,nam), m_manager(mgr)
{
}

/// Standard destructor
DeviceManager::~DeviceManager()   {
}

/// Check if device exists
bool DeviceManager::exists(const std::string& nam)  const  {
  DpID id(0);
  return pvss_lookup_dpid(nam.c_str(), id);
}

/// Access device by name - if it's not active it is searched....
std::auto_ptr<Device> DeviceManager::device(const std::string& nam)  {
  DpID id(0);
  if ( pvss_lookup_dpid(nam.c_str(), id) )  {
    const DevType* typ = manager()->typeMgr()->type(id.type());
    return std::auto_ptr<Device>(DevManip<Device>::create(this,typ,nam));
  }
  return std::auto_ptr<Device>(0);
}

/// Create a new device
std::auto_ptr<Device> 
DeviceManager::createDevice(const std::string& nam, const DevType* typ, bool wait)  {
  if ( typ )  {
    if ( !exists(nam) )  {
      DevAnswer a;
      std::string dev = ControlsManager::dpName(nam);
      if ( pvss_debug() > 0 ) {
	::printf("PVSS> DeviceManager::createDevice> '%s' of type '%s'\n",
		 dev.c_str(),typ->name().c_str());
      }
      bool ret = pvss_create_device(dev.c_str(),typ->id(),id(),wait ? &a : 0);
      if ( ret )  {
        return std::auto_ptr<Device>(DevManip<Device>::create(this,typ,nam));
      }
      a.print();
    }
  }
  return std::auto_ptr<Device>(0);
}

/// Create a new device
std::auto_ptr<Device> 
DeviceManager::createDevice(const std::string& nam, const std::string& dev_typ, bool wait)  {
  return createDevice(nam,manager()->typeMgr()->type(dev_typ), wait);
}

/// Delete a device
bool DeviceManager::deleteDevice(Device* device, bool wait)  {
  if ( device )  {
    if ( exists(device->name()) )  {
      DevAnswer a;
      bool ret = pvss_delete_device(device->id(),id(),wait ? &a : 0);
      if ( !ret ) a.print();
      return ret;
    }
  }
  return false;
}

/// Delete a device
bool DeviceManager::deleteDevice(const std::string& dev_name, bool wait)  {
  DevAnswer a;
  DpID dpid(0);
  if ( pvss_lookup_dpid(dev_name.c_str(),dpid) )  {
    bool ret = pvss_delete_device(dpid,id(),wait ? &a : 0);
    if ( !ret ) a.print();
    return ret;
  }
  return false;
}
