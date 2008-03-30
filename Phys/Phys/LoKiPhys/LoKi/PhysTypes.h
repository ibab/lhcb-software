// $Id: PhysTypes.h,v 1.10 2008-03-30 13:43:36 ibelyaev Exp $
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
  // ==========================================================================
  /** @namespace LoKi::PhysTypes LoKi/PhysTypes.h
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
    
    // functional types for LHCb::Particle: 
    
    // type of "map"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Map       Maps    ;
    // type of "map"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<Maps>::Type                          Map     ;
    
    // type of "pipe"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Pipe      Pipes   ;
    // type of "pipe"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<Pipes>::Type                         Pipe    ; 
    
    // type of "fun-val"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::FunVal    FunVals ;
    // type of "fun-val"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<FunVals>::Type                       FunVal  ; 
    
    // type of "element"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Element   Elements ;
    // type of "element"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<Elements>::Type                      Element  ; 

    // type of "source"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Source    Sources  ;
    // type of "source"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<Sources>::Type                       Source   ;

    // functional types for LHCb::VertexBase
    
    // type of "map"  for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Map     VMaps    ;
    // type of "map"  for LHCb::VertexBase (assignable)
    typedef LoKi::Assignable<VMaps>::Type                         VMap     ;
    
    // type of "pipe"  for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Pipe    VPipes   ;
    // type of "pipe"  for LHCb::VertexBase (assignable)
    typedef LoKi::Assignable<VPipes>::Type                        VPipe    ; 
    
    // type of "fun-val"  for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::FunVal  VFunVals ;
    // type of "fun-val"  for LHCb::VertexBase (assignable)
    typedef LoKi::Assignable<VFunVals>::Type                      VFunVal  ; 
    
    // type of "element"  for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Element VElements ;
    // type of "element"  for LHCb::VertexBase (assignable)
    typedef LoKi::Assignable<VElements>::Type                     VElement  ; 

    // type of "source"  for  LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  VSources  ;
    // type of "source"  for Particles (assignable)
    typedef LoKi::Assignable<VSources>::Type                      VSource   ;


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

    // functional  types for LHCb::Particle 

    // type for "map" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::Maps                               Maps      ;
    // type for "map" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::Map                                Map       ;
    // type for "pipe" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::Pipes                              Pipes     ;
    // type for "pipe" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::Pipe                               Pipe      ;
    // type for "fun-val" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::FunVals                            FunVals   ;
    // type for "fun-val" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::FunVal                             FunVal    ;
    // type for "element" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::Elements                           Elements  ;
    // type for "element" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::Element                            Element   ;
    // type for "source" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::Sources                            Sources   ;
    // type for "source" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::Source                             Source    ;

    // functional  types for LHcb::VertexBase 

    // type for "map" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VMaps                              VMaps     ;
    // type for "map" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VMap                               VMap      ;
    // type for "pipe" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VPipes                             VPipes    ;
    // type for "pipe" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VPipe                              VPipe     ;
    // type for "fun-val" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VFunVals                           VFunVals  ;
    // type for "fun-val" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VFunVal                            VFunVal   ;
    // type for "element" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VElements                          VElements ;
    // type for "element" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VElement                           VElement  ;
    // type for "source" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VSources                           VSources  ;
    // type for "source" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VSource                            VSource   ;

  }  // end of namespace LoKi::Types 
} // end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================
  template <>
  inline std::ostream& 
  Constant<const LHCb::Particle*,bool>::fillStream   ( std::ostream& s ) const 
  { return s << ( this->m_value ? "ALL" : "NONE" ) ; }
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================  
  template <>
  inline std::ostream& 
  Constant<const LHCb::VertexBase*,bool>::fillStream ( std::ostream& s ) const 
  { return s << ( this->m_value ? "VALL" : "VNONE" ) ; }
  // ==========================================================================  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSTYPES_H
// ============================================================================
