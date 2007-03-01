// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DeviceSensor.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
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

class Interactor;

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
    *
    *  One sensor can handle only ONE device group
    *  Dynamic reconfiguration is not possible.
    *
    *  If the datapoint to be watched is NOT a primitive 
    *  type, the primitive leaves will be added.
    *
    *  This may be unwanted in case there are many
    *  leaves - since the callback includes all leaves
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DeviceSensor : public HotLinkCallback  {
    typedef std::map<DpIdentifier,DataPoint> DataPoints;
    typedef std::set<Interactor*>  Listeners;

    /// Reference to controls manager
    ControlsManager* m_manager;
    /// Device datapoints
    DataPoints  m_datapoints;
    /// Device listeners
    Listeners   m_listeners;
    /// PVSS list context
    void*       m_context;
    /// PVSS hotlink context
    void*       m_hotlink;
    /// Disconnect from data points
    void disconnect();
    /// Connect from data points
    void connect();
  public:
    /// Default constructor
    DeviceSensor(ControlsManager* mgr, const std::string& dp);
    /// Initializing constructor
    DeviceSensor(ControlsManager* mgr, const DataPoint& dp);
    /// Initializing constructor
    DeviceSensor(ControlsManager* mgr, const std::vector<DataPoint>& dp);
    /// Standard destructor
    virtual ~DeviceSensor();
    /// Run the sensors
    static void run();
    /// HotLinkCallback overload: Handle callback for device group
    virtual void handleDataUpdate();
    /// HotLinkCallback overload: Set data value
    virtual void setValue(const DpIdentifier& dpid, int typ, const Variable* val);
    /// Delayed handling callback for device group
    virtual void handle();
    /// Access DeviceSensor type manager
    ControlsManager* manager() const    {  return m_manager;   }
    /// Add device listener
    void addListener(Interactor* listener);
    /// Remove listener
    void removeListener(Interactor* listener);
    /// Remove all listeners
    void removeListeners();
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DEVLISTENER_H
