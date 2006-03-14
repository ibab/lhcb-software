// $Id: PhysTypes.h,v 1.3 2006-03-14 19:32:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
namespace LHCb
{
  class Particle ;
  class Vertex   ;
};
// ============================================================================

namespace LoKi 
{
  /** @namespace PhysTypes PhysTypes.h LoKi/PhysTypes.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu 
   *  @date   2006-02-11
   */
  namespace  PhysTypes 
  {
    /// type of 'cuts' for LHCb::Particle       (interface)
    typedef LoKi::Predicate<const LHCb::Particle*>              Cuts  ;
    /// type of 'functions' for LHCb::Particle  (interface)
    typedef LoKi::Function<const LHCb::Particle*>               Func  ;
    /// type of 'cuts' for LHCb::Particle       (assignable)
    typedef LoKi::PredicateFromPredicate<const LHCb::Particle*> Cut   ; 
    /// type of 'functions' for LHCb::Particle  (assignable)
    typedef LoKi::FunctionFromFunction<const LHCb::Particle*>   Fun   ;
    /// type of 'cuts' for LHCb::Vertex         (interface)
    typedef LoKi::Predicate<const LHCb::Vertex*>                VCuts ;
    /// type of 'functions' for LHCb::Vertex    (interface)
    typedef LoKi::Function<const LHCb::Vertex*>                 VFunc ;
    /// type of 'cuts' for LHCb::Vertex         (assignable)
    typedef LoKi::PredicateFromPredicate<const LHCb::Vertex*>   VCut  ; 
    /// type of 'functions' for LHCb::Vertex    (assignable)
    typedef LoKi::FunctionFromFunction<const LHCb::Vertex*>     VFun  ;
    
  } ; // end of namespace LoKi::PhysTypes 
  
  namespace Types 
  {    
    /// type of 'cuts' for LHCb::Particle       (interface)
    typedef LoKi::PhysTypes::Cuts                               Cuts  ;
    /// type of 'functions' for LHCb::Particle  (interface)
    typedef LoKi::PhysTypes::Func                               Func  ;
    /// type of 'cuts' for LHCb::Particle       (assignable)
    typedef LoKi::PhysTypes::Cut                                Cut   ; 
    /// type of 'functions' for LHCb::Particle  (assignable)
    typedef LoKi::PhysTypes::Fun                                Fun   ;
    /// type of 'cuts' for LHCb::Vertex         (interface)
    typedef LoKi::PhysTypes::VCuts                              VCuts ;
    /// type of 'functions' for LHCb::Vertex    (interface)
    typedef LoKi::PhysTypes::VFunc                              VFunc ;
    /// type of 'cuts' for LHCb::Vertex         (assignable)
    typedef LoKi::PhysTypes::VCut                               VCut  ; 
    /// type of 'functions' for LHCb::Vertex    (assignable)
    typedef LoKi::PhysTypes::VFun                               VFun  ;  

  } ;

} ; // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSTYPES_H
// ============================================================================
