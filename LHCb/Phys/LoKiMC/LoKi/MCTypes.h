// $Id: MCTypes.h,v 1.4 2006-04-09 09:03:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/03/14 19:04:29  ibelyaev
//  rename LoKi.cpp -> LoKiMC.cpp
//
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
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-10 
 */
// ============================================================================

namespace LoKi
{
  /** @namespace  MCTypes MCTypes.h LoKi/MCTypes.h
   *  
   *  Helper namespace to collect some useful MC data types 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-05-11
   */
  namespace MCTypes 
  {
    /// the basic imported type for the sequence of MC-particles 
    typedef std::vector<const LHCb::MCParticle*>  MCContainer      ;
    /// the basic imported type for the sequence of MC-vertices 
    typedef std::vector<const LHCb::MCVertex*>    MCVContainer     ;
    
    typedef LoKi::Selected_<MCContainer>    MCSelected       ;
    typedef LoKi::Selected_<MCVContainer>   MCVSelected      ;
    
    /// the actual type of the sequence of MC-particles 
    typedef MCSelected::Range               MCRange          ;
    /// the actual type of the sequence of MC-vertices  
    typedef MCVSelected::Range              MCVRange         ;
    
    /// type of 'cuts' for MCParticles       (interface)
    typedef LoKi::Predicate<const LHCb::MCParticle*>              MCCuts  ;
    /// type of 'functions' for <MCParticles  (interface)
    typedef LoKi::Function<const LHCb::MCParticle*>               MCFunc  ;
    /// type of 'cuts' for MCParticles       (assignable)
    typedef LoKi::PredicateFromPredicate<const LHCb::MCParticle*> MCCut   ;
    /// type of 'functions' for MCParticles  (assignable)
    typedef LoKi::FunctionFromFunction<const LHCb::MCParticle*>   MCFun   ;
    
    /// type of 'cuts' for MCVertices      (interface)
    typedef LoKi::Predicate<const LHCb::MCVertex*>                MCVCuts ;
    /// type of 'functions' for MCVertices (interface)
    typedef LoKi::Function<const LHCb::MCVertex*>                 MCVFunc ;
    /// type of 'cuts' for Vertices      (assignable)
    typedef LoKi::PredicateFromPredicate<const LHCb::MCVertex*>   MCVCut  ;
    /// type of 'functions' for Vertices (assignable)
    typedef LoKi::FunctionFromFunction<const LHCb::MCVertex*>     MCVFun  ;
    
  }; // end of namespace MCTypes
  
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
    
  };
  
}; // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCTYPES_H
// ============================================================================
