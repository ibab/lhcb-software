// $Id: GetIDVAlgorithm.cpp,v 1.1 2010-05-14 15:03:46 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GetAlg.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
// ============================================================================
/** @file 
 *  Implementation file for function Gaudi::Utils::GetIDVAlgorithm
 *  @see IDVAlgorithm
 *  @see IAlgContextSvc 
 *  @see GaudiAlg/GetAlg.h
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-14
 */
// ============================================================================
namespace 
{
  // =========================================================================
  /** @class IDVSelector 
   *  Helper class for selection of IDVAlgorithm 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-05-14
   */
  class IDVSelector : public Gaudi::Utils::AlgSelector 
  {
  public:
    // ========================================================================
    /// the only one essential method:
    virtual bool operator() ( const IAlgorithm* alg ) const 
    {
      if ( 0 == alg ) { return false ; } 
      IAlgorithm* _alg = const_cast<IAlgorithm*> ( alg );
      SmartIF<IDVAlgorithm> idv ( _alg );
      return idv.isValid() ;
    }
    virtual ~IDVSelector(){};
    // ========================================================================
  };
  // ==========================================================================
}
// ============================================================================
/* get the pointer to the "current" IDVAlgorithm
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
// ============================================================================
IDVAlgorithm* Gaudi::Utils::getIDVAlgorithm ( const IAlgContextSvc* svc ) 
{
  if ( 0 == svc ) { return 0 ; }                                // RETURN
  // use context service:
  IAlgorithm* alg = Gaudi::Utils::getAlgorithm ( svc , IDVSelector() ) ;
  if ( 0 == alg ) { return 0 ; }                                // RETURN 
  return SmartIF<IDVAlgorithm> ( alg ) ;
}
// ============================================================================
// The END 
// ============================================================================
