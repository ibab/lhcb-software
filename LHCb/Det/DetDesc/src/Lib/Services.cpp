// $Id: Services.cpp,v 1.6 2007-10-01 11:42:33 marcocle Exp $ 

// Include files
#include "DetDesc/Services.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h" 
#include "GaudiKernel/IUpdateManagerSvc.h"

/**
 * Default constructor
 */
DetDesc::Services::Services() : m_svcLocator(nullptr),
                                m_detSvc(nullptr),
                                m_msgSvc(nullptr),
                                m_updMgrSvc(nullptr),
                                m_refCount(0) {
}

/// a static instance of the Services class
DetDesc::Services* DetDesc::Services::s_services = nullptr ;

/// reset the static pointer 
void DetDesc::Services::setServices ( DetDesc::Services* val )
{
  s_services = val ;
}


/**
 * the accessor to Detector data provider 
 * @exception GaudiException the service could not be located 
 * @return pointer to detector data provider
 */
IDataProviderSvc* DetDesc::Services::detSvc() { 
  /// locate the service if necessary
  if (!m_detSvc){
    m_detSvc = svcLocator()->service ("DetectorDataSvc");
    if (!m_detSvc) {
      throw GaudiException
        ("DetDesc::Could not locate IDataProviderSvc='DetectorDataSvc'",
         "*DetDescException*" , StatusCode::FAILURE);
    }
    return m_detSvc;
  }
  // If already there, return the cached service
  return m_detSvc ;
}

/**
 * the accessor to Service Locator
 * @exception GaudiException the service could not be located 
 * @return pointer to message service
 */
ISvcLocator* DetDesc::Services::svcLocator() {
  /// Get the service if necessary
  if (!m_svcLocator){
    m_svcLocator = Gaudi::svcLocator();
    if (!m_svcLocator) {
      throw GaudiException("DetDesc::ISvcLocator* points to NULL!",
                           "*DetDescException*" , 
                           StatusCode::FAILURE);
    }
  }
  return m_svcLocator.get();
}

/**
 * the accessor to Message Service 
 * @exception GaudiException the service could not be located 
 * @return pointer to message service
 */
IMessageSvc* DetDesc::Services::msgSvc() {
  // locate the service if necessary
  if (!m_msgSvc) {
    m_msgSvc = svcLocator()->service("MessageSvc");
    if (!m_msgSvc) { 
      throw GaudiException
        ("DetDesc::Could not locate IMessageSvc='MessageSvc'",
         "*DetDescException*" , StatusCode::FAILURE);
    }
  }
  return m_msgSvc.get();
}

/**
 * the accessor to Update Manager Service
 * @exception GaudiException the service could not be located 
 * @return pointer to UpdateManagerSvc instance
 */
IUpdateManagerSvc* DetDesc::Services::updMgrSvc(bool create) {
  // locate the service if necessary
  if (!m_updMgrSvc) {
    m_updMgrSvc = svcLocator()->service("UpdateManagerSvc", create);
    if (!m_updMgrSvc)
      throw GaudiException
        ("DetDesc::Could not locate IUpdateManagerSvc='UpdateManagerSvc'",
         "*DetDescException*" , StatusCode::FAILURE);
  }
  return m_updMgrSvc.get();
}

/**
 * Gets an instance of Services
 */
DetDesc::Services* DetDesc::Services::services() {
  if (!s_services) s_services = new DetDesc::Services();
  s_services->addRef();
  return s_services;
}

/// Release this instance.
unsigned long DetDesc::Services::release() {
  --m_refCount;
  if (m_refCount <= 0) {
    delete this;
    setServices( nullptr ) ;
    return 0;
  }
  return m_refCount;
}

/// Increment the reference count of this instance.
unsigned long DetDesc::Services::addRef() 
{
  m_refCount++;
  return m_refCount;
}
