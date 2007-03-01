// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DevTypeManager.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $
//  ====================================================================
//  DevTypeManager.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DevTypeManager.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $

// Framework include files
#include "PVSS/DevTypeManager.h"
#include "PVSS/DevType.h"
#include <algorithm>
namespace PVSS {
  template <> struct DevManip<DevType>  {
    inline void operator()(const std::pair<std::string,DevType*>& dev)
    { if ( dev.second ) delete dev.second;            }
  };
}

using namespace PVSS;

/// Initializing constructor
DevTypeManager::DevTypeManager(ControlsManager* mgr, DeviceManager* devmgr, int i, const std::string& nam)
: NamedIdentified(i,nam), m_manager(mgr), m_devMgr(devmgr)
{
}

/// Standard destructor
DevTypeManager::~DevTypeManager()   {
  std::for_each(m_types.begin(),m_types.end(),DevManip<DevType>());
}

/// Add new active DevType
bool DevTypeManager::add(DevType* t)  {
  if ( t )  {
    DeviceTypes::iterator i = m_types.find(t->name());
    if (i != m_types.end())  {
      m_types.erase(i);
      m_types.insert(std::make_pair(t->name(),t));
      return false;
    }
    m_typesInt.insert(std::make_pair(t->id(),t));
    m_types.insert(std::make_pair(t->name(),t));
    return true;
  }
  return false;
}

/// Access DevType by name - if it's not active it is searched....
const DevType* DevTypeManager::type(const std::string& nam)  const {
  DeviceTypes::const_iterator i = m_types.find(nam);
  return (i == m_types.end()) ? 0 : (*i).second;
}

/// Access DevType by name - if it's not active it is searched....
const DevType* DevTypeManager::type(int dev_id)  const {
  DeviceTypesInt::const_iterator i = m_typesInt.find(dev_id);
  return (i == m_typesInt.end()) ? 0 : (*i).second;
}
