// $Id: GetPhysDesktop.cpp,v 1.2 2008-12-05 09:09:21 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/GetPhysDesktop.h"
#include "LoKi/Services.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file
 *  The implementation file for functions LoKi::getPhysDesktop
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  date 2008-01-16
 */
// ============================================================================
/*  get the desktop from DVAlgorithm class
 *  @param alg pointer to DVAlgorithm 
 *  @return the pointer to desktop
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  date 2008-01-16
 */
// ============================================================================
IPhysDesktop* LoKi::getPhysDesktop ( const DVAlgorithm* alg ) 
{
  if ( 0 != alg ) { return alg -> desktop() ; }
  LoKi::Report::Error("LoKi::getPhysDesktop: DVAlgorithm* points to NULL!") ;
  return 0 ;
}
// ============================================================================
/*  get the desktop from Algorithm Context Service 
 *  @param  pointer to Algorithm Context Service  
 *  @return the pointer to desktop
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  date 2008-01-16
 */
// ============================================================================
IPhysDesktop* LoKi::getPhysDesktop ( IAlgContextSvc* svc ) 
{
  if ( 0 == svc ) 
  {
    LoKi::Report::Error("LoKi::getPhysDesktop: IAlgContextSvc* points to NULL!") ;
    return 0 ;
  }
  return LoKi::getPhysDesktop ( Gaudi::Utils::getDVAlgorithm ( svc ) ) ;
}
// ============================================================================
/** get the desktop from LoKi Service 
 *  @param  pointer to LoKi Service   
 *  @return the pointer to desktop
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  date 2008-01-16
 */
// ============================================================================
IPhysDesktop* LoKi::getPhysDesktop ( LoKi::ILoKiSvc* svc  ) 
{
  if ( 0 == svc ) 
  {
    LoKi::Report::Error("LoKi::getPhysDesktop: LoKi::ILoKiSvc* points to NULL!") ;
    return 0 ;
  }
  return LoKi::getPhysDesktop ( SmartIF<IAlgContextSvc> ( svc ) ) ;
}
// ==========================================================================
/** get the desktop using the chain LoKi -> Algorithm Context -> DVAlgorithm
 *  @return the pointer to desktop
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  date 2008-01-16
 */
// ==========================================================================
IPhysDesktop* LoKi::getPhysDesktop () 
{
  const LoKi::Services& instance = LoKi::Services::instance() ;
  LoKi::ILoKiSvc* svc = instance.lokiSvc() ;
  if ( 0 == svc ) 
  {
    LoKi::Report::Error("LoKi::getPhysDesktop: LoKi::Services::loKiSvc() points to NULL!") ;
    return 0 ;
  }
  return LoKi::getPhysDesktop ( svc ) ;
}
// ============================================================================
// The END 
// ============================================================================

