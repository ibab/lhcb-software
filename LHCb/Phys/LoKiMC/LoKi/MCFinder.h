// $Id: MCFinder.h,v 1.6 2007-07-23 17:27:30 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCFINDER_H 
#define LOKI_MCFINDER_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/MCFinderObj.h"
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
  /// forward declaration 
  class MCFinderObj ; 
  // ==========================================================================
  /** @class MCFinder MCFinder.h LoKi/MCFinder.h
   *
   *  Helper class whcih simplify the manipulation with 
   *  the class LoKi::IMCFinderObj 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  class GAUDI_API MCFinder : public LoKi::Interface<LoKi::MCFinderObj>
  {
  public: 
    // ========================================================================
    /// Standard constructor
    MCFinder ( const LoKi::MCFinderObj* ); 
    /// virtual destructor
    virtual ~MCFinder( );    // Destructor
    /// implicit conversion to the pointer 
    operator const LoKi::MCFinderObj* () const ; 
    /// check for pointer validity:
    bool operator! () const { return validPointer() ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    MCFinder() ;                         // the default constructor is disabled 
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                  end of the namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCFINDER_H
// ============================================================================
