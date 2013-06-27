// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DeviceListener.cpp,v 1.3 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  DevType.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DeviceListener.cpp,v 1.3 2007-10-01 14:46:55 frankm Exp $

// Framework include files
#include "PVSS/DeviceListener.h"
#include "PVSS/DeviceSensor.h"
#include "CPP/Event.h"
// C++ include files
#include <cstdio>
#include <iostream>
#include <stdexcept>
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
      DevDesc& devices = m_sensor->devices();
      DevDesc& invalid = m_sensor->invalidDevices();
      for(DevDesc::iterator i=invalid.begin(); i!=invalid.end();++i)  {
        m_dp = (*i).second;
        handleInvalidDevice();
      }
      for(DevDesc::iterator j=devices.begin(); j!=devices.end();++j)  {
        m_dp = (*j).second;
        handleDevice();
      }
      handleDevices();
    }
  }
  catch(std::exception& e) {
    ::printf("PVSS> Exception occurred while executing DeviceListener callbacks:\n%s\n",e.what());
  }
  catch(...) {
    ::printf("PVSS> Unknown exception occurred while executing DeviceListener callbacks.\n");
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

/// Interface callback to handle single devices one by one
void DeviceListener::handleInvalidDevice()  {
}
