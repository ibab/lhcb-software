// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DeviceSensor.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $
//  ====================================================================
//  DevType.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DeviceSensor.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $

// Framework include files
#include "PVSS/DeviceSensor.h"
#include "PVSS/DeviceManager.h"
#include "PVSS/ControlsManager.h"
#include "PVSS/Internals.h"
#include "CPP/Sensor.h"
#include "CPP/Event.h"
#include "WT/wtdef.h"
#include <set>
using namespace PVSS;

struct DevSensor : public Sensor {
  typedef std::set<DeviceSensor*> SensorMap;
  SensorMap m_clients;
  DevSensor() : Sensor(WT_FACILITY_PVSS,"PVSSDeviceSensor",true) {}
  virtual ~DevSensor() {}
  static DevSensor& instance()  {
    static DevSensor inst;
    return inst;
  }
  virtual void add(DeviceSensor* sensor)  { 
    m_clients.insert(sensor); 
  }
  virtual void remove(DeviceSensor* sensor)  {
    SensorMap::iterator i = m_clients.find(sensor);
    if ( i != m_clients.end() ) {
      m_clients.erase(i);
    }
  }
  virtual void dispatch(void* param) {
    DeviceSensor* sensor = (DeviceSensor*)param;
    sensor->handle();
  }
  virtual void rearm() {
  }
};

/// Default constructor
DeviceSensor::DeviceSensor(ControlsManager* mgr, const std::string& dp_name)  
: m_manager(mgr), m_context(0), m_hotlink(0)
{
  DevSensor::instance().add(this);
  DataPoint dp(mgr,dp_name);
  dp.id();
  m_datapoints.insert(std::make_pair(dp.id(),dp));
  connect();
}

/// Run the sensors
void DeviceSensor::run()  {
  DevSensor::instance().run();
}

/// Initializing constructor
DeviceSensor::DeviceSensor(ControlsManager* mgr, const DataPoint& dp)
: m_manager(mgr), m_context(0), m_hotlink(0)
{
  DevSensor::instance().add(this);
  dp.id();
  m_datapoints.insert(std::make_pair(dp.id(),dp));
  connect();
}

/// Initializing constructor
DeviceSensor::DeviceSensor(ControlsManager* mgr, const std::vector<DataPoint>& dp)
: m_manager(mgr), m_context(0), m_hotlink(0)
{
  DevSensor::instance().add(this);
  for(std::vector<DataPoint>::const_iterator i=dp.begin();i!=dp.end();++i)  {
    (*i).id();
    m_datapoints.insert(std::make_pair((*i).id(),*i));
  }
  connect();
}

/// Standard destructor
DeviceSensor::~DeviceSensor()  {
  disconnect();
  pvss_list_drop(m_context);
  DevSensor::instance().remove(this);
}

/// Disconnect from data points
void DeviceSensor::disconnect()  {
  pvss_list_disconnect(m_context,m_hotlink);
  m_hotlink = 0;
}

/// Connect from data points
void DeviceSensor::connect()  {
  pvss_list_create(m_context);
  for(DataPoints::const_iterator i=m_datapoints.begin(); i!=m_datapoints.end(); ++i)
    pvss_list_add(m_context, (*i).first);
  pvss_list_connect(m_context,this,m_hotlink);
}

/// Handle callback for device group
void DeviceSensor::handleDataUpdate()  {
  ::wtc_insert(WT_FACILITY_PVSS,this);
}

/// Handle callback for device group
void DeviceSensor::handle()  {
  Event ev;
  ev.type = PVSSEvent;
  ev.data = &m_datapoints;
  for(Listeners::iterator i=m_listeners.begin();i!=m_listeners.end();++i)  {
    // Handle each listener
    (*i)->handle(ev);
  }
}

void DeviceSensor::setValue(const DpIdentifier& id, int typ, const Variable* val) {
  DataPoints::iterator i = m_datapoints.find(id);
  if ( i != m_datapoints.end() )  {
    (*i).second.setValue(typ,val);
  }
}

/// Add device listener
void DeviceSensor::addListener(Interactor* listener)   {
  m_listeners.insert(listener);
}

/// Remove listener
void DeviceSensor::removeListener(Interactor* listener)  {
  Listeners::iterator i=m_listeners.find(listener);
  if ( i != m_listeners.end() )
    m_listeners.erase(i);
}

/// Remove all listeners
void DeviceSensor::removeListeners()  {
  m_listeners.clear();
}
