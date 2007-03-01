// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DevElement.cpp,v 1.1 2007-03-01 10:39:50 frankb Exp $
//  ====================================================================
//  DevElement.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $

// Framework include files
#include "PVSS/DevElement.h"
#include "PVSS/Device.h"
#include "PVSS/DevType.h"

using namespace PVSS;

/// Initializing constructor
DevElement::DevElement(Device* dev, DevTypeElement* elem)
: m_device(dev), m_content(elem)
{
}

/// Add new config to the device element
void DevElement::addConfig(const CfgType* config)   {
}

/// Remove config from the device element
void DevElement::removeConfig(const CfgType* config)   {
}

/// Standard destructor
DevElement::~DevElement()   {
}

/// Access to the element type
DevTypeElement::Type DevElement::contentType() const  {
  return m_content->content();
}

/// Access to the device type
const DevType* DevElement::deviceType() const   {
  return m_device->type();
}

/// Access device type manager
DeviceManager* DevElement::manager() const    { 
  return m_device->manager();   
}

/// Access sub-element by identifier
std::auto_ptr<DevElement> DevElement::element(int id) const   {
  DevElement* e = 0;
  return std::auto_ptr<DevElement>(e);
}

/// Access sub-element by name
std::auto_ptr<DevElement> DevElement::element(const std::string& nam) const   {
  DevElement* e = 0;
  return std::auto_ptr<DevElement>(e);
}
