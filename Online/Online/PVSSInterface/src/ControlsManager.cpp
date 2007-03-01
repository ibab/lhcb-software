// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/ControlsManager.cpp,v 1.1 2007-03-01 10:39:50 frankb Exp $
//  ====================================================================
//  ControlsManager.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $
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
    { return DevManip<DevTypeManager>::add(m,new DevType(m,id,n));     }
    static void add(DevType* t, DevTypeElement* e)  {  t->add(e); }
  };
  template <> struct DevManip<DevTypeElement>  {
    static void add(DevType* t, int id, int typ, CCHAR n)
    { DevManip<DevType>::add(t,new DevTypeElement(t,id,n,(DevTypeElement::Type)typ));     }
  };
}

/// Initializing constructor
ControlsManager::ControlsManager(int i, const std::string nam)
: NamedIdentified(i,nam), m_cfgMgr(0), m_devMgr(0), m_devTypeMgr(0)
{
  ::printf("PVSS> Create controls manager for system[%d] %s\n",i,nam.c_str());
  m_cfgMgr = CfgManip<CfgManager>::create(this,i,nam);
  pvss_load_configurations(m_cfgMgr,i,
                     CfgManip<CfgType>::add,
                     CfgManip<CfgAttribute>::add,
                     CfgManip<CfgDetail>::add);
  m_devMgr = DevManip<DeviceManager>::create(this,i,nam);
  m_devTypeMgr = DevManip<DevTypeManager>::create(this,m_devMgr,i,nam);
  pvss_load_device_types(m_devTypeMgr,i,DevManip<DevType>::add,DevManip<DevTypeElement>::add);
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


