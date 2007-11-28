// $Id: PhysTypes.h,v 1.8 2007-11-28 14:39:30 ibelyaev Exp $
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
namespace LoKi 
{
  /** @namespace LoKi::PhysTypes PhysTypes.h LoKi/PhysTypes.h
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu 
   *  @date   2006-02-11
   */
  namespace  PhysTypes 
  {
    /// type of 'cuts' for LHCb::Particle       (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Predicate Cuts  ;
    /// type of 'functions' for LHCb::Particle  (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Function  Func  ;
    /// type of 'cuts' for LHCb::Particle       (assignable)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate  Cut ; 
    /// type of 'functions' for LHCb::Particle  (assignable)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::FunctionFromFunction    Fun ;
    /// type of 'cuts' for LHCb::VertexBase     (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate VCuts ;
    /// type of 'functions' for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Function  VFunc ;
    /// type of 'cuts' for LHCb::Vertex         (assignable)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::PredicateFromPredicate VCut ; 
    /// type of 'functions' for LHCb::Vertex    (assignable)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::FunctionFromFunction   VFun ;
    //
  } // end of namespace LoKi::PhysTypes
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
