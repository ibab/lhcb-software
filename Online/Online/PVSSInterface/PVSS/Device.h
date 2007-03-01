// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/Device.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  Device.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DEVICE_H
#define ONLINE_PVSS_DEVICE_H

// Framework include files
#include "PVSS/DataPoint.h"
#include "PVSS/DevType.h"
#include <map>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class DeviceManager;
  class DevType;

  /** @class Device   Device.h  PVSS/Device.h
    *
    *  PVSS device descriptor.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class Device : public DataPoint {
  public:
    friend struct DevManip<Device>;

  protected:
    /// Access to manager
    DeviceManager *m_manager;
    /// Access to device type
    const DevType *m_type;
    /// Initializing constructor
    Device(DeviceManager* mgr, const DevType *typ, const std::string& nam);

  public:
    /// Standard destructor
    virtual ~Device();
    /// Access device type manager
    DeviceManager* manager() const    {  return m_manager;   }
    /// Access to device description
    const DevType* type() const       {  return m_type;      }
    /// Access to elements (only present after loading
    const DevTypeElement::Elements& elements()  const {
      return m_type->elements();
    }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DEVICE_H
