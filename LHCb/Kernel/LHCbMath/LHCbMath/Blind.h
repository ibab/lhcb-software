// $Id: $
// ============================================================================
#ifndef LHCBMATH_BLIND_H 
#define LHCBMATH_BLIND_H 
// ===========================================================================
// Include files
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/Kernel.h"
// ===========================================================================
namespace Gaudi
{
  // =========================================================================
  namespace Math
  {
    // =======================================================================
    /** get the 'blind'-number form the string 
     *  @param name the name to be used for construction of the coefficient
     *  @param minv the minimal value 
     *  @param minv the maximal value 
     *  @return the value that can be used for blinding 
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-10
     */
    GAUDI_API double blind 
    ( const std::string& name , 
      const double       minv , 
      const double       maxv ) ;
    // ========================================================================
  } //                                             end of namespace Gaudi::Math
  // ==========================================================================
} //                                                     end of namespace Gaudi 
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_BLIND_H
// ============================================================================
