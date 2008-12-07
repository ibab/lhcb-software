// $Id: LTTools.cpp,v 1.3 2008-12-07 14:34:29 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
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
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  base.Assert( !(!cntx) , "IAlgContextSvc* points to NULL!") ;
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
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  base.Assert( !(!cntx) , "IAlgContextSvc* points to NULL!") ;
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
bool LoKi::Hlt1::Utils::CmpTrack::compare
( const LHCb::Track* trk1 , 
  const LHCb::Track* trk2 ) const 
{
  // Compare by flags:
  const unsigned int f1 = trk1 -> flags() ;
  const unsigned int f2 = trk2 -> flags() ;
  if      ( f1  < f2   ) { return true  ; }
  else if ( f2  < f1   ) { return false ; }
  // compare by pt 
  const double pt1 = trk1 -> pt() ;
  const double pt2 = trk2 -> pt() ;
  if      ( pt1 < pt2  ) { return true  ; }
  else if ( pt2 < pt1  ) { return false ; }
  // compare by key      
  const bool h1 = trk1 -> hasKey() ;
  const bool h2 = trk2 -> hasKey() ;
  if      ( !h1 &&  h2 ) { return true  ; }
  else if (  h1 && !h2 ) { return false ; }
  else if (  h1 &&  h2 ) 
  {
    const int key1 = trk1 -> key() ;
    const int key2 = trk2 -> key() ;
    if       ( key1 < key2 ) { return true  ; }
    else if  ( key2 < key1 ) { return false ; }
  }
  // compare by LHCbIDs 
  //if      ( trk1 -> lhcbIDs   () < trk2 -> lhcbIDs   () ) { return true  ; }
  //else if ( trk2 -> lhcbIDs   () < trk1 -> lhcbIDs   () ) { return false ; }
  // compare by extra info 
  if      ( trk1 -> extraInfo () < trk2 -> extraInfo () ) { return true  ; }
  else if ( trk2 -> extraInfo () < trk1 -> extraInfo () ) { return false ; }
  // Finally: raw pointer comparison 
  return trk1 < trk2 ;
}
// ==========================================================================
// The END 
// ==========================================================================
