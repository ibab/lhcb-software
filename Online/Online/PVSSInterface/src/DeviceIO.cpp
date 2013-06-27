// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DeviceIO.cpp,v 1.10 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  DeviceIO.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DeviceIO.cpp,v 1.10 2007-10-01 14:46:55 frankm Exp $

// Framework include files
#include "PVSS/ControlsManager.h"
#include "PVSS/DevTypeElement.h"
#include "PVSS/Internals.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/DeviceIO.h"
#include "PVSS/PVSSIO.h"
#include <stdexcept>
#include <cstdio>

using namespace PVSS;

static DpID lookup(ControlsManager* manager,const std::string& name)  {
  DpID onl(0);
  std::string nam = manager->dpFullName(DataPoint::online(name));
  if ( !pvss_lookup_dpid(nam.c_str(),onl) )    {
    throw std::invalid_argument("DeviceIO> The datapoint:"+nam+" does not exist!");
  }
  return onl;
}

/// Default constructor
DeviceIO::Read::Read() : IDevIO()  {
}

/// Default destructor
DeviceIO::Read::~Read()  {
  if ( m_context ) dropList();
}

/// Drop PVSS IO list
void DeviceIO::Read::dropList()  {
  if ( m_context ) pvss_list_drop(m_context);
}

/// Execute PVSS IO action
bool DeviceIO::Read::exec(bool keep_list, DeviceIO* par, DevAnswer* a) {
  const DataPoints& p = par->points();
  if ( !m_context )  {
    pvss_list_create(m_context);
    for(DataPoints::const_iterator i=p.begin(); i != p.end(); ++i)  {
      pvss_list_add(m_context,(*i).first);
      (*i).second->setFlag(1,2);
    }
  }
  pvss_exec_read(m_context,a,par,keep_list);
  return true;
}

/// Default constructor
DeviceIO::Write::Write() : IDevIO()  {
}

/// Default destructor
DeviceIO::Write::~Write()  {
  if ( m_context ) pvss_val_list_drop(m_context);
}

/// Drop PVSS IO list
void DeviceIO::Write::dropList()  {
  pvss_val_list_drop(m_context);
  m_context = 0;
}

/// Execute PVSS IO action
bool DeviceIO::Write::exec(bool keep_list, DeviceIO* par, DevAnswer* a)  {
  const DataPoints& p = par->points();
  void* listCtxt = (void*)~0x0;
  if ( !m_context )  {
    pvss_val_list_create(m_context);
  }
  for(DataPoints::const_iterator i=p.begin(); i != p.end(); ++i)  {
    if ( (*i).second ) {
      const Value* val = (*i).second->value();
      if ( !val )  {
	std::string err = "All datapoints must be set to perform a write action. ";
	err +=  " Missing is:"+(*i).second->name();
	::printf("PVSS> %s\n",err.c_str());
	throw std::runtime_error(err);
      }
      (*i).second->setFlag(1,2);
      setGenWriteIO(m_context,listCtxt,(*i).first,val);
    }
    else {
      ::printf("PVSS> Invalid datapoint in transaction!!!!!\n");
    }
  }
  pvss_exec_dpset(m_context,a,keep_list);
  return true;
}

/// Initializing constructor for reading
DeviceIO::DeviceIO(ControlsManager* mgr, const Read& ) : m_manager(mgr)  {
  setRead();
}

/// Initializing constructor for writing
DeviceIO::DeviceIO(ControlsManager* mgr, const Write& ) : m_manager(mgr)  {
  setWrite();
}

/// Standard destructor
DeviceIO::~DeviceIO()   {
}

/// Access to copy of datapoints
std::vector<DataPoint*> DeviceIO::pointVector() const {
  std::vector<DataPoint*> v;
  const DataPoints& p = points();
  for(DataPoints::const_iterator i=p.begin(); i != p.end(); ++i)  {
    v.push_back((*i).second);
  }
  return v;
}

/// Change Device IO to writing mode
void DeviceIO::setWrite()   {
  m_devIO = std::auto_ptr<IDevIO>(new Write);
}

/// Change Device IO to writing mode
void DeviceIO::setRead()    {  
  m_devIO = std::auto_ptr<IDevIO>(new Read);
}

/// HotLinkCallback overload: Handle callback for device group
void DeviceIO::handleDataUpdate()  {
}

/// HotLinkCallback overload: Set data value
void DeviceIO::setValue(const DpID& dpid, int typ, const Variable* val)  {
  DataPoints::iterator i=m_points.find(dpid);
  if ( i != m_points.end() )  {
    DataPoint* dp = (*i).second;
    dp->setValue(typ,val);
    dp->setFlag(1,3);
  }
  else  {
    ::printf("PVSS> Attempt to set datapoint, which is not contained in the transaction.\n");
  }
}

/// Execute dpset list
bool DeviceIO::execute(bool keep_list, bool wait)  {
  if ( wait )  {
    DevAnswer a;
    m_devIO->exec(keep_list, this, &a);
    if ( a.state() != DevAnswer::FINISHED )  {
      a.print();
    }
    if ( !keep_list ) m_points.clear();
    return a.state() == DevAnswer::FINISHED;
  }
  m_devIO->exec(keep_list, this, 0);
  if ( !keep_list ) m_points.clear();
  return true;
}

/// Length of known datapoints
size_t DeviceIO::length() const
{  return m_points.size();                                  }

/// Clear the datapoint container
void DeviceIO::clear()   {
  m_points.clear();
  if ( m_devIO->context() ) m_devIO->dropList();
}

/// add datapoint value
void DeviceIO::i_add(const DpID& id, DataPoint& dp) {
  if ( m_devIO->context() ) m_devIO->dropList();
  m_points.insert(std::make_pair(id,&dp));
  dp.setFlag(1,1);
}

/// add datapoint value by name
void DeviceIO::add(DataPoint& dp) 
{  i_add(dp.id(),dp);                                       }

/// add datapoint value by name
void DeviceIO::addOnline(DataPoint& dp) 
{  i_add(lookup(m_manager,DataPoint::online(dp.name())),dp);          }

/// add datapoint value by name
void DeviceIO::addOriginal(DataPoint& dp) 
{  i_add(lookup(m_manager,DataPoint::original(dp.name())),dp);        }

/// add datapoint values
void DeviceIO::add(std::vector<DataPoint>& dp)  
{  std::for_each(dp.begin(),dp.end(),addData());            }

/// add original datapoint values
void DeviceIO::addOriginal(std::vector<DataPoint>& dp)
{  std::for_each(dp.begin(),dp.end(),addOriginalData());    }

/// add online datapoint values
void DeviceIO::addOnline(std::vector<DataPoint>& dp)
{  std::for_each(dp.begin(),dp.end(),addOnlineData());      }
