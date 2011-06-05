// $Id$
// ============================================================================
#ifndef LOKI_MCMATCH_H 
#define LOKI_MCMATCH_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Interface.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-11 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  class MCMatchObj ;
  // ==========================================================================
  /** @class MCMatch.h LoKi/MCMatch.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  class GAUDI_API MCMatch : public LoKi::Interface<LoKi::MCMatchObj>
  { 
  public: 
    // ========================================================================
    /// Standard constructor from the object and reporter 
    MCMatch ( const LoKi::MCMatchObj*                  object ) ; 
    /// destructor 
    virtual ~MCMatch( ); // Destructor
    /// implicit conversion to the pointer 
    operator const LoKi::MCMatchObj* () const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    MCMatch();
    // ========================================================================
  } ; 
  // ==========================================================================
} //                                                  end of the namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_MCMATCH_H
// ============================================================================
