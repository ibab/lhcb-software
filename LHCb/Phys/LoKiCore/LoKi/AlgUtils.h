// ============================================================================
#ifndef LOKI_ALGUTILS_H 
#define LOKI_ALGUTILS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// forward declaration 
// ============================================================================
class     IAlgorithm ;
class GaudiAlgorithm ;
// ============================================================================
/** @file  
 *
 *  Access to algorithms 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-06
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  class AuxFunBase ;
  // ==========================================================================
  namespace AlgUtils 
  {
    // ========================================================================
    /// get the algorithm from context service 
    GAUDI_API 
    IAlgorithm*     getAlg      ( const LoKi::AuxFunBase& base         , 
                                  const bool              force = true ) ;
    // ========================================================================
    /// get gaudi algorithm from the context 
    GAUDI_API 
    GaudiAlgorithm* getGaudiAlg ( const LoKi::AuxFunBase& base  , 
                                  const bool              force = true ) ;
    // ========================================================================
  } //                                          end of namespace LoKi::AlgUtils 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGUTILS_H
// ============================================================================
