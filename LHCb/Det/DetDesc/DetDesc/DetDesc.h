// $Id: DetDesc.h,v 1.2 2002-05-11 18:25:45 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// ===========================================================================
#ifndef DETDESC_DETDESC_H 
#define DETDESC_DETDESC_H 1
/// STD & STL 
#include <string>
/// GaudiKernel
class IDataProviderSvc;
class IMessageSvc;

/** @namespace DetDesc DetDesc.h "DetDesc/DetDesc.h"
 *  
 *  several general technical methods 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   10/08/2001
 */

namespace DetDesc
{

  /** print double value using format
   *  @param value   value to be printed 
   *  @param format  desirable format
   *  @returnstring representation of the "value"
   */
  const std::string print
  ( const double value            , 
    const char*  format = "%9.3g" );
  
  /** print  long value using format
   *  @param value   value to be printed 
   *  @param format  desirable format
   *  @returnstring representation of the "value"
   */
  const std::string print
  ( const long   value            , 
    const char*  format = "%d"    );
  
  /// name of Detector Data service 
  static const std::string s_DetDataSvcName("DetectorDataSvc");
  /// name of Message  Service 
  static const std::string s_MessageSvcName("MessageSvc");
  
  /** the accessor to Detector data provider 
   *  @exception GaudiException the service could not be located 
   *  @return pointer to detector data provider
   */
  IDataProviderSvc*  detSvc ();

  /** the accessor to Mesage Service 
   *  @exception GaudiException the service could not be located 
   *  @return pointer to message service
   */
  IMessageSvc*       msgSvc ();

}; /// end of namespace DetDesc 
 

/// ===========================================================================
#endif ///< DETDESC_DETDESC_H
/// ===========================================================================
