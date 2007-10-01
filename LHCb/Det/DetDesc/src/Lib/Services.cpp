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
DetDesc::Services::Services() : m_svcLocator(0),
                                m_detSvc(0),
                                m_msgSvc(0),
                                m_updMgrSvc(0),
                                m_refCount(0) {
}

/// a static instance of the Services class
DetDesc::Services* DetDesc::Services::s_services = 0 ;

/// reset the static pointer 
void DetDesc::Services::setServices ( DetDesc::Services* val )
{
  s_services = val ;
}


/** Default destructor */
DetDesc::Services::~Services() {
  if (0 != m_svcLocator) m_svcLocator->release();
  if (0 != m_detSvc) m_detSvc->release();
  if (0 != m_msgSvc) m_msgSvc->release();
  if (0 != m_updMgrSvc) m_updMgrSvc->release();
}

/**
 * the accessor to Detector data provider 
 * @exception GaudiException the service could not be located 
 * @return pointer to detector data provider
 */
IDataProviderSvc* DetDesc::Services::detSvc() { 
  /// locate the service if necessary
  if (0 == m_detSvc){
    StatusCode sc = svcLocator()->service ("DetectorDataSvc", m_detSvc);
    if (!sc.isSuccess()) {
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
  if (0 == m_svcLocator){
    m_svcLocator = Gaudi::svcLocator();
    if (0 == m_svcLocator) {
      throw GaudiException("DetDesc::ISvcLocator* points to NULL!",
                           "*DetDescException*" , 
                           StatusCode::FAILURE);
    }
    return m_svcLocator;
  }
  // If already there, return the cached service
  return m_svcLocator ;
}

/**
 * the accessor to Message Service 
 * @exception GaudiException the service could not be located 
 * @return pointer to message service
 */
IMessageSvc* DetDesc::Services::msgSvc() {
  // locate the service if necessary
  if (0 == m_msgSvc) {
    StatusCode sc = svcLocator()->service("MessageSvc" , m_msgSvc);
    if (!sc.isSuccess()) {
      throw GaudiException
        ("DetDesc::Could not locate IMessageSvc='MessageSvc'",
         "*DetDescException*" , StatusCode::FAILURE);
    }
    return m_msgSvc;
  }
  return m_msgSvc ;
}

/**
 * the accessor to Update Manager Service
 * @exception GaudiException the service could not be located 
 * @return pointer to UpdateManagerSvc instance
 */
IUpdateManagerSvc* DetDesc::Services::updMgrSvc(bool create) {
  // locate the service if necessary
  if (0 == m_updMgrSvc) {
    StatusCode sc = svcLocator()->service("UpdateManagerSvc", m_updMgrSvc, create);
    if (!sc.isSuccess()) {
      throw GaudiException
        ("DetDesc::Could not locate IUpdateManagerSvc='UpdateManagerSvc'",
         "*DetDescException*" , StatusCode::FAILURE);
    }
  }
  return m_updMgrSvc ;
}

/**
 * Gets an instance of Services
 */
DetDesc::Services* DetDesc::Services::services() {
  if (0 == s_services){ 
    s_services = new DetDesc::Services();
  }
  s_services->addRef();
  return s_services;
}

/// Release this instance.
unsigned long DetDesc::Services::release() {
  m_refCount--;
  if (m_refCount <= 0) {
    delete this;
    setServices ( 0 ) ;
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
