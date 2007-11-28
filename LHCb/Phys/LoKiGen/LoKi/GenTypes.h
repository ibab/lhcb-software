// $Id: GenTypes.h,v 1.9 2007-11-28 14:08:29 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENTYPES_H 
#define LOKI_GENTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/CoreTypes.h"
#include "LoKi/Functions.h"
#include "LoKi/Selected.h"
#include "LoKi/RangeList.h"
// ============================================================================
// forward declarations  
// ============================================================================
namespace HepMC { class GenParticle ; } 
namespace HepMC { class GenVertex   ; } 
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{
  /** @namespace LoKi::GenTypes GenTypes.h LoKi/GenTypes.h
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date   2005-03-27
   */
  namespace GenTypes 
  {
    /// the basic imported type for the sequence of HepMC-particles 
    typedef std::vector<const HepMC::GenParticle*>  GenContainer      ;
    /// the basic imported type for the sequence of HepMC-vertices 
    typedef std::vector<const HepMC::GenVertex*>    GenVContainer     ;
    //
    typedef LoKi::Selected_<GenContainer>           GenSelected       ;
    typedef LoKi::Selected_<GenVContainer>          GenVSelected      ;
    //
    /// the actual type of the range of HepMC-particles 
    typedef GenSelected::Range                      GRange            ;
    /// the actual type of the range  of HepMC-vertices  
    typedef GenVSelected::Range                     GVRange           ;
    /// type of 'cuts' for HepMC::GenParticle        (interface)
    typedef LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate GCuts ;
    /// type of 'functions' for HEpMC::GenParticle   (interface)
    typedef LoKi::BasicFunctors<const HepMC::GenParticle*>::Function  GFunc ;
    /// type of 'cuts' for HepMC::GenParticle        (assignable)
    typedef LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate GCut  ;
    /// type of 'functions' for HepMC::GenParticle   (assignable)
    typedef LoKi::BasicFunctors<const   HepMC::GenParticle*>::FunctionFromFunction GFun  ;
    //
    /// type of 'cuts' for HepMC::GenVertex          (interface)
    typedef LoKi::BasicFunctors<const HepMC::GenVertex*>::Predicate GVCuts ;
    /// type of 'functions' for HepMC::GenVertex     (interface)
    typedef LoKi::BasicFunctors<const HepMC::GenVertex*>::Function  GVFunc ;
    /// type of 'cuts' for HepMC::GenVertex          (assignable)
    typedef LoKi::BasicFunctors<const HepMC::GenVertex*>::PredicateFromPredicate GVCut ;
    /// type of 'functions' for HepMC::GenVertex     (assignable)
    typedef LoKi::BasicFunctors<const HepMC::GenVertex*>::FunctionFromFunction   GVFun ;
    
  }  // end of namespace GenTypes
  namespace Types
  {
    /// type of 'cuts' for HepMC::GenParticle       (interface)
    typedef LoKi::GenTypes::GCuts     GCuts ;
    /// type of 'functions' for HepMC::GenParticle  (interface)
    typedef LoKi::GenTypes::GFunc     GFunc ;
    /// type of 'cuts' for HepMC::GenParticle       (assignable)
    typedef LoKi::GenTypes::GCut      GCut  ;
    /// type of 'functions' for HepMC::GenParticle  (assignable)
    typedef LoKi::GenTypes::GFun      GFun  ;
    /// type of 'cuts' for HepMC::GenVertex       (interface)
    typedef LoKi::GenTypes::GVCuts    GVCuts ;
    /// type of 'functions' for HepMC::GenVertex  (interface)
    typedef LoKi::GenTypes::GVFunc    GVFunc ;
    /// type of 'cuts' for HepMC::GenVertex       (assignable)
    typedef LoKi::GenTypes::GVCut     GVCut  ;
    /// type of 'functions' for HepMC::GenVertex  (assignable)
    typedef LoKi::GenTypes::GVFun     GVFun  ;  
    /// the actual type of the range of HepMC-particles 
    typedef LoKi::GenTypes::GRange    GRange  ;
    /// the actual type of the range of HepMC-vertices  
    typedef LoKi::GenTypes::GVRange   GVRange ;
    /// Range list for HepMC-particles:
    typedef LoKi::RangeList_<GRange>  GRangeL  ;
    /// Range list for HepMC-vertices
    typedef LoKi::RangeList_<GVRange> GVRangeL ;
    //
  }  // end of namespace Types
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENTYPES_H
// ============================================================================
