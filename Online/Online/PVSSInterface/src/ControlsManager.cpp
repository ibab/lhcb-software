// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/ControlsManager.cpp,v 1.8 2008-03-19 08:07:47 frankm Exp $
//  ====================================================================
//  ControlsManager.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: ControlsManager.cpp,v 1.8 2008-03-19 08:07:47 frankm Exp $
//#define _UseSpecializedManip

// Framework include files
#include "PVSS/ControlsManager.h"
#include "PVSS/DeviceManager.h"
#include "PVSS/DeviceIO.h"
#include "PVSS/DevTypeManager.h"
#include "PVSS/CfgManager.h"
#include "PVSS/CfgType.h"
#include "PVSS/CfgAttribute.h"
#include "PVSS/CfgDetail.h"
#include "PVSS/DevType.h"
#include "PVSS/Printer.h"
#include "PVSS/Internals.h"
#include "RTL/rtl.h"
#include <cstring>

using namespace PVSS;

namespace PVSS {
  typedef const std::string& CSTR;
  typedef const char*        CCHAR;
  template <> struct CfgManip<CfgManager>  {
    static CfgManager* create(ControlsManager* m,int id, CSTR n)  
      {  return new CfgManager(m,id,n);     }
    static void release(CfgManager*& p)  { if ( p ) {delete p; p = 0;}}
    static void addConfig(CfgManager* m, CfgType* c) { m->addConfig(c); }
  };
  template <> struct CfgManip<CfgType>  {
    public:
    static void add(CfgManager* m,int id, CCHAR n)  { 
      //std::cout << "Add Cgf:" << id << " " << n << std::endl;
      CfgManip<CfgManager>::addConfig(m,new CfgType(m,id,n));  
    }
    static void addA(CfgType* m,CfgAttribute* n)  { m->addAttribute(n);  }
    static void addD(CfgType* m,CfgDetail* n)     { m->addDetail(n);     }
  };
  template <> struct CfgManip<CfgAttribute>  {
    static void add(CfgManager* m,int id, CCHAR n)    {
      CfgType* t=const_cast<CfgType*>(m->configurations().back());
      //std::cout << "  Add attr:" << id << " " << n << std::endl;
      CfgManip<CfgType>::addA(t,new CfgAttribute(t,id,n));    
    }
  };
  template <> struct CfgManip<CfgDetail>  {
    static void add(CfgManager* m,int id, CCHAR n)    {
      CfgType* t=const_cast<CfgType*>(m->configurations().back());
      //std::cout << "  Add detail:" << id << " " << n << std::endl;
      CfgManip<CfgType>::addD(t,new CfgDetail(t,id,n));
    }
  };
  template <> struct CfgManip<ControlsManager>  {
    static ControlsManager* create(int id, CCHAR n)
    {      return new ControlsManager(id, n);  }
  };
  template <> struct DevManip<DeviceManager>  {
    static DeviceManager* create(ControlsManager* m,int id, CSTR n)  
      {  return new DeviceManager(m,id,n);     }
    static void release(DeviceManager*& p)  { if ( p ) {delete p; p = 0;}}
  };
  template <> struct DevManip<DevTypeManager>  {
    static DevTypeManager* create(ControlsManager* mgr, DeviceManager* devmgr, int id, CSTR nam)  
      {  return new DevTypeManager(mgr,devmgr,id,nam);     }
    static void release(DevTypeManager*& p)  { if ( p ) {delete p; p = 0;}}
    static DevType* add(DevTypeManager* m, DevType* t)  {  m->add(t); return t; }
  };
  template <> struct DevManip<DevType>  {
    static DevType* add(DevTypeManager* m,int id, CCHAR n) 
    { return DevManip<DevTypeManager>::add(m,new DevType(m,id,n,0));     }
    static void add(DevType* t, DevTypeElement* e)  {  t->add(e); }
  };
  template <> struct DevManip<DevTypeElement>  {
    static void add(DevType* t, int id, int typ, CCHAR n)
    { DevManip<DevType>::add(t,new DevTypeElement(t,id,n,typ));     }
  };
}

/// Initializing constructor
ControlsManager::ControlsManager(int i, const std::string nam)
: NamedIdentified(i,nam), m_cfgMgr(0), m_devMgr(0), m_devTypeMgr(0), m_logger(0)
{
  ::printf("PVSS> Create controls manager for system[%d] %s on %s\n",i,nam.c_str(),hostName().c_str());
  m_cfgMgr = CfgManip<CfgManager>::create(this,i,nam);
  pvss_load_configurations(m_cfgMgr,i,
                     CfgManip<CfgType>::add,
                     CfgManip<CfgAttribute>::add,
                     CfgManip<CfgDetail>::add);
  m_devMgr = DevManip<DeviceManager>::create(this,i,nam);
  m_devTypeMgr = DevManip<DevTypeManager>::create(this,m_devMgr,i,nam);
  pvss_load_device_types(m_devTypeMgr,i,DevManip<DevType>::add,DevManip<DevTypeElement>::add);
  m_logger = std::auto_ptr<Printer>(createLogger());
  if ( pvss_debug() ) m_logger->print(*this);
}

/// Standard destructor
ControlsManager::~ControlsManager()   {
  CfgManip<CfgManager>::release(m_cfgMgr);
  DevManip<DeviceManager>::release(m_devMgr);
}

/// Create transaction object
std::auto_ptr<WriteTransaction> ControlsManager::writeTransaction()  {
  return std::auto_ptr<WriteTransaction>(new WriteTransaction(this));
}

/// Create transaction object
std::auto_ptr<ReadTransaction> ControlsManager::readTransaction()  {
  return std::auto_ptr<ReadTransaction>(new ReadTransaction(this));
}

/// Create device reader to read device data from PVSS
std::auto_ptr<DeviceIO> ControlsManager::devReader()    {
  return std::auto_ptr<DeviceIO>(new DeviceIO(this,DeviceIO::Read()));
}

/// Create device writer to set device data to PVSS
std::auto_ptr<DeviceIO> ControlsManager::devWriter()    {
  return std::auto_ptr<DeviceIO>(new DeviceIO(this,DeviceIO::Write()));
}

/// Full data point name with system name
std::string ControlsManager::dpFullName(const std::string& nam)  const {
  std::string::size_type idx1 = nam.find(":");
  if ( idx1 == std::string::npos )
    return name()+":"+nam;
  else {
    std::string::size_type idx2 = nam.find(".");
    if ( idx2 != std::string::npos && idx1 > idx2 )
      return name()+":"+nam;
    else if ( 0 != ::strncmp(nam.c_str(),name().c_str(),name().length()) ) {
      return name()+":"+dpName(nam);
    }
  }
  return nam;
}

/// Extract name of datapoint from online/original name
std::string ControlsManager::dpName(const std::string& dp)    {
  std::string::size_type id1 = dp.find(":");
  std::string::size_type id2 = dp.rfind(":");
  if ( id1 == std::string::npos && id2 == std::string::npos )
    return dp;
  else if ( id2 > id1 )
    return dp.substr(0,id2);
  else if ( id1 == id2 )  {
    std::string s = dp.substr(id1,3);
    if( s == ":_o")
      return dp.substr(0,id1);
    else
      return dp.substr(id1+1);
  }
  return dp;
}

/// Extract system name of datapoint from online/original name
std::string ControlsManager::dpSystemName(const std::string& dp)   {
  std::string::size_type id1 = dp.find(":");
  std::string::size_type id2 = dp.rfind(":",id1+1);
  if ( id1 == std::string::npos && id2 == std::string::npos )
    return "";
  else if ( id2 == std::string::npos )
    return dp.substr(0,id1);
  return dp.substr(0,id1);
}

/// Extract system name of datapoint from online/original name
std::string ControlsManager::dpElementName(const std::string& dp)   {
  std::string::size_type id1 = dp.find(":");
  std::string::size_type id2 = dp.find(".",id1+1);
  if ( id1 == std::string::npos && id2 == std::string::npos )
    return dp;
  else if ( id2 == id1 )
    return dp.substr(id1+1);
  return dp.substr(id1+1,id2-id1-1);
}

/// Extract system name of datapoint from online/original name
std::string ControlsManager::dpSysElementName(const std::string& dp)   {
  return dpSystemName(dp)+":"+dpElementName(dp);
}

/// Extract system name of datapoint from online/original name
std::string ControlsManager::hostName()  const {
  static std::string host = eventHostName();
  if ( host == "localhost" ) {
    host = RTL::nodeName();
  }
  return host;
}
