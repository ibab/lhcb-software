// $Id: Services.cpp,v 1.1 2002-11-21 15:42:54 sponce Exp $ 

// Include files
#include "DetDesc/Services.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h" 

/**
 * Default constructor
 */
DetDesc::Services::Services() : m_svcLocator(0),
                                m_detSvc(0),
                                m_msgSvc(0),
                                m_refCount(0) {
};

/** Default destructor */
DetDesc::Services::~Services() {
  if (0 != m_svcLocator) m_svcLocator->release();
  if (0 != m_detSvc) m_detSvc->release();
  if (0 != m_msgSvc) m_msgSvc->release();
};

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
};

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
};

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
};
