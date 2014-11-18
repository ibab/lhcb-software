// $Id: GiGaCnvUtils.h,v 1.1 2002-01-22 18:24:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGACNV_GIGACNVUTILS_H 
#define GIGACNV_GIGACNVUTILS_H 1
// Include files
// GauidiKernel
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"

/** @namespace GiGaCnvUtils GiGaCnvUtils.h GiGaCnv/GiGaCnvUtils.h
 *  
 *  Collection of  useful function to make the transition 
 *  from Gaudi v9 to Gaudi v10 much easier 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   17/01/2002
 */

namespace  GiGaCnvUtils
{

  enum
  {
    CNVSVC_IS_NULL = 300 ,
    OBJECT_IS_NULL       ,
    ADDRESS_IS_NULL      ,
    REGISTRY_IS_NULL     ,
    SELECTOR_IS_NULL
  };
  
  /** @function createRep 
   * 
   *  Useful combination of createObj and fillRepRefs calls
   *  and update registry methods 
   *  
   *  @param cncsvc pointer to conversion service 
   *  @param object object to be converted to another representation
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   17/01/2002
   */
  inline StatusCode createRep 
  ( IConversionSvc* cnvsvc , 
    DataObject*     object )
  {
    if( 0 == cnvsvc        ) { return StatusCode ( CNVSVC_IS_NULL   ) ; }
    if( 0 == object        ) { return StatusCode ( OBJECT_IS_NULL   ) ; }
    IOpaqueAddress* address = 0 ;
    /// create the representation 
    StatusCode status = cnvsvc->createRep( object , address ) ;
    ///
    if( status.isFailure() ) { return status ; }
    if( 0 == address       ) { return StatusCode ( ADDRESS_IS_NULL  ) ; }
    /// update the registry 
    IRegistry* registry = object->registry() ;
    if( 0 == registry      ) { return StatusCode ( REGISTRY_IS_NULL ) ; }
    registry->setAddress( address ) ;
    /// fill references 
    return cnvsvc->fillRepRefs( registry->address() , object );
  }
  
  /** @function createReps 
   * 
   *  Useful combination of createObj and fillRepRefs calls
   *  and update registry methods 
   *  
   *  @param cncsvc   pointer to conversion service 
   *  @param selector collection of objects to be converted
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   17/01/2002
   */
  inline StatusCode createReps 
  ( IConversionSvc* cnvsvc   , 
    IDataSelector*  selector )
  {
    if( 0 == cnvsvc        ) { return StatusCode ( CNVSVC_IS_NULL   ) ; }
    if( 0 == selector      ) { return StatusCode ( SELECTOR_IS_NULL ) ; }
    /// the first loop over selector
    IDataSelector::iterator object;
    for( object = selector->begin() ; selector->end() != object ; ++object ) 
      {
        if( 0 == *object   ) { return StatusCode ( OBJECT_IS_NULL   ) ; }
        IOpaqueAddress* address = 0 ;
        /// create the representation 
        StatusCode st = cnvsvc->createRep( *object , address ) ;
        ///
        if( st.isFailure() ) { return st ; }
        if( 0 == address   ) { return StatusCode ( ADDRESS_IS_NULL  ) ; }
        /// update the registry 
        IRegistry* registry = (*object)->registry() ;
        if( 0 == registry  ) { return StatusCode ( REGISTRY_IS_NULL ) ; }
        registry->setAddress( address ) ;
      }
    /// the second loop over selector 
    for( object = selector->begin() ; selector->end() != object ; ++object ) 
      {
        IRegistry* registry = (*object)->registry();
        StatusCode st = cnvsvc->fillRepRefs( registry->address() , *object );
        if( st.isFailure() ) { return st ; }
      }
    /// 
    return StatusCode::SUCCESS ;
  }

  /** @function parent
   *  
   *  useful function to get the access to parent registry
   * 
   *  @param  registry pointer  to object registry  
   *  @param  manager  data manager service 
   *  @return selector registry of parent object 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   17/01/2002
   */
  inline  IRegistry* parent
  ( const IRegistry* registry , 
    IDataManagerSvc* manager  ) 
  {
    if( 0 == registry || 0 == manager ) { return 0 ; }
    IRegistry* reg = 0 ;
    StatusCode sc = manager->objectParent( registry , reg );
    return sc.isSuccess() ? reg : (IRegistry*) 0 ;
  }
  
  /** @function parent
   *  
   *  useful function to get the access to parent registry
   * 
   *  @param  registry pointer  to object registry  
   *  @param  manager  data provider service 
   *  @return selector registry of parent object 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   17/01/2002
   */
  inline  IRegistry*  parent
  ( const IRegistry*  registry , 
    IDataProviderSvc* provider ) 
  {
    return 
      ( 0 == registry ) ? (IRegistry*) 0 :
      ( 0 == provider ) ? (IRegistry*) 0 :
      parent( registry , SmartIF<IDataManagerSvc>( provider ) );
  }
  
  /** @function parent
   *  
   *  useful function to get the access to parent registry
   * 
   *  @param  registry pointer  to object registry  
   *  @return selector registry of parent object 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   17/01/2002
   */
  inline IRegistry* parent( const IRegistry* registry ) 
  {
    return 
      ( 0 == registry ) ? (IRegistry*) 0 : 
      parent( registry  , registry->dataSvc() ) ;  
  }
  
} /// end of namespace GiGaCnvUtils

// ============================================================================
// End 
// ============================================================================
#endif // GIGACNV_GIGACNVUTILS_H
// ============================================================================
