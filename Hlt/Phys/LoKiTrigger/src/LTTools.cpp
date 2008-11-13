// $Id: LTTools.cpp,v 1.1 2008-11-13 13:14:42 ibelyaev Exp $
// ========================================================================
// Include files
// ========================================================================
// GaudiKernel
// ========================================================================
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GetAlgs.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AuxFunBase.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/IHltUnit.h"
// ============================================================================
// local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file 
 *  Implementation fiel for namespace LoKi::Hlt1::Utils 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-13
 */
// ==========================================================================
// get gaudi algorithm from the context 
// ==========================================================================
GaudiAlgorithm* LoKi::Hlt1::Utils::getGaudiAlg ( const LoKi::AuxFunBase& base ) 
{
  // get LoKi service
  const LoKi::Interface<LoKi::ILoKiSvc>& svc = base.lokiSvc() ;
  base.Assert( !(!svc) , "LoKi Service is not available!" ) ;
  const IAlgContextSvc* cntx = svc->contextSvc() ;
  base.Assert( 0 != cntx , "IAlgContextSvc* points to NULL!") ;
  return Gaudi::Utils::getGaudiAlg ( cntx ) ;
}
// ==========================================================================
// get the algorithm from context service 
// ==========================================================================
IAlgorithm* LoKi::Hlt1::Utils::getAlg ( const LoKi::AuxFunBase& base ) 
{
  // get LoKi service
  const LoKi::Interface<LoKi::ILoKiSvc>& svc = base.lokiSvc() ;
  base.Assert( !(!svc) , "LoKi Service is not available!" ) ;
  const IAlgContextSvc* cntx = svc->contextSvc() ;
  base.Assert( 0 != cntx , "IAlgContextSvc* points to NULL!") ;
  IAlgorithm* ialg = cntx->currentAlg() ;
  base.Assert( 0 != ialg , "IAlgorithm* points to NULL!") ;
  return ialg ;
}
// ==========================================================================
// get the selection by key 
// ==========================================================================
Hlt::Selection* LoKi::Hlt1::Utils::getSelection
( const stringKey&        key  ,
  const LoKi::AuxFunBase& base )
{
  SmartIF<LoKi::IHltUnit> unit ( getUnit( base ) ) ;
  base.Assert( !(!unit) , "LoKi::IHltUnit* is invalid") ;
  return unit->selection ( key ) ;
}
// ==========================================================================
