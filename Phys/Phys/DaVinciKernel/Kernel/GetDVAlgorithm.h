// $Id: GetDVAlgorithm.h,v 1.2 2010-05-14 15:07:10 ibelyaev Exp $
// ============================================================================
#ifndef KERNEL_GETDVALGORITHM_H 
#define KERNEL_GETDVALGORITHM_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/GetIDVAlgorithm.h"
// ============================================================================
// Forward decalrations 
// ============================================================================
class DVAlgorithm    ;
class IAlgContextSvc ;
// ============================================================================
namespace Gaudi 
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    /** get the pointer to the "current" DVAlgorithm
     *
     *  @code
     * 
     *   // get the context service:
     *   IAlgContextSvc* svc = ... ;
     * 
     *   // get DVAlgorithm from it:
     *   DVAgorithm* alg = Gaudi::Utils::getDVAlgorithm ( svc ) ;
     *
     *  @endcode
     *
     *  @see DVAlgorithm
     *  @see IAlgContextSvc 
     *  @see file GaudiAlg/GetAlg.h
     *  @see file GaudiAlg/GetAlgs.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-04
     */
    DVAlgorithm* getDVAlgorithm ( const IAlgContextSvc* svc ) ;
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_GETDVALGORITHM_H
// ============================================================================
