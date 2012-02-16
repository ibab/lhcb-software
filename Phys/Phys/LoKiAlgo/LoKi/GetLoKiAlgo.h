// $Id$
// ============================================================================
#ifndef LOKI_GETLOKIALGO_H 
#define LOKI_GETLOKIALGO_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// forward declarations 
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
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @date 2008-06-11 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ==========================================================================--
class IAlgContextSvc ;
namespace LoKi 
{ 
  // ==========================================================================
  class Algo     ; 
  class ILoKiSvc ;
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    /** get the last LoKi algorithm form the context service
     *  @see LoKi::Algo
     *  @see IAlgContextSvc
     *  @param svc pointer to algorithm context service
     *  @return the last LoKi-algortihm
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-04 
     */ 
    GAUDI_API 
    LoKi::Algo* getLoKiAlgo ( const IAlgContextSvc* svc     ) ;
    // ========================================================================
    /** get the last LoKi algorithm from the context service
     *  @see LoKi::Algo
     *  @see LoKi::ILoKiSvc
     *  @param svc pointer to LoKi service 
     *  @return the last LoKi-algortihm
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-04 
     */ 
    GAUDI_API 
    LoKi::Algo* getLoKiAlgo ( LoKi::ILoKiSvc* svc = 0 ) ;
    // ======================================================================== 
  } //                                            end of namespace Gaudi::Utils 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** get the last LoKi algorithm form the context service
   *  @see LoKi::Algo
   *  @see IAlgContextSvc
   *  @param svc pointer to algorithm context service
   *  @return the last LoKi-algortihm
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-04 
   */ 
  inline LoKi::Algo* getAlgo ( const IAlgContextSvc* svc     ) 
  { return Gaudi::Utils::getLoKiAlgo ( svc ) ; }
  // ==========================================================================
  /** get the last LoKi algorithm from the context service
   *  @see LoKi::Algo
   *  @see LoKi::ILoKiSvc
   *  @param svc pointer to LoKi service 
   *  @return the last LoKi-algortihm
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-04 
   */ 
  inline LoKi::Algo* getAlgo ( LoKi::ILoKiSvc* svc = 0 ) 
  { return Gaudi::Utils::getLoKiAlgo ( svc ) ; }
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETLOKIALGO_H
// ============================================================================
