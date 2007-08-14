// $Id: InfoDicts.cpp,v 1.1 2007-08-14 20:32:32 ibelyaev Exp $
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
LoKi::FunctionFromFunction<LHCb::Track>
LoKi::Dicts::Info::info 
( const int                              index  , 
  const LoKi::Function<LHCb::Track>&     fun    ,
  const bool                             update ) 
{ 
  return LoKi::Tracks::SmartInfo( index , fun , update ) ; 
}  
// ======================================================================
LoKi::FunctionFromFunction<LHCb::RecVertex>
LoKi::Dicts::Info::info 
( const int                              index  , 
  const LoKi::Function<LHCb::RecVertex>& fun    ,
  const bool                             update ) 
{ 
  return LoKi::RecVertices::SmartInfo( index , fun , update ) ; 
}
// ============================================================================
// The END 
// ============================================================================
