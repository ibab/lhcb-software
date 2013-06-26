// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DevElement.h,v 1.2 2007-03-02 12:19:02 frankb Exp $
//  ====================================================================
//  DevElement.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DEVELEMENT_H
#define ONLINE_PVSS_DEVELEMENT_H

// Framework include files
#include "PVSS/NamedIdentified.h"

// C++ include files
#include <vector>
#include <memory>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class DeviceManager;
  class Device;
  class DevTypeElement;
  class DevType;
  class CfgType;

  /** @class DevElement   DevElement.h  PVSS/DevElement.h
    *
    *  PVSS device type element.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DevElement : public NamedIdentified {
  public:
    friend struct DevManip<DevElement>;
    friend class std::auto_ptr<DevElement>;
    /// Internal type definitions: device element container
    typedef std::vector<DevElement*> Elements;
    typedef std::auto_ptr<DevElement> Holder;

  protected:
    /// Access to device type
    Device         *m_device;
    /// Element type
    DevTypeElement *m_content;

    /// Initializing constructor
    DevElement(Device* dev, DevTypeElement* elem);

    /// Standard destructor
    virtual ~DevElement();

  public:
    /// Access sub-element by identifier
    Holder element(int id) const;
    /// Access sub-element by name
    Holder element(const std::string& nam) const;

    /// Access device type manager
    DeviceManager* manager() const;
    /// Access to device type
    Device* device() const            { return m_device;    }
    /// Access the device type element
    DevTypeElement* type() const      { return m_content;   }
    /// Access to the device type
    const DevType* deviceType() const;
    /// Access to the element type
    int contentType() const;
    /// Add new config to the device element
    void addConfig(const CfgType* config);
    /// Remove config from the device element
    void removeConfig(const CfgType* config);
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DEVELEMENT_H
