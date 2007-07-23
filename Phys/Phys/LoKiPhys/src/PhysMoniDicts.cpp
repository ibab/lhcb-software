// $Id: PhysMoniDicts.cpp,v 1.1 2007-07-23 17:35:56 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Monitoring.h"
#include "LoKi/PhysMoniDicts.h"
// ============================================================================
/** @file 
 *  Implementation file for the functions fomr the file LoKi/PhysMoniDicts.h
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-07-21 
 */
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*>
LoKi::Dicts::monitor 
( const LoKi::Predicate<const LHCb::Particle*>&   cut  ,
  StatEntity*                                     stat )
{ return LoKi::monitor ( cut , stat ) ; }
// ============================================================================
LoKi::FunctionFromFunction<const LHCb::Particle*>
LoKi::Dicts::monitor 
( const LoKi::Function<const LHCb::Particle*>&    fun  ,
  StatEntity*                                     stat )
{ return LoKi::monitor ( fun , stat ) ; }
// ============================================================================
LoKi::FunctionFromFunction<const LHCb::Particle*>
LoKi::Dicts::monitor 
( const LoKi::Function<const LHCb::Particle*>&    fun  ,
  AIDA::IHistogram1D*                             stat )
{ return LoKi::monitor ( fun , stat ) ; }
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::VertexBase*>
LoKi::Dicts::monitor 
( const LoKi::Predicate<const LHCb::VertexBase*>& cut  ,
  StatEntity*                                     stat )
{ return LoKi::monitor ( cut , stat ) ; }
// ============================================================================
LoKi::FunctionFromFunction<const LHCb::VertexBase*>
LoKi::Dicts::monitor 
( const LoKi::Function<const LHCb::VertexBase*>&  fun  ,
  StatEntity*                                     stat )
{ return LoKi::monitor ( fun , stat ) ; }
// ============================================================================
LoKi::FunctionFromFunction<const LHCb::VertexBase*>
LoKi::Dicts::monitor 
( const LoKi::Function<const LHCb::VertexBase*>&  fun  ,
  AIDA::IHistogram1D*                             stat )
{ return LoKi::monitor ( fun  , stat ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
