// $Id: TrackTypes.h,v 1.3 2007-12-11 18:56:21 ibelyaev Exp $
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
    typedef LoKi::BasicFunctors<LHCb::Track>::Predicate              TrCuts ;
    /// type of 'functions' for Tracks  (interface)
    typedef LoKi::BasicFunctors<LHCb::Track>::Function               TrFunc ;
    /// type of 'cuts' for Tracks       (assignable)
    typedef LoKi::BasicFunctors<LHCb::Track>::PredicateFromPredicate TrCut  ;
    /// type of 'functions' for Tracks  (assignable)
    typedef LoKi::BasicFunctors<LHCb::Track>::FunctionFromFunction   TrFun  ;
    
    /// type of 'map'       for Tracks    (interface)
    typedef LoKi::BasicFunctors<LHCb::Track*>::Map                   TrMaps ;
    /// type of 'map'       for Tracks    (assignable)
    typedef LoKi::Assignable<TrMaps>::Type                           TrMap  ;
    
    /// type of 'pipe'      for Tracks    (interface)
    typedef LoKi::BasicFunctors<LHCb::Track*>::Pipe                  TrPipes ;
    /// type of 'pipe'      for Tracks    (assignable)
    typedef LoKi::Assignable<TrPipes>::Type                          TrPipe  ;
    
    /// type of 'fun-val'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<LHCb::Track*>::FunVal                TrFunVals ;
    /// type of 'fun-val'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrFunVals>::Type                        TrFunVal  ;
    
    /// type of 'element'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<LHCb::Track*>::Element               TrElements ;
    /// type of 'element'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrElements>::Type                       TrElement  ;
    
    /// type of 'source'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<LHCb::Track*>::Source                TrSources ;
    /// type of 'source'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrSources>::Type                        TrSource  ;
    
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

    // functional part for Tracks :
    
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
    
    /// type for "elements" for Track  (interface)
    typedef LoKi::TrackTypes::TrElements TrElements ;
    /// type for "elements" for Track  (assignable)
    typedef LoKi::TrackTypes::TrElement  TrElement  ;
    
    /// type for "sources" for Track  (interface)
    typedef LoKi::TrackTypes::TrSources TrSources   ;
    /// type for "sources" for Track  (assignable)
    typedef LoKi::TrackTypes::TrSource  TrSource    ;

  } // end of namespace LoKi::Types 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRACKTYPES_H
// ============================================================================
