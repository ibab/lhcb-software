// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DeviceIO.cpp,v 1.4 2007-03-02 19:54:05 frankb Exp $
//  ====================================================================
//  DeviceIO.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DeviceIO.cpp,v 1.4 2007-03-02 19:54:05 frankb Exp $

// Framework include files
#include "PVSS/DevTypeElement.h"
#include "PVSS/Internals.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/DeviceIO.h"
#include "PVSS/PVSSIO.h"
#include <stdexcept>

using namespace PVSS;

static DpIdentifier lookup(const std::string& nam)  {
  DpIdentifier onl(0);
  if ( !pvss_lookup_dpid(DataPoint::online(nam).c_str(),onl) )    {
    throw std::invalid_argument("The datapoint:"+nam+" does not exist!");
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
  pvss_list_drop(m_context);
}

/// Execute PVSS IO action
bool DeviceIO::Read::exec(bool keep_list, DeviceIO* par, DevAnswer* a) {
  const DataPoints& p = par->points();
  if ( !m_context )  {
    pvss_list_create(m_context);
    for(DataPoints::const_iterator i=p.begin(); i != p.end(); ++i)
      pvss_list_add(m_context,(*i).first);
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
}

/// Execute PVSS IO action
bool DeviceIO::Write::exec(bool keep_list, DeviceIO* par, DevAnswer* a)  {
  const DataPoints& p = par->points();
  void* listCtxt = 0;
  if ( !m_context )  {
    pvss_val_list_create(m_context);
  }
  for(DataPoints::const_iterator i=p.begin(); i != p.end(); ++i)  {
    const Value* val = (*i).second->value();
    if ( !val )  {
      throw std::runtime_error("All datapoints must be set to perform a write action. "
        " Missing is:"+(*i).second->name());
    }
    setGenWriteIO(m_context,listCtxt,(*i).first,val);
  }
  pvss_exec_dpset(m_context,a,keep_list);
  return true;
}

/// Initializing constructor for reading
DeviceIO::DeviceIO(ControlsManager* mgr, const Read& ) 
: m_manager(mgr)
{
  setRead();
}

/// Initializing constructor for writing
DeviceIO::DeviceIO(ControlsManager* mgr, const Write& ) 
: m_manager(mgr)
{
  setWrite();
}

/// Standard destructor
DeviceIO::~DeviceIO()   {
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
void DeviceIO::setValue(const DpIdentifier& dpid, int typ, const Variable* val)  {
  DataPoints::iterator i=m_points.find(dpid);
  if ( i != m_points.end() )  {
    DataPoint* dp = (*i).second;
    dp->setValue(typ,val);
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
    return a.state() == DevAnswer::FINISHED;
  }
  m_devIO->exec(keep_list, this, 0);
  if ( !keep_list ) m_points.clear();
  return true;
}

/// Restart dpset list (scratch old list if present)
bool DeviceIO::start()  {
  clear();
  return true;
}
/// Clear the datapoint container
void DeviceIO::clear()   {
  m_points.clear();
  if ( m_devIO->context() ) m_devIO->dropList();
}

/// add datapoint value
void DeviceIO::i_add(const DpIdentifier& id, DataPoint& dp) {
  m_points.insert(std::make_pair(id,&dp));
  if ( m_devIO->context() ) m_devIO->dropList();
}

/// add datapoint value by name
void DeviceIO::add(DataPoint& dp) {
  i_add(dp.id(),dp);
}

/// add datapoint value by name
void DeviceIO::addOnline(DataPoint& dp) {
  i_add(lookup(DataPoint::online(dp.name())),dp);
}

/// add datapoint value by name
void DeviceIO::addOriginal(DataPoint& dp) {
  i_add(lookup(DataPoint::original(dp.name())),dp);
}

/// add datapoint values
void DeviceIO::add(std::vector<DataPoint>& dp)  {
  std::for_each(dp.begin(),dp.end(),addData());
}

/// add original datapoint values
void DeviceIO::addOriginal(std::vector<DataPoint>& dp)  {
  std::for_each(dp.begin(),dp.end(),addOriginalData());
}

/// add online datapoint values
void DeviceIO::addOnline(std::vector<DataPoint>& dp)  {
  std::for_each(dp.begin(),dp.end(),addOnlineData());
}
