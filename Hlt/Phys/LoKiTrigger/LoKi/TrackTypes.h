// $Id: TrackTypes.h,v 1.2 2007-11-28 14:56:23 ibelyaev Exp $
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
namespace LHCb
{
  class Track     ;
  class RecVertex ;
}
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::TrackTypes 
   *  helper namespace  to collect the types neede for "Track"-functions
   *  @author Vanya BELAYEV ibelyaev@physics.syr.edu
   *  @date 2007-07-06
   */
  namespace TrackTypes 
  {
    // ========================================================================
    // the regular functors for Tracks
    // ========================================================================
    
    /// type of 'cuts' for Tracks       (interface)
    typedef LoKi::BasicFunctors<LHCb::Track>::Predicate             TrCuts ;
    /// type of 'functions' for Tracks  (interface)
    typedef LoKi::BasicFunctors<LHCb::Track>::Function              TrFunc ;
    /// type of 'cuts' for Tracks       (assignable)
    typedef LoKi::BasicFunctors<LHCb::Track>::PredicateFromPredicate TrCut ;
    /// type of 'functions' for Tracks  (assignable)
    typedef LoKi::BasicFunctors<LHCb::Track>::FunctionFromFunction   TrFun ;
    
    // ========================================================================
    // the regular functors for reconstructed vertics 
    // ========================================================================
    
    /// type of 'cuts' for recontructed vertics        (interface)
    typedef LoKi::BasicFunctors<LHCb::RecVertex>::Predicate              RVCuts ;
    /// type of 'functions' for recontructed vertices (interface)
    typedef LoKi::BasicFunctors<LHCb::RecVertex>::Function               RVFunc ;
    /// type of 'cuts' for recontructed vertices (assignable)
    typedef LoKi::BasicFunctors<LHCb::RecVertex>::PredicateFromPredicate RVCut  ;
    /// type of 'functions' for recontructed vertices (assignable)
    typedef LoKi::BasicFunctors<LHCb::RecVertex>::FunctionFromFunction   RVFun  ;
    
    /// The *fictive* argument for  "Track Track bi-functions"
    typedef LoKi::Holder<LHCb::Track,LHCb::Track>                 TrackPair ;
    
    // ========================================================================
    // the functors for two tracks  
    // ========================================================================
    
    /// the type of 'cuts'     for Track-Vertex pair  (interface)
    typedef LoKi::BasicFunctors<TrackPair>::Predicate              TTrCuts   ;
    /// the type of 'function' for Track-Vertex pair  (interface)
    typedef LoKi::BasicFunctors<TrackPair>::Function               TTrFunc   ;
    /// type of 'cuts' for Track-VertexPair        (assignable)
    typedef LoKi::BasicFunctors<TrackPair>::PredicateFromPredicate TTrCut    ;
    /// type of 'functions' for Track-Vertex-Pair  (assignable)
    typedef LoKi::BasicFunctors<TrackPair>::FunctionFromFunction   TTrFun    ;
    
    /// The *fictive* argument for  "Track-Vertex bi-functions"
    typedef LoKi::Holder<LHCb::Track,LHCb::RecVertex>       TrackVertexPair ;
    
    // ========================================================================
    // the functors for track&vertex 
    // ========================================================================
    
    /// the type of 'cuts'     for Track-Vertex pair  (interface)
    typedef LoKi::BasicFunctors<TrackVertexPair>::Predicate              TrVCuts ;
    /// the type of 'function' for Track-Vertex pair  (interface)
    typedef LoKi::BasicFunctors<TrackVertexPair>::Function               TrVFunc ;
    /// type of 'cuts' for Track-VertexPair        (assignable)
    typedef LoKi::BasicFunctors<TrackVertexPair>::PredicateFromPredicate TrVCut  ;
    /// type of 'functions' for Track-Vertex-Pair  (assignable)
    typedef LoKi::BasicFunctors<TrackVertexPair>::FunctionFromFunction   TrVFun  ;
    
    /// The *fictive* argument for  "Vertex-Vertex bi-functions"
    typedef LoKi::Holder<LHCb::RecVertex,LHCb::RecVertex>        VertexPair ;
   
    // ========================================================================
    // the functors for two vertices 
    // ========================================================================
    
    /// the type of 'cuts'     for Track-Vertex pair  (interface)
    typedef LoKi::BasicFunctors<VertexPair>::Predicate              RVVCuts ;
    /// the type of 'function' for Track-Vertex pair  (interface)
    typedef LoKi::BasicFunctors<VertexPair>::Function               RVVFunc ;
    /// type of 'cuts' for Track-VertexPair        (assignable)
    typedef LoKi::BasicFunctors<VertexPair>::PredicateFromPredicate RVVCut  ;
    /// type of 'functions' for Track-Vertex-Pair  (assignable)
    typedef LoKi::BasicFunctors<VertexPair>::FunctionFromFunction   RVVFun  ;
    
  } // end of namespace LoKi::TrackTypes 
  // ==========================================================================
  namespace Types 
  {
    // regular functions/predicated for tracks: "Tr"
    
    /// type of 'cuts' for Tracks       (interface)
    typedef LoKi::TrackTypes::TrCuts              TrCuts  ;
    /// type of 'functions' for Tracks  (interface)
    typedef LoKi::TrackTypes::TrFunc              TrFunc  ;
    /// type of 'cuts' for Tracks       (assignable)
    typedef LoKi::TrackTypes::TrCut               TrCut   ;
    /// type of 'functions' for Tracks  (assignable)
    typedef LoKi::TrackTypes::TrFun               TrFun   ;
    
    // regular functions/predicated for Vertices: "RV"
    
    /// type of 'cuts' for Vertices       (interface)
    typedef LoKi::TrackTypes::RVCuts              RVCuts  ;
    /// type of 'functions' for Tracks  (interface)
    typedef LoKi::TrackTypes::RVFunc              RVFunc  ;
    /// type of 'cuts' for Tracks       (assignable)
    typedef LoKi::TrackTypes::RVCut               RVCut   ;
    /// type of 'functions' for Tracks  (assignable)
    typedef LoKi::TrackTypes::RVFun               RVFun   ;

    // the function for 2-tracks: "TTr"
    
    /// the type of 'cuts'     for Track-Vertex pair  (interface)
    typedef LoKi::TrackTypes::TTrCuts             TTrCuts   ;
    /// the type of 'function' for Track-Vertex pair  (interface)
    typedef LoKi::TrackTypes::TTrFunc             TTrFunc   ;
    /// type of 'cuts' for Track-VertexPair        (assignable)
    typedef LoKi::TrackTypes::TTrCut              TTrCut    ;
    /// type of 'functions' for Track-Vertex-Pair  (assignable)
    typedef LoKi::TrackTypes::TTrFun              TTrFun    ;
    
    // the functors for 2 vertices: "RVV"
    
    /// the type of 'cuts'     for Track-Vertex pair  (interface)
    typedef LoKi::TrackTypes::RVVCuts             RVVCuts   ;
    /// the type of 'function' for Track-Vertex pair  (interface)
    typedef LoKi::TrackTypes::RVVFunc             RVVFunc   ;
    /// type of 'cuts' for Track-VertexPair        (assignable)
    typedef LoKi::TrackTypes::RVVCut              RVVCut     ;
    /// type of 'functions' for Track-Vertex-Pair  (assignable)
    typedef LoKi::TrackTypes::RVVFun              RVVFun     ;

    // the functors for track&vertex: "TrV"
    
    /// the type of 'cuts'     for Track-Vertex pair  (interface)
    typedef LoKi::TrackTypes::TrVCuts             TrVCuts   ;
    /// the type of 'function' for Track-Vertex pair  (interface)
    typedef LoKi::TrackTypes::TrVFunc             TrVFunc   ;
    /// type of 'cuts' for Track-VertexPair        (assignable)
    typedef LoKi::TrackTypes::TrVCut              TrVCut    ;
    /// type of 'functions' for Track-Vertex-Pair  (assignable)
    typedef LoKi::TrackTypes::TrVFun              TrVFun    ;
    
  } // end of namespace LoKi::Types 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRACKTYPES_H
// ============================================================================
