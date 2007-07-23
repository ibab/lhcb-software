// $Id: PhysTypes.h,v 1.7 2007-07-23 17:35:48 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHYSTYPES_H 
#define LOKI_PHYSTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/CoreTypes.h"
#include "LoKi/Functions.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
namespace LHCb { class RecVertex ; }
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::PhysTypes PhysTypes.h LoKi/PhysTypes.h
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu 
   *  @date   2006-02-11
   */
  namespace  PhysTypes 
  {
    /// type of 'cuts' for LHCb::Particle       (interface)
    typedef LoKi::Predicate<const LHCb::Particle*>                Cuts  ;
    /// type of 'functions' for LHCb::Particle  (interface)
    typedef LoKi::Function<const LHCb::Particle*>                 Func  ;
    /// type of 'cuts' for LHCb::Particle       (assignable)
    typedef LoKi::PredicateFromPredicate<const LHCb::Particle*>   Cut   ; 
    /// type of 'functions' for LHCb::Particle  (assignable)
    typedef LoKi::FunctionFromFunction<const LHCb::Particle*>     Fun   ;
    /// type of 'cuts' for LHCb::VertexBase     (interface)
    typedef LoKi::Predicate<const LHCb::VertexBase*>              VCuts ;
    /// type of 'functions' for LHCb::VertexBase (interface)
    typedef LoKi::Function<const LHCb::VertexBase*>               VFunc ;
    /// type of 'cuts' for LHCb::Vertex         (assignable)
    typedef LoKi::PredicateFromPredicate<const LHCb::VertexBase*> VCut  ; 
    /// type of 'functions' for LHCb::Vertex    (assignable)
    typedef LoKi::FunctionFromFunction<const LHCb::VertexBase*>   VFun  ;
    //
  }  // end of namespace LoKi::PhysTypes
  namespace Types 
  {    
    /// type of 'cuts' for LHCb::Particle        (interface)
    typedef LoKi::PhysTypes::Cuts                               Cuts  ;
    /// type of 'functions' for LHCb::Particle   (interface)
    typedef LoKi::PhysTypes::Func                               Func  ;
    /// type of 'cuts' for LHCb::Particle        (assignable)
    typedef LoKi::PhysTypes::Cut                                Cut   ; 
    /// type of 'functions' for LHCb::Particle   (assignable)
    typedef LoKi::PhysTypes::Fun                                Fun   ;
    /// type of 'cuts' for LHCb::VertexBase      (interface)
    typedef LoKi::PhysTypes::VCuts                              VCuts ;
    /// type of 'functions' for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VFunc                              VFunc ;
    /// type of 'cuts' for LHCb::VertexBase      (assignable)
    typedef LoKi::PhysTypes::VCut                               VCut  ; 
    /// type of 'functions' for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VFun                               VFun  ;  
    //
  }  // end of namespace LoKi::Types 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSTYPES_H
// ============================================================================
