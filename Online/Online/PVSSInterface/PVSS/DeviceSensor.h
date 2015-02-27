// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DeviceSensor.h,v 1.3 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  DeviceSensor.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DEVLISTENER_H
#define ONLINE_PVSS_DEVLISTENER_H

// Framework include files
#include "PVSS/Kernel.h"
#include "PVSS/DataPoint.h"
#include "PVSS/HotLinkCallback.h"
#include "CPP/Interactor.h"

// C++ include files
#include <vector>
#include <map>
#include <set>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class ControlsManager;
  class DevAnswer;


  /** @class DeviceSensor DeviceSensor.h PVSS/DeviceSensor.h
    *
    *  PVSS DeviceSensor descriptor.
    *
    *  Note:
    *  One sensor can handle only ONE device group
    *  Dynamic reconfiguration is not possible.
    *
    *  The datapoints must be held by the client object,
    *  internally only pointers are held
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DeviceSensor : public HotLinkCallback  {
  public:
    // Type definitions
    typedef std::map<DpID,DataPoint*>  DataPoints;
    typedef std::vector<DataPoint*>    DataPointArray;
    typedef std::set<CPP::Interactor*> Listeners;

  protected:
    /// Reference to controls manager
    ControlsManager*  m_manager;
    /// Device datapoints
    DataPoints        m_datapoints;
    /// Datapoint array containing invalid entries
    DataPoints        m_invalid;
    /// Datapoint array
    DataPointArray    m_pointArray;
    /// Device listeners
    Listeners         m_listeners;
    /// PVSS list context
    void*             m_context;
    /// PVSS hotlink context
    void*             m_hotlink;
    /// Disconnect from data points
    void disconnect();
    /// Connect from data points
    void connect();
  public:
    /// Initializing constructor
    DeviceSensor(ControlsManager* mgr, DataPoint& dp);
    /// Initializing constructor
    DeviceSensor(ControlsManager* mgr, std::vector<DataPoint>& dp);
    /// Standard destructor
    virtual ~DeviceSensor();
    /// Run the sensors
    static void run();
    /// HotLinkCallback overload: Handle callback for device group
    virtual void handleDataUpdate();
    /// HotLinkCallback overload: Set data value
    virtual void setValue(const DpID& dpid, int typ, const Variable* val);
    /// Delayed handling callback for device group
    virtual void handle();
    /// Access DeviceSensor type manager
    ControlsManager* manager() const          {  return m_manager;    }
    /// Access to the device map
    DataPoints& devices()                     {  return m_datapoints; }
    /// Access to the device map (CONST)
    const DataPoints& devices() const         {  return m_datapoints; }
    /// Access to the device map of invalid devices
    DataPoints& invalidDevices()              {  return m_invalid;    }
    /// Access to the device map of invalid devices (CONST)
    const DataPoints& invalidDevices() const  {  return m_invalid;    }
    /// Access to the device array
    DataPointArray& deviceArray()             {  return m_pointArray; }
    /// Access to the device array (CONST)
    const DataPointArray& deviceArray() const {  return m_pointArray; }
    /// Add device listener
    void addListener(CPP::Interactor* listener);
    /// Remove listener
    void removeListener(CPP::Interactor* listener);
    /// Remove all listeners
    void removeListeners();
    /// Run Sensor instance
    int run(bool seperate_thread);
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DEVLISTENER_H
