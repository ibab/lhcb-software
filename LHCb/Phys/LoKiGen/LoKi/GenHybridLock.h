// $Id: GenHybridLock.h,v 1.1 2007-07-25 15:07:57 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENHYBRIDLOCK_H 
#define LOKI_GENHYBRIDLOCK_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/IGenHybridTool.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-10-31 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid
  {
    // ========================================================================
    /** @class GenLock  LoKi/GenHybridLock.h 
     *  Helper class (sentry) to connent IGenHybridTool to GenEngine  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    class GenLock 
    {
    public:
      // ======================================================================
      /// constructor : Lock 
      GenLock  ( IGenHybridTool* tool ) ; // contructor : Lock 
      /// destructor : UnLock
      virtual ~GenLock () ;               // destrcutor : UnLock
      // ======================================================================
    private:
      // ======================================================================
      /// no default constructor 
      GenLock () ;                     // no default constructor 
      /// no copy constructor 
      GenLock ( const GenLock& ) ;     // no copy constructor 
      /// no assignement opeartor 
      GenLock& operator = ( const GenLock& ) ; // no assignement operator
      // ======================================================================
    private:
      // ======================================================================
      /// the tool itself 
      LoKi::Interface<LoKi::IGenHybridTool> m_tool ;  // the tool itself 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCHYBRIDLOCK_H
// ============================================================================
