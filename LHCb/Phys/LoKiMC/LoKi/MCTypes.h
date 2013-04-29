// $Id$
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
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace  LoKi::MCTypes LoKi/MCTypes.h
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
    // ========================================================================
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
    // ========================================================================
    // "functional" types for LHCb::MCParticle
    // ========================================================================
    /// type of "map" for LHCb::MCParticle   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Map     MCMaps     ;
    /// type of "map" for LHCb::MCParticle   (assignable)
    typedef LoKi::Assignable<MCMaps>::Type                        MCMap      ;
    /// type of "pipe" for LHCb::MCParticle   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Pipe    MCPipes    ;
    /// type of "pipe" for LHCb::MCParticle   (assignable)
    typedef LoKi::Assignable<MCPipes>::Type                       MCPipe     ;
    /// type of "fun-val" for LHCb::MCParticle   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::FunVal  MCFunVals  ;
    /// type of "fun-val" for LHCb::MCParticle   (assignable)
    typedef LoKi::Assignable<MCFunVals>::Type                     MCFunVal   ;
    /// type of "cut-val" for LHCb::MCParticle   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::CutVal  MCCutVals  ;
    /// type of "cut-val" for LHCb::MCParticle   (assignable)
    typedef LoKi::Assignable<MCCutVals>::Type                     MCCutVal   ;
    /// type of "source" for LHCb::MCParticle   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Source  MCSources  ;
    /// type of "source" for LHCb::MCParticle   (assignable)
    typedef LoKi::Assignable<MCSources>::Type                     MCSource   ;
    // ========================================================================
    // "functional" types for LHCb::MCVertex
    // ========================================================================
    /// type of "map" for LHCb::MCVertex   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::Map       MCVMaps     ;
    /// type of "map" for LHCb::MCVertex   (assignable)
    typedef LoKi::Assignable<MCVMaps>::Type                       MCVMap      ;
    /// type of "pipe" for LHCb::MCVertex   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::Pipe      MCVPipes    ;
    /// type of "pipe" for LHCb::MCVertex   (assignable)
    typedef LoKi::Assignable<MCVPipes>::Type                      MCVPipe     ;
    /// type of "fun-val" for LHCb::MCVertex   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::FunVal    MCVFunVals  ;
    /// type of "fun-val" for LHCb::MCVertex   (assignable)
    typedef LoKi::Assignable<MCVFunVals>::Type                    MCVFunVal   ;
    /// type of "cut-val" for LHCb::MCVertex   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::CutVal    MCVCutVals  ;
    /// type of "cut-val" for LHCb::MCVertex   (assignable)
    typedef LoKi::Assignable<MCVCutVals>::Type                    MCVCutVal   ;
    /// type of "source" for LHCb::MCVertex   (interface)
    typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::Source    MCVSources  ;
    /// type of "source" for LHCb::MCVertex   (assignable)
    typedef LoKi::Assignable<MCVSources>::Type                    MCVSource   ;
    // ========================================================================
  } //                                           end of namespace LoKi::MCTypes
  // ==========================================================================
  namespace Types 
  { 
    // ========================================================================
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
    // ========================================================================
    // "functional" types for LHCb::MCParticle
    // ========================================================================
    /// type of "map"     for LHCb::MCParticle   (interface)
    typedef LoKi::MCTypes::MCMaps                MCMaps      ;
    /// type of "map"     for LHCb::MCParticle   (assignable)
    typedef LoKi::MCTypes::MCMap                 MCMap       ;
    /// type of "pipe"    for LHCb::MCParticle   (interface)
    typedef LoKi::MCTypes::MCPipes               MCPipes     ;
    /// type of "pipe"    for LHCb::MCParticle   (assignable)
    typedef LoKi::MCTypes::MCPipe                MCPipe      ;
    /// type of "fun-val" for LHCb::MCParticle   (interface)
    typedef LoKi::MCTypes::MCFunVals             MCFunVals   ;
    /// type of "fun-val" for LHCb::MCParticle   (assignable)
    typedef LoKi::MCTypes::MCFunVal              MCFunVal    ;
    /// type of "cut-val" for LHCb::MCParticle   (interface)
    typedef LoKi::MCTypes::MCCutVals             MCCutVals   ;
    /// type of "cut-val" for LHCb::MCParticle   (assignable)
    typedef LoKi::MCTypes::MCCutVal              MCCutVal    ;
    /// type of "source"  for LHCb::MCParticle   (interface)
    typedef LoKi::MCTypes::MCSources             MCSources   ;
    /// type of "source"  for LHCb::MCParticle   (assignable)
    typedef LoKi::MCTypes::MCSource              MCSource    ;
    // ========================================================================
    // "functional" types for LHCb::MCVertex
    // ========================================================================
    /// type of "map"     for LHCb::MCVertex     (interface)
    typedef LoKi::MCTypes::MCVMaps               MCVMaps     ;
    /// type of "map"     for LHCb::MCVertex     (assignable)
    typedef LoKi::MCTypes::MCVMap                MCVMap      ;
    /// type of "pipe"    for LHCb::MCVertex     (interface)
    typedef LoKi::MCTypes::MCVPipes              MCVPipes    ;
    /// type of "pipe"    for LHCb::MCVertex     (assignable)
    typedef LoKi::MCTypes::MCVPipe               MCVPipe     ;
    /// type of "fun-val" for LHCb::MCVertex     (interface)
    typedef LoKi::MCTypes::MCVFunVals            MCVFunVals  ;
    /// type of "fun-val" for LHCb::MCVertex     (assignable)
    typedef LoKi::MCTypes::MCVFunVal             MCVFunVal   ;
    /// type of "cut-val" for LHCb::MCVertex     (interface)
    typedef LoKi::MCTypes::MCVCutVals            MCVCutVals  ;
    /// type of "cut-val" for LHCb::MCVertex     (assignable)
    typedef LoKi::MCTypes::MCVCutVal             MCVCutVal   ;
    /// type of "source"  for LHCb::MCVertex     (interface)
    typedef LoKi::MCTypes::MCVSources            MCVSources  ;
    /// type of "source"  for LHCb::MCVertex     (assignable)
    typedef LoKi::MCTypes::MCVSource             MCVSource   ;
    // ========================================================================
  } //                                             end of namespace LoKi::Types
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================
  template <>
  inline std::ostream& 
  Constant<const LHCb::MCParticle*,bool>::fillStream( std::ostream& s ) const 
  { return s << ( this->m_value ? "MCALL" : "MCNONE"  ) ; }
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================  
  template <>
  inline std::ostream& 
  Constant<const LHCb::MCVertex*,bool>::fillStream  ( std::ostream& s ) const 
  { return s << ( this->m_value ? "MCVALL" : "MCVNONE" ) ; }
  // ==========================================================================  
} //                                                      end of namespace LoKi
// ============================================================================
// a little bit of streamers 
// ============================================================================
GAUDI_API 
LHCb::MCParticle::ConstVector 
operator>>
( const LHCb::MCParticle::ConstVector&    input , 
  const LoKi::Types::MCCuts&              cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::MCParticle::ConstVector 
operator>> 
( const LHCb::MCParticle::Container*      input , 
  const LoKi::Types::MCCuts&              cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::MCParticle::ConstVector 
operator>> 
( const LoKi::MCTypes::MCRange&           input , 
  const LoKi::Types::MCCuts&              cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::MCParticle::ConstVector 
operator>> 
( const SmartRefVector<LHCb::MCParticle>& input , 
  const LoKi::Types::MCCuts&              cuts  ) ;
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_MCTYPES_H
// ============================================================================
