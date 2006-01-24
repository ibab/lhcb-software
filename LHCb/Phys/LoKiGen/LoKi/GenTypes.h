// $Id: GenTypes.h,v 1.1.1.1 2006-01-24 09:54:23 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_GENTYPES_H 
#define LOKI_GENTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
// ============================================================================
// forward declarations  
// ============================================================================
namespace HepMC { class GenParticle ; } ;
// ============================================================================


// ============================================================================
/** @file
 *
 *  Collection of useful function to extract the information from  HepMC 
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
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{
  /** @namespace GenTypes GenTypes.h LoKi/GenTypes.h
   *  
   *
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date   2005-03-27
   */
  namespace GenTypes 
  {
    /// type of 'cuts' for HepMC::GenParticle       (interface)
    typedef LoKi::Predicate<const HepMC::GenParticle*>              GCuts ;
    /// type of 'functions' for HEpMC::GenParticle  (interface)
    typedef LoKi::Function<const HepMC::GenParticle*>               GFunc ;
    /// type of 'cuts' for HepMC::GenParticle        (assignable)
    typedef LoKi::PredicateFromPredicate<const HepMC::GenParticle*> GCut  ;
    /// type of 'functions' for HepMC::GenParticle   (assignable)
    typedef LoKi::FunctionFromFunction<const   HepMC::GenParticle*> GFun  ;
    
  }; // end of namespace GenTypes 
  
  namespace Types
  {
    /// type of 'cuts' for HepMC::GenParticle       (interface)
    typedef LoKi::GenTypes::GCuts GCuts ;
    /// type of 'functions' for HEpMC::GenParticle  (interface)
    typedef LoKi::GenTypes::GFunc GFunc ;
    /// type of 'cuts' for HepMC::GenParticle       (assignable)
    typedef LoKi::GenTypes::GCut  GCut  ;
    /// type of 'functions' for HepMC::GenParticle  (assignable)
    typedef LoKi::GenTypes::GFun  GFun  ;
    
  } ; // end of namespace Types 
  
}; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENTYPES_H
// ============================================================================
