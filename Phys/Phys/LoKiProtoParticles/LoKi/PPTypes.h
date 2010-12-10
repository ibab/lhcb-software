// $Id$
// ============================================================================
#ifndef LOKI_PPTYPES_H 
#define LOKI_PPTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// RecEvent 
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKi
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/CoreTypes.h"
#include "LoKi/Functions.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::PPTypes   LoKi/PPTypes.h
   *  Collection of basic types  to deal with protoparticles 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-05-26
   */
  namespace PPTypes 
  {
    // ========================================================================
    // basic types for LHCb::ProtoParticle: 
    // ========================================================================    
    // 
    /// type of 'cuts' for LHCb::ProtoParticle       (interface)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Predicate PPCuts  ;
    /// type of 'functions' for LHCb::ProtoParticle  (interface)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Function  PPFunc  ;
    /// type of 'cuts' for LHCb::ProtoParticle       (assignable)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::PredicateFromPredicate  PPCut ; 
    /// type of 'functions' for LHCb::ProtoParticle  (assignable)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::FunctionFromFunction    PPFun ;    
    //
    // ========================================================================    
    // functional types for LHCb::ProtoParticle: 
    // ========================================================================    
    //
    /// type of "map"  for LHCb::ProtoParticle (interface)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Map       PPMaps     ;
    /// type of "map"  for LHCb::ProtoParticle (assignable)
    typedef LoKi::Assignable<PPMaps>::Type                             PPMap      ;
    //
    /// type of "pipe"  for LHCb::ProtoParticle (interface)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Pipe      PPPipes    ;
    /// type of "pipe"  for LHCb::ProtoParticle (assignable)
    typedef LoKi::Assignable<PPPipes>::Type                            PPPipe     ; 
    //
    /// type of "fun-val"  for LHCb::ProtoParticle (interface)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::FunVal    PPFunVals  ;
    /// type of "fun-val"  for LHCb::ProtoParticle (assignable)
    typedef LoKi::Assignable<PPFunVals>::Type                          PPFunVal   ; 
    //
    /// type of "fun-val"  for LHCb::ProtoParticle (interface)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::CutVal    PPCutVals  ;
    /// type of "fun-val"  for LHCb::ProtoParticle (assignable)
    typedef LoKi::Assignable<PPCutVals>::Type                          PPCutVal   ; 
    //
    /// type of "element"  for LHCb::ProtoParticle (interface)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Element   PPElements ;
    /// type of "element"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<PPElements>::Type                         PPElement  ; 
    //
    /// type of "source"  for LHCb::ProtoParticle (interface)
    typedef LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Source    PPSources  ;
    /// type of "source"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<PPSources>::Type                          PPSource   ;
    //
    // ========================================================================
  } //                                           end of namespace LoKi::PPTypes 
  // ==========================================================================
  namespace Types 
  {
    // ========================================================================
    // basic  types for LHCb::ProtoParticle 
    // ========================================================================
    //
    /// type of 'cuts' for LHCb::ProtoParticle        (interface)
    typedef LoKi::PPTypes::PPCuts                               PPCuts  ;
    /// type of 'functions' for LHCb::ProtoParticle   (interface)
    typedef LoKi::PPTypes::PPFunc                               PPFunc  ;
    /// type of 'cuts' for LHCb::ProtoParticle        (assignable)
    typedef LoKi::PPTypes::PPCut                                PPCut   ; 
    /// type of 'functions' for LHCb::ProtoParticle   (assignable)
    typedef LoKi::PPTypes::PPFun                                PPFun   ;
    //
    // ========================================================================
    // functional  types for LHCb::ProtoParticle 
    // ========================================================================
    //
    /// type for "map" for LHCb::ProtoParticle (interface)
    typedef LoKi::PPTypes::PPMaps                               PPMaps      ;
    /// type for "map" for LHCb::ProtoParticle (assignable)
    typedef LoKi::PPTypes::PPMap                                PPMap       ;
    /// type for "pipe" for LHCb::ProtoParticle (interface)
    typedef LoKi::PPTypes::PPPipes                              PPPipes     ;
    /// type for "pipe" for LHCb::ProtoParticle (assignable)
    typedef LoKi::PPTypes::PPPipe                               PPPipe      ;
    /// type for "fun-val" for LHCb::ProtoParticle (interface)
    typedef LoKi::PPTypes::PPFunVals                            PPFunVals   ;
    /// type for "fun-val" for LHCb::ProtoParticle (assignable)
    typedef LoKi::PPTypes::PPFunVal                             PPFunVal    ;
    /// type for "cut-val" for LHCb::ProtoParticle (interface)
    typedef LoKi::PPTypes::PPCutVals                            PPCutVals   ;
    /// type for "cut-val" for LHCb::ProtoParticle (assignable)
    typedef LoKi::PPTypes::PPCutVal                             PPCutVal    ;
    /// type for "element" for LHCb::ProtoParticle (interface)
    typedef LoKi::PPTypes::PPElements                           PPElements  ;
    /// type for "element" for LHCb::ProtoParticle (assignable)
    typedef LoKi::PPTypes::PPElement                            PPElement   ;
    /// type for "source" for LHCb::ProtoParticle (interface)
    typedef LoKi::PPTypes::PPSources                            PPSources   ;
    /// type for "source" for LHCb::ProtoParticle (assignable)
    typedef LoKi::PPTypes::PPSource                             PPSource    ;
    // ========================================================================
  } //                                             end of namespace LoKi::Types 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PPTYPES_H
// ============================================================================
