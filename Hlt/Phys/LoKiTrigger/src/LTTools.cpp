// $Id$
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
#include "LoKi/AlgUtils.h"
#include "HltBase/IHltUnit.h"
// ============================================================================
// local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file 
 *  Implementation file for namespace LoKi::Hlt1::Utils 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-13
 */
// ==========================================================================
// get the selection by key 
// ==========================================================================
const Hlt::Selection* LoKi::Hlt1::Utils::getSelection
( const Gaudi::StringKey& key  ,
  const LoKi::AuxFunBase& base )
{
  SmartIF<Hlt::IUnit> unit ( getAlg ( base ) ) ;
  base.Assert( !(!unit) , "Hlt::IUnit* is invalid") ;
  return unit->selection ( key ) ;
}
// ==========================================================================
// compare vald pointers 
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
  if      ( trk1 -> lhcbIDs   () < trk2 -> lhcbIDs   () ) { return true  ; }
  else if ( trk2 -> lhcbIDs   () < trk1 -> lhcbIDs   () ) { return false ; }
  // compare by extra info 
  if      ( trk1 -> extraInfo () < trk2 -> extraInfo () ) { return true  ; }
  else if ( trk2 -> extraInfo () < trk1 -> extraInfo () ) { return false ; }
  // Finally: raw pointer comparison 
  return trk1 < trk2 ;
}
// ==========================================================================
// The END 
// ==========================================================================
