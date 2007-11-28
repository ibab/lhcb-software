// $Id: MCTypes.h,v 1.10 2007-11-28 14:13:59 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCTYPES_H 
#define LOKI_MCTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// Event
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Selected.h"
#include "LoKi/RangeList.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-10 
 */
// ============================================================================
namespace LoKi
{
  /** @namespace  LoKi::MCTypes MCTypes.h LoKi/MCTypes.h
   *  
   *  Helper namespace to collect some useful MC data types 
   *
   *  
   *  @todo use LHCb::MCVertex::ConstVertex instead of explicit type 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-05-11
   */
  namespace MCTypes 
  {
    /// the basic imported type for the sequence of MC-particles 
    typedef LHCb::MCParticle::ConstVector   MCContainer      ;
    /// the basic imported type for the sequence of MC-vertices 
    typedef LHCb::MCVertex::ConstVector     MCVContainer     ;
    
    typedef LoKi::Selected_<MCContainer>    MCSelected       ;
    typedef LoKi::Selected_<MCVContainer>   MCVSelected      ;
    
    /// the actual type of the sequence of MC-particles 
    typedef MCSelected::Range               MCRange          ;
    /// the actual type of the sequence of MC-vertices  
    typedef MCVSelected::Range              MCVRange         ;
    
    /// type of 'cuts' for MCParticles       (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate MCCuts ;
    /// type of 'functions' for <MCParticles  (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function  MCFunc ;
    /// type of 'cuts' for MCParticles       (assignable)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate MCCut   ;
    /// type of 'functions' for MCParticles  (assignable)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::FunctionFromFunction   MCFun   ;
    
    /// type of 'cuts' for MCVertices      (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::Predicate MCVCuts ;
    /// type of 'functions' for MCVertices (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::Function  MCVFunc ;
    /// type of 'cuts' for Vertices      (assignable)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::PredicateFromPredicate MCVCut ;
    /// type of 'functions' for Vertices (assignable)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::FunctionFromFunction   MCVFun ;
    //
  } // end of namespace LoKi::MCTypes  
  namespace Types 
  { 
    /// the actual type of the sequence of MC-particles 
    typedef LoKi::MCTypes::MCRange  MCRange ;
    /// the actual type of the sequence of MC-vertices  
    typedef LoKi::MCTypes::MCVRange MCVRange ; 
    /// type of 'cuts' for MCParticles       (interface)
    typedef LoKi::MCTypes::MCCuts  MCCuts ;
    /// type of 'functions' for <MCParticles  (interface)
    typedef LoKi::MCTypes::MCFunc  MCFunc ;
    /// type of 'cuts' for MCParticles       (assignable)
    typedef LoKi::MCTypes::MCCut   MCCut  ;
    /// type of 'functions' for MCParticles  (assignable)
    typedef LoKi::MCTypes::MCFun   MCFun  ;
    /// type of 'cuts' for MCVertices        (interface)
    typedef LoKi::MCTypes::MCVCuts MCVCuts ;
    /// type of 'functions' for MCVertices   (interface)
    typedef LoKi::MCTypes::MCVFunc MCVFunc ;
    /// type of 'cuts' for Vertices          (assignable)
    typedef LoKi::MCTypes::MCVCut  MCVCut  ;
    /// type of 'functions' for Vertices     (assignable)
    typedef LoKi::MCTypes::MCVFun  MCVFun  ;
    /// the actual type of the range list  
    typedef LoKi::RangeList_<MCRange>  MCRangeL  ;
    /// the actual type of the range list 
    typedef LoKi::RangeList_<MCVRange> MCVRangeL ; 
    //
  } // end of namespace LoKi::Types  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCTYPES_H
// ============================================================================
