// $Id: DetDesc.cpp,v 1.2 2002-05-11 18:25:47 ibelyaev Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// ===========================================================================
// STD & STL 
#include <string> 
#include <stdio.h>
// GaudiKernel includes 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h" 
#include "GaudiKernel/GaudiException.h" 
// DetDesc 
#include "DetDesc/DetDesc.h"

// ============================================================================
/** @file 
 *
 * Implementationof methods from namespace DetDesc 
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 10/08/2001 
 */
// ============================================================================

// ============================================================================
/** print double value using format
 *  @param value   value to be printed 
 *  @param format  desirable format
 *  @returnstring representation of the "value"
 */
// ============================================================================
const std::string DetDesc::print
( const double value  , 
  const char*  format )
{
  static const unsigned int buflen = 128;
  static char  buffer[buflen];
  return std::string( buffer , buffer + sprintf( buffer , format , value ) );
};
// ============================================================================

// ============================================================================
/** print double value using format
 *  @param value   value to be printed 
 *  @param format  desirable format
 *  @returnstring representation of the "value"
 */
// ============================================================================
const std::string DetDesc::print
( const long   value  , 
  const char*  format )
{
  static const unsigned int buflen = 128;
  static char  buffer[buflen];
  return std::string( buffer , buffer + sprintf( buffer , format , value ) );
};
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


// ============================================================================
/** the accessor to Mesage Service 
 *  @exception GaudiException the service could not be located 
 *  @return pointer to message service
 */
// ============================================================================
IMessageSvc*       DetDesc::msgSvc()
{
  static IMessageSvc* s_msgSvc = 0 ;
  //  locate the service 
  if( 0 == s_msgSvc ) 
    {
      ISvcLocator* svcLoc = Gaudi::svcLocator();
      if( 0 == svcLoc ) 
        { throw GaudiException("DetDesc::ISvcLocator* points to NULL!",
                               "*DetDescException*" , 
                               StatusCode::FAILURE  ); }
      StatusCode sc = 
        svcLoc->service( DetDesc::s_MessageSvcName , s_msgSvc );
      if( sc.isFailure() ) 
        { throw GaudiException("DetDesc::Could not locate IMessageSvc='" 
                               + DetDesc::s_MessageSvcName + "'",
                               "*DetDescException*" , 
                               StatusCode::FAILURE); }
      if( 0 == s_msgSvc ) 
        { throw GaudiException("DetDesc::IMessageSvc*(" 
                               + DetDesc::s_MessageSvcName + 
                               "') points to NULL!" ,
                               "*DetDescException*" , 
                               StatusCode::FAILURE  ); }
      s_msgSvc->addRef();
    }
  ///
  return s_msgSvc ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================


