// $Id: TrackTypes.h,v 1.6 2008-11-22 17:21:57 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_TRACKTYPES_H 
#define LOKI_TRACKTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/BiFunctions.h"
// ============================================================================
// Forward declarations
// ============================================================================
namespace LHCb { class Track ; }
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::TrackTypes 
   *  helper namespace  to collect the types needed for "Track"-functions
   *  @author Vanya BELAYEV ibelyaev@physics.syr.edu
   *  @date 2007-07-06
   */
  namespace TrackTypes 
  {
    // ========================================================================
    // the regular functors for Tracks
    // ========================================================================
    
    /// type of 'cuts' for Tracks       (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Predicate TrCuts ;
    /// type of 'functions' for Tracks  (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Function  TrFunc ;
    /// type of 'cuts' for Tracks       (assignable)
    typedef LoKi::Assignable<TrCuts>::Type                     TrCut  ;
    /// type of 'functions' for Tracks  (assignable)
    typedef LoKi::Assignable<TrFunc>::Type                     TrFun  ;
    
    /// type of 'map'       for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Map       TrMaps ;
    /// type of 'map'       for Tracks    (assignable)
    typedef LoKi::Assignable<TrMaps>::Type                     TrMap  ;
    
    /// type of 'pipe'      for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Pipe      TrPipes ;
    /// type of 'pipe'      for Tracks    (assignable)
    typedef LoKi::Assignable<TrPipes>::Type                    TrPipe  ;
    
    /// type of 'fun-val'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::FunVal    TrFunVals ;
    /// type of 'fun-val'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrFunVals>::Type                  TrFunVal  ;
    
    /// type of 'fun-val'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::CutVal    TrCutVals ;
    /// type of 'fun-val'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrCutVals>::Type                  TrCutVal  ;
    
    /// type of 'element'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Element   TrElements ;
    /// type of 'element'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrElements>::Type                 TrElement  ;
    
    /// type of 'source'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Source    TrSources  ;
    /// type of 'source'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrSources>::Type                  TrSource   ;
    // ========================================================================
  } //                                        end of namespace LoKi::TrackTypes 
  // ==========================================================================
  namespace Types 
  {
    // ========================================================================
    // regular functions/predicates for tracks: "Tr"
    // ========================================================================
    /// type of 'cuts' for Tracks       (interface)
    typedef LoKi::TrackTypes::TrCuts              TrCuts  ;
    /// type of 'functions' for Tracks  (interface)
    typedef LoKi::TrackTypes::TrFunc              TrFunc  ;
    /// type of 'cuts' for Tracks       (assignable)
    typedef LoKi::TrackTypes::TrCut               TrCut   ;
    /// type of 'functions' for Tracks  (assignable)
    typedef LoKi::TrackTypes::TrFun               TrFun   ;

    // ========================================================================
    // functional part for Tracks :
    // ========================================================================

    /// type for "maps" for Track  (interface)
    typedef LoKi::TrackTypes::TrMaps     TrMaps    ;
    /// type for "maps" for Track  (assignable)
    typedef LoKi::TrackTypes::TrMap      TrMap     ;
    
    /// type for "pipes" for Track  (interface)
    typedef LoKi::TrackTypes::TrPipes    TrPipes   ;
    /// type for "pipes" for Track  (assignable)
    typedef LoKi::TrackTypes::TrPipe     TrPipe    ;
    
    /// type for "fun-vals" for Track  (interface)
    typedef LoKi::TrackTypes::TrFunVals  TrFunVals ;
    /// type for "fun-vals" for Track  (assignable)
    typedef LoKi::TrackTypes::TrFunVal   TrFunVal  ;
    
    /// type for "fun-vals" for Track  (interface)
    typedef LoKi::TrackTypes::TrCutVals  TrCutVals ;
    /// type for "fun-vals" for Track  (assignable)
    typedef LoKi::TrackTypes::TrCutVal   TrCutVal  ;
    
    /// type for "elements" for Track  (interface)
    typedef LoKi::TrackTypes::TrElements TrElements ;
    /// type for "elements" for Track  (assignable)
    typedef LoKi::TrackTypes::TrElement  TrElement  ;
    
    /// type for "sources" for Track  (interface)
    typedef LoKi::TrackTypes::TrSources TrSources   ;
    /// type for "sources" for Track  (assignable)
    typedef LoKi::TrackTypes::TrSource  TrSource    ;
    
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
  std::ostream& 
  Constant<const LHCb::Track*,bool>::fillStream( std::ostream& s ) const ;
  // ==========================================================================  
} //                                                      end of namespace LoKi
// ============================================================================
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
    std::ostream& Empty<const LHCb::Track*>::fillStream ( std::ostream& s ) const ; 
    // ========================================================================
    // the specific printpout
    // ========================================================================
    template <>
    std::ostream& 
    Size<const LHCb::Track*>::fillStream ( std::ostream& s ) const  ;
    // ========================================================================
  } //                                          end of namespace LoKi::Functors 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================

// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TRACKTYPES_H
// ============================================================================
