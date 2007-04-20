// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DeviceSensor.cpp,v 1.5 2007-04-20 09:34:19 frankb Exp $
//  ====================================================================
//  DevType.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DeviceSensor.cpp,v 1.5 2007-04-20 09:34:19 frankb Exp $

// Framework include files
#include "PVSS/DeviceSensor.h"
#include "PVSS/DeviceManager.h"
#include "PVSS/ControlsManager.h"
#include "PVSS/Internals.h"
#include "CPP/Sensor.h"
#include "CPP/Event.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"
#include "RTL/rtl.h"
#include <set>
using namespace PVSS;

struct DevSensor : public Sensor {
  typedef std::set<DeviceSensor*> SensorMap;
  SensorMap          m_clients;
  ::lib_rtl_thread_t m_threadH;
  DevSensor() : Sensor(WT_FACILITY_PVSS,"PVSSDeviceSensor",true), m_threadH(0) {}
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
  static int s_runSensor(void* arg)  {
    DevSensor* s = (DevSensor*)arg;
    ::printf("All current and future PVSS Device sensors are now active.\n");
    s->run();
    return 1;
  }
  virtual int runSensor(bool seperate_thread)  {
    if ( seperate_thread && m_threadH == 0 )  {
      int sc = ::lib_rtl_start_thread(s_runSensor,this,&m_threadH);
      if ( !lib_rtl_is_success(sc) )  {
        ::lib_rtl_signal_message(LIB_RTL_OS,"Failed to start device sensor thread.");
      }
      return 1;
    }
    else if ( !seperate_thread )  {
      s_runSensor(this);
      return 1;
    }
    return 0;
  }
};

/// Run the sensors
void DeviceSensor::run()  {
  DevSensor::instance().run();
}

/// Initializing constructor
DeviceSensor::DeviceSensor(ControlsManager* mgr, DataPoint& dp)
: m_manager(mgr), m_context(0), m_hotlink(0)
{
  DevSensor::instance().add(this);
  dp.id();
  m_datapoints.insert(std::make_pair(dp.id(),&dp));
  m_pointArray.push_back(&dp);
  connect();
}

/// Initializing constructor
DeviceSensor::DeviceSensor(ControlsManager* mgr, std::vector<DataPoint>& dp)
: m_manager(mgr), m_context(0), m_hotlink(0)
{
  DevSensor::instance().add(this);
  for(std::vector<DataPoint>::iterator i=dp.begin();i!=dp.end();++i)  {
    (*i).id();
    m_datapoints.insert(std::make_pair((*i).id(),&(*i)));
    m_pointArray.push_back(&(*i));
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
  bool res = pvss_list_disconnect(m_context,m_hotlink);
  if ( res )  {
    // std::cout << "Successfully disconnected from device list." << std::endl;
  }
  else  {
    std::cout << "Failed to disconnected from device list." << std::endl;
  }
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
  ev.data = this;
  for(Listeners::iterator i=m_listeners.begin();i!=m_listeners.end();++i)  {
    // Handle each listener
    (*i)->handle(ev);
  }
}

void DeviceSensor::setValue(const DpID& id, int typ, const Variable* val) {
  DataPoints::iterator i = m_datapoints.find(id);
  if ( i != m_datapoints.end() )  {
    (*i).second->setValue(typ,val);
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

/// Run Sensor instance
int DeviceSensor::run(bool seperate_thread)  {
  return DevSensor::instance().runSensor(seperate_thread);
}
