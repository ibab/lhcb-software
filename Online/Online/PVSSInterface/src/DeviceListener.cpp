// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DeviceListener.cpp,v 1.1 2007-03-12 10:07:24 frankb Exp $
//  ====================================================================
//  DevType.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DeviceListener.cpp,v 1.1 2007-03-12 10:07:24 frankb Exp $

// Framework include files
#include "PVSS/DeviceListener.h"
#include "PVSS/DeviceSensor.h"
#include "CPP/Event.h"
#include <iostream>

using namespace PVSS;

/// Initializing constructor
DeviceListener::DeviceListener(ControlsManager* m)
: m_manager(m), m_sensor(0), m_dp(0)
{
}

/// Standard destructor
DeviceListener::~DeviceListener() {
}

/// Access to current device list (Only valid during callback!)
DeviceListener::DevDesc& DeviceListener::devices()  const  {
  if ( m_sensor ) return m_sensor->devices();
  throw std::runtime_error("[Possible Internal error] No active sensor!");
}

/// Access to device vector
DeviceListener::DevArray& DeviceListener::devArray()  const  {
  if ( m_sensor ) return m_sensor->deviceArray();
  throw std::runtime_error("[Possible Internal error] No active sensor!");
}

/// Access to current device (Only valid during callback!)
DataPoint& DeviceListener::dp() const  {
  if ( m_dp ) return *m_dp;
  throw std::runtime_error("[Possible Internal error] datapoint variable is not set!");
}

/// Sensor callback
void DeviceListener::handle(const Event& ev)  {
  try {
    if ( ev.type == PVSSEvent )  {
      m_sensor = (DeviceSensor*)ev.data;
      DevDesc& d = m_sensor->devices();
      handleDevices();
      for(DevDesc::iterator i=d.begin(); i!=d.end();++i)  {
        m_dp = (*i).second;
        handleDevice();
      }
    }
  }
  catch(std::exception& e) {
    ::printf("Exception occurred while executing DeviceListener callbacks:\n%s\n",e.what());
  }
  catch(...) {
    ::printf("Unknown exception occurred while executing DeviceListener callbacks.\n");
  }
  m_sensor = 0;
  m_dp = 0;
}

/// Interface callback to handle all devices
void DeviceListener::handleDevices()  {
}

/// Interface callback to handle single devices one by one
void DeviceListener::handleDevice()  {
}
