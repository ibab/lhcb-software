/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef DETDESC_DETDESC_H 
#define DETDESC_DETDESC_H 1
/// STD & STL 
#include <string>
/// GaudiKernel
class IDataProviderSvc;

/** @namespace DetDesc DetDesc.h "DetDesc/DetDesc.h"
 *  
 *  several general technical methods 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   10/08/2001
 */

namespace DetDesc
{
  
  /** name of Detector Data service 
   */
  static const std::string s_DetDataSvcName("DetectorDataSvc");
  
  /** the accessor to Detector data provider 
   *  @exception GaudiException the service could not be located 
   *  @return pointer to detector data provider
   */
  IDataProviderSvc*  detSvc();

}; /// end of namespace DetDesc 
 

/// ===========================================================================
#endif ///< DETDESC_DETDESC_H
/// ===========================================================================
