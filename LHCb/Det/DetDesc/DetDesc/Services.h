// $Id: Services.h,v 1.2 2002-12-13 14:18:18 ibelyaev Exp $
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

    /// Increment the reference count of this instance.
    virtual unsigned long addRef() ;
    
    /// Release this instance.
    virtual unsigned long release() ;
    

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
    
    /// reset the static pointer 
    static void setServices ( DetDesc::Services* val );

    // static acessor 
    static Services* services();

  protected:
    
    /** Default constructor */
    Services();
    
    /** Default destructor */
    virtual ~Services();

  private:

    /// copy constructor 
    Services( const Services & );    
    
  private:
    /// The service locator
    ISvcLocator* m_svcLocator;
    /// The Detector data provider
    IDataProviderSvc* m_detSvc;
    /// The Message service
    IMessageSvc* m_msgSvc;
    /// Reference count
    int m_refCount;
    
    
    /// a static instance of the Services class
    static DetDesc::Services* s_services ;
    
   };
  
}

#endif ///< DETDESC_DETDESC_H
