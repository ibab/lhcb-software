// $Id: Services.h,v 1.1 2002-11-21 15:42:53 sponce Exp $
#ifndef DETDESC_SERVICES_H 
#define DETDESC_SERVICES_H 1

#include <iostream>
// Forward declarations
class ISvcLocator;
class IDataProviderSvc;
class IMessageSvc;

/**
 * @namespace DetDesc DetDesc.h "DetDesc/DetDesc.h"
 *  
 * single access point to several usefull methods and accessor.
 * The main goal here is to cache references to very usefull services
 * to avoid a call to Service::service from each small component or
 * object.
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @author Sebastien Ponce
 */
namespace DetDesc {

  /**
   * This class is a container for accessors to different usefull services,
   * namely the service locator, message service and the detector data provider.
   * @author Sebastien Ponce
   */
  class Services {
  public:
    /** Default constructor */
    Services();

    /** Default destructor */
    virtual ~Services();
    
  public:
    /// Increment the reference count of this instance.
    virtual unsigned long addRef() {
      m_refCount++;
      return m_refCount;
    };

    /// Release this instance.
    virtual unsigned long release() {
      m_refCount--;
      if (m_refCount <= 0) {
        delete this;
        return 0;
      }
      return m_refCount;
    };

  public:
    /**
     * the accessor to Service Locator
     * @exception GaudiException the service could not be located 
     * @return pointer to message service
     */
    ISvcLocator* svcLocator();
    /**
     * the accessor to Detector data provider 
     * @exception GaudiException the service could not be located 
     * @return pointer to detector data provider
     */
    IDataProviderSvc* detSvc();    
    /**
     * the accessor to Message Service 
     * @exception GaudiException the service could not be located 
     * @return pointer to message service
     */
    IMessageSvc* msgSvc();

  private:
    /// The service locator
    ISvcLocator* m_svcLocator;
    /// The Detector data provider
    IDataProviderSvc* m_detSvc;
    /// The Message service
    IMessageSvc* m_msgSvc;
    /// Reference count
    int m_refCount;
  };
  
}

#endif ///< DETDESC_DETDESC_H
