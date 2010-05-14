// $Id: GetPhysDesktop.h,v 1.4 2010-05-14 15:28:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GETPHYSDESKTOP_H 
#define LOKI_GETPHYSDESKTOP_H 1
// ============================================================================
// Include files
// ============================================================================
// forward declarations
// ============================================================================
class IDVAlgorithm    ;
class IAlgContextSvc ;
namespace LoKi { class ILoKiSvc ; }
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** get the desktop from Algorithm Context Service 
   *  @param  svc pointer to Algorithm Context Service  
   *  @return the pointer to desktop
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  date 2008-01-16
   */
  IDVAlgorithm* getPhysDesktop ( const IAlgContextSvc* svc ) ;
  // ==========================================================================
  /** get the desktop from LoKi Service 
   *  @param  pointer to LoKi Service   
   *  @return the pointer to desktop
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  date 2008-01-16
   */
  IDVAlgorithm* getPhysDesktop ( const LoKi::ILoKiSvc* svc  ) ;
  // ==========================================================================
  /** get the desktop using the chain LoKi -> Algorithm Context -> DVAlgorithm
   *  @return the pointer to desktop
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  date 2008-01-16
   */
  IDVAlgorithm* getPhysDesktop () ;
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETPHYSDESKTOP_H
// ============================================================================
