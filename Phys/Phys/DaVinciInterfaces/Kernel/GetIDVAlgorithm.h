// $Id: GetIDVAlgorithm.h,v 1.1 2010-05-14 15:03:45 ibelyaev Exp $
// ============================================================================
#ifndef KERNEL_GETIDVALGORITHM_H 
#define KERNEL_GETIDVALGORITHM_H 1
// ============================================================================
// Include files
// ============================================================================
// Forward declarations 
// ============================================================================
class IDVAlgorithm   ;
class IAlgContextSvc ;
// ============================================================================
namespace Gaudi 
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    /** get the pointer to the "current" IDVAlgorithm
     *
     *  @code
     * 
     *   // get the context service:
     *   IAlgContextSvc* svc = ... ;
     * 
     *   // get IDVAlgorithm from it:
     *   IDVAgorithm* alg = Gaudi::Utils::getIDVAlgorithm ( svc ) ;
     *
     *  @endcode
     *
     *  @see IDVAlgorithm
     *  @see IAlgContextSvc 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-04
     */
    IDVAlgorithm* getIDVAlgorithm ( const IAlgContextSvc* svc ) ;
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_GETIDVALGORITHM_H
// ============================================================================
