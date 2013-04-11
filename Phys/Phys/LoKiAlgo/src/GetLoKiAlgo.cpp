// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// GaudiAlgs 
// ============================================================================
#include "GaudiAlg/GetAlg.h"
#include "GaudiAlg/GetAlgs.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Algo.h"
#include "LoKi/GetLoKiAlgo.h"
#include "LoKi/Services.h"
// ============================================================================
/** @file
 * Implementation file for function Gaudi::Utils::getLoKiAlgo
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-12-04 
 */
// ============================================================================
/*  get the last LoKi algorithm form the context service
 *  @see LoKi::Algo
 *  @see IAlgContextSvc
 *  @param svc pointer to algorithm context service
 *  @return the last LoKi-algortihm
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-12-04 
 */ 
// ============================================================================
LoKi::Algo* Gaudi::Utils::getLoKiAlgo ( const IAlgContextSvc* svc ) 
{
  if ( 0 == svc ) { return 0 ; }                           // RETURN 
  IAlgorithm* alg = Gaudi::Utils::getAlgorithm 
    ( svc , Gaudi::Utils::AlgTypeSelector<LoKi::Algo> () ) ;
  if ( 0 == alg ) { return 0 ; }                           // RETURN 
  return dynamic_cast<LoKi::Algo*> ( alg ) ;
}
// ======================================================================== 
/*  get the last LoKi algorithm from the context service
 *  @see LoKi::Algo
 *  @see LoKi::ILoKiSvc
 *  @param svc pointer to LoKi service 
 *  @return the last LoKi-algortihm
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-12-04 
 */ 
// ============================================================================
LoKi::Algo* Gaudi::Utils::getLoKiAlgo ( LoKi::ILoKiSvc* svc ) 
{
  if ( 0 == svc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    return Gaudi::Utils::getLoKiAlgo ( svcs.contextSvc() ) ;   // RETURN 
  }
  return Gaudi::Utils::getLoKiAlgo ( SmartIF<IAlgContextSvc>( svc ) ) ; // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
