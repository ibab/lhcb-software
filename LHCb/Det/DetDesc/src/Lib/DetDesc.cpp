/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
/// STD & STL 
#include <string> 
///@{ 
/** GaudiKernel includes */
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h" 
#include "GaudiKernel/GaudiException.h" 
///@} 
/// DetDesc 
#include "DetDesc/DetDesc.h"

// ============================================================================
/** @file DetDesc.cpp
 *
 * Implementationof methods fromnamespace DetDesc 
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 10/08/2001 
 */
// ============================================================================

// ============================================================================
/** the accessor to Detector data provider 
 *  @exception GaudiException service could not be located 
 *  @return pointer to detector data provider
 */
// ============================================================================
IDataProviderSvc* DetDesc::detSvc()
{ 
  /// initialize the static pointer  
  static IDataProviderSvc* s_detSvc = 0 ;
  /// locate te service 
  if( 0 == s_detSvc ) 
    {
      ISvcLocator* svcLoc = Gaudi::svcLocator();
      if( 0 == svcLoc ) 
        { throw GaudiException("DetDesc::ISvcLocator* points to NULL!",
                               "*DetDescException*" , 
                               StatusCode::FAILURE  ); }
      StatusCode sc = 
        svcLoc->service( DetDesc::s_DetDataSvcName , s_detSvc );
      if( sc.isFailure() ) 
        { throw GaudiException("DetDesc::Could not locate IDataProviderSvc='" 
                               + DetDesc::s_DetDataSvcName + "'",
                               "*DetDescException*" , 
                               StatusCode::FAILURE); }
      if( 0 == s_detSvc ) 
        { throw GaudiException("DetDesc::IDataProviderSvc*(" 
                               + DetDesc::s_DetDataSvcName + 
                               "') points to NULL!" ,
                               "*DetDescException*" , 
                               StatusCode::FAILURE  ); }
      s_detSvc->addRef();
    }
  ///
  return s_detSvc ;
};

// ============================================================================

