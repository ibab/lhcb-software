// $Id: GetPhysDesktop.cpp,v 1.3 2010-05-14 15:28:33 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
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
/*  get the desktop from Algorithm Context Service 
 *  @param  pointer to Algorithm Context Service  
 *  @return the pointer to desktop
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  date 2008-01-16
 */
// ============================================================================
IDVAlgorithm* LoKi::getPhysDesktop ( const IAlgContextSvc* svc ) 
{
  if ( 0 == svc ) 
  {
    LoKi::Report::Error("LoKi::getPhysDesktop: IAlgContextSvc* points to NULL!") ;
    return 0 ;
  }
  return Gaudi::Utils::getIDVAlgorithm ( svc ) ;
}
// ============================================================================
/** get the desktop from LoKi Service 
 *  @param  pointer to LoKi Service   
 *  @return the pointer to desktop
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  date 2008-01-16
 */
// ============================================================================
IDVAlgorithm* LoKi::getPhysDesktop ( const LoKi::ILoKiSvc* svc  ) 
{
  if ( 0 == svc ) 
  {
    LoKi::Report::Error("LoKi::getPhysDesktop: LoKi::ILoKiSvc* points to NULL!") ;
    return 0 ;
  }
  LoKi::ILoKiSvc* _svc = const_cast<LoKi::ILoKiSvc*> ( svc ) ;
  return LoKi::getPhysDesktop ( SmartIF<IAlgContextSvc> ( _svc ) ) ;
}
// ==========================================================================
/** get the desktop using the chain LoKi -> Algorithm Context -> IDVAlgorithm
 *  @return the pointer to desktop
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  date 2008-01-16
 */
// ==========================================================================
IDVAlgorithm* LoKi::getPhysDesktop () 
{
  const LoKi::Services& instance = LoKi::Services::instance() ;
  const LoKi::ILoKiSvc* svc = instance.lokiSvc() ;
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

