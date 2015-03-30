// $Id$
// ============================================================================
#ifndef LOKI_TRIGGERTYPES_H 
#define LOKI_TRIGGERTYPES_H 1
// ============================================================================
// Include files 
// ============================================================================
// HltBase 
// ============================================================================
#include "Event/HltCandidate.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/BasicFunctors.h"
// ============================================================================
/** @file
 *  Definition of generic Hlt-Candidates functors&types  
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
namespace  LoKi
{
  // ==========================================================================
  /** @namespace LoKi::TriggerTypes 
   *  collection of functors that deal with Hlt-objects 
   *  @see Hlt::Stage
   *  @see Hlt::Candidate
   *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
   *  @date 2010-08-01
   */
  namespace TriggerTypes 
  {
    // ========================================================================
    // the regular functors for Hlt::Candidate
    // ========================================================================
    
    /// type of 'cuts'      for Candidate(interface)
    typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate TC_Cuts    ;
    /// type of 'functions' for Candidates  (interface)
    typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Function  TC_Func    ;
    /// type of 'cuts'      for Candidates       (assignable)
    typedef LoKi::Assignable<TC_Cuts>::Type                        TC_Cut    ;
    /// type of 'functions' for Candidates  (assignable)
    typedef LoKi::Assignable<TC_Func>::Type                        TC_Fun    ;
    // ========================================================================
    
    /// type of 'map'       for Candidates    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Map       TC_Maps     ;
    /// type of 'map'       for Candidates    (assignable)
    typedef LoKi::Assignable<TC_Maps>::Type                        TC_Map     ;
    
    /// type of 'pipe'      for Candidates    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe      TC_Pipes    ;
    /// type of 'pipe'      for Candidates    (assignable)
    typedef LoKi::Assignable<TC_Pipes>::Type                       TC_Pipe     ;
    
    /// type of 'fun-val'   for Candidates    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Candidate*>::FunVal    TC_FunVals  ;
    /// type of 'fun-val'   for Candidates    (assignable)
    typedef LoKi::Assignable<TC_FunVals>::Type                     TC_FunVal   ;
    
    /// type of 'fun-val'   for Candidates    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Candidate*>::CutVal    TC_CutVals  ;
    /// type of 'fun-val'   for Candidates    (assignable)
    typedef LoKi::Assignable<TC_CutVals>::Type                     TC_CutVal   ;
    
    /// type of 'source'   for Candidates    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source    TC_Sources  ;
    /// type of 'source'   for Candidates    (assignable)
    typedef LoKi::Assignable<TC_Sources>::Type                     TC_Source   ;
    // ========================================================================

    // ========================================================================
    // the regular functors for Hlt::Stage
    // ========================================================================
    
    /// type of 'cuts'      for Stage(interface)
    typedef LoKi::BasicFunctors<const Hlt::Stage*>::Predicate TS_Cuts    ;
    /// type of 'functions' for Stages  (interface)
    typedef LoKi::BasicFunctors<const Hlt::Stage*>::Function  TS_Func    ;
    /// type of 'cuts'      for Stages       (assignable)
    typedef LoKi::Assignable<TS_Cuts>::Type                        TS_Cut    ;
    /// type of 'functions' for Stages  (assignable)
    typedef LoKi::Assignable<TS_Func>::Type                        TS_Fun    ;
    // ========================================================================
    
    /// type of 'map'       for Stages    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Stage*>::Map       TS_Maps     ;
    /// type of 'map'       for Stages    (assignable)
    typedef LoKi::Assignable<TS_Maps>::Type                        TS_Map     ;
    
    /// type of 'pipe'      for Stages    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Stage*>::Pipe      TS_Pipes    ;
    /// type of 'pipe'      for Stages    (assignable)
    typedef LoKi::Assignable<TS_Pipes>::Type                       TS_Pipe     ;
    
    /// type of 'fun-val'   for Stages    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Stage*>::FunVal    TS_FunVals  ;
    /// type of 'fun-val'   for Stages    (assignable)
    typedef LoKi::Assignable<TS_FunVals>::Type                     TS_FunVal   ;
    
    /// type of 'fun-val'   for Stages    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Stage*>::CutVal    TS_CutVals  ;
    /// type of 'fun-val'   for Stages    (assignable)
    typedef LoKi::Assignable<TS_CutVals>::Type                     TS_CutVal   ;
    
    /// type of 'source'   for Stages    (interface)
    typedef LoKi::BasicFunctors<const Hlt::Stage*>::Source    TS_Sources  ;
    /// type of 'source'   for Stages    (assignable)
    typedef LoKi::Assignable<TS_Sources>::Type                     TS_Source   ;
    // ========================================================================
    
    // ========================================================================
  } //                                               end of namespace LoKi::Hlt
  // ==========================================================================
  namespace Types
  {
    // ========================================================================
    // regular functions/predicates for Candidates : 
    // ========================================================================
    /// type of 'cuts' for Candidates       (interface)
    typedef LoKi::TriggerTypes::TC_Cuts              TC_Cuts  ;
    /// type of 'functions' for Candidates  (interface)
    typedef LoKi::TriggerTypes::TC_Func              TC_Func  ;
    /// type of 'cuts' for Candidates       (assignable)
    typedef LoKi::TriggerTypes::TC_Cut               TC_Cut   ;
    /// type of 'functions' for Candidates  (assignable)
    typedef LoKi::TriggerTypes::TC_Fun               TC_Fun   ;
    
    // ========================================================================
    // functional part for Candidates
    // ========================================================================
    
    /// type for "maps" for Candidate  (interface)
    typedef LoKi::TriggerTypes::TC_Maps     TC_Maps     ;
    /// type for "maps" for Candidate  (assignable)
    typedef LoKi::TriggerTypes::TC_Map      TC_Map      ;
    
    /// type for "pipes" for Candidate  (interface)
    typedef LoKi::TriggerTypes::TC_Pipes    TC_Pipes    ;
    /// type for "pipes" for Candidate  (assignable)
    typedef LoKi::TriggerTypes::TC_Pipe     TC_Pipe     ;
    
    /// type for "fun-vals" for Candidate  (interface)
    typedef LoKi::TriggerTypes::TC_FunVals  TC_FunVals  ;
    /// type for "fun-vals" for Candidate  (assignable)
    typedef LoKi::TriggerTypes::TC_FunVal   TC_FunVal   ;
    
    /// type for "fun-vals" for Candidate  (interface)
    typedef LoKi::TriggerTypes::TC_CutVals  TC_CutVals  ;
    /// type for "fun-vals" for Candidate  (assignable)
    typedef LoKi::TriggerTypes::TC_CutVal   TC_CutVal   ;
    
    /// type for "sources" for Candidate  (interface)
    typedef LoKi::TriggerTypes::TC_Sources TC_Sources   ;
    /// type for "sources" for Candidate  (assignable)
    typedef LoKi::TriggerTypes::TC_Source  TC_Source    ;

    // ========================================================================
    // regular functions/predicates for Stages 
    // ========================================================================
    /// type of 'cuts' for Stages       (interface)
    typedef LoKi::TriggerTypes::TS_Cuts              TS_Cuts  ;
    /// type of 'functions' for Stages  (interface)
    typedef LoKi::TriggerTypes::TS_Func              TS_Func  ;
    /// type of 'cuts' for Stages       (assignable)
    typedef LoKi::TriggerTypes::TS_Cut               TS_Cut   ;
    /// type of 'functions' for Stages  (assignable)
    typedef LoKi::TriggerTypes::TS_Fun               TS_Fun   ;
    
    // ========================================================================
    // functional part for Stages 
    // ========================================================================
    
    /// type for "maps" for Stage  (interface)
    typedef LoKi::TriggerTypes::TS_Maps     TS_Maps     ;
    /// type for "maps" for Stage  (assignable)
    typedef LoKi::TriggerTypes::TS_Map      TS_Map      ;
    
    /// type for "pipes" for Stage  (interface)
    typedef LoKi::TriggerTypes::TS_Pipes    TS_Pipes    ;
    /// type for "pipes" for Stage  (assignable)
    typedef LoKi::TriggerTypes::TS_Pipe     TS_Pipe     ;
    
    /// type for "fun-vals" for Stage  (interface)
    typedef LoKi::TriggerTypes::TS_FunVals  TS_FunVals  ;
    /// type for "fun-vals" for Stage  (assignable)
    typedef LoKi::TriggerTypes::TS_FunVal   TS_FunVal   ;
    
    /// type for "fun-vals" for Stage  (interface)
    typedef LoKi::TriggerTypes::TS_CutVals  TS_CutVals  ;
    /// type for "fun-vals" for Stage  (assignable)
    typedef LoKi::TriggerTypes::TS_CutVal   TS_CutVal   ;
    
    /// type for "sources" for Stage  (interface)
    typedef LoKi::TriggerTypes::TS_Sources TS_Sources   ;
    /// type for "sources" for Stage  (assignable)
    typedef LoKi::TriggerTypes::TS_Source  TS_Source    ;
    
    // ========================================================================
   } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================
  template <>
  std::ostream& 
  Constant<const Hlt::Candidate*,bool>::fillStream( std::ostream& s ) const ;
  // ==========================================================================  
  // the specialized printout 
  // ==========================================================================
  template <>
  std::ostream& 
  Constant<const Hlt::Stage*    ,bool>::fillStream( std::ostream& s ) const ;
  // ==========================================================================  
} //                                                      end of namespace LoKi
// // ============================================================================
#include "LoKi/Filters.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Functors 
  {
    // ========================================================================
    // the specific printout
    // ========================================================================
    template <>
    std::ostream& 
    Empty<const Hlt::Candidate*>::fillStream ( std::ostream& s ) const ; 
    // ========================================================================
    // the specific printpout
    // ========================================================================
    template <>
    std::ostream& 
    Size<const Hlt::Candidate*>::fillStream ( std::ostream& s ) const  ;
    // ========================================================================
    // the specific printout
    // ========================================================================
    template <>
    std::ostream& 
    Empty<const Hlt::Stage*>::fillStream ( std::ostream& s ) const ; 
    // ========================================================================
    // the specific printpout
    // ========================================================================
    template <>
    std::ostream& 
    Size<const Hlt::Stage*>::fillStream ( std::ostream& s ) const  ;
    // ========================================================================
  } //                                          end of namespace LoKi::Functors 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================

// ============================================================================
// Local 
// ============================================================================
// #include "LoKi/OldTypes.h"
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TRIGGERTYPES_H
// ============================================================================
