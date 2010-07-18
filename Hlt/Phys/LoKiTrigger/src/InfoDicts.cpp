// $Id: InfoDicts.cpp,v 1.2 2007-11-28 14:56:24 ibelyaev Exp $
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/InfoDicts.h"
#include "LoKi/Tracks.h"
#include "LoKi/RecVertices.h"
// ============================================================================
/** @file 
 *  Implementation file for functions LoKi::Dicts::Info::info
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-08-14 
 */
// ============================================================================
LoKi::BasicFunctors<const LHCb::Track*>::FunctionFromFunction
LoKi::Dicts::TrInfo::info 
( const int                                                index  , 
  const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun    ,
  const bool                                              update ) 
{ 
  return LoKi::Tracks::SmartInfo ( index , fun , update ) ; 
}  
// ======================================================================
LoKi::BasicFunctors<LHCb::RecVertex>::FunctionFromFunction
LoKi::Dicts::TrInfo::info 
( const int                                             index  , 
  const LoKi::BasicFunctors<LHCb::RecVertex>::Function& fun    ,
  const bool                                            update ) 
{ 
  return LoKi::RecVertices::SmartInfo( index , fun , update ) ; 
}
// ============================================================================
// The END 
// ============================================================================
