// $Id: HltBindersDicts.h,v 1.2 2008-11-17 17:38:48 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLTBINDERSDICTS_H 
#define LOKI_HLTBINDERSDICTS_H 1
// ============================================================================
// Include Files 
// ============================================================================
#include "LoKi/TrackTypes.h"
// ============================================================================
/** @file 
 *  Collection of binders for Hlt-functions 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-08-19
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Dicts
  {
    // ========================================================================
    class HltBind
    {
    public:
      // ======================================================================
      // bindMin:  Track-Track
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindMin  
      ( const LoKi::Types::TTrFunc&      fun                    , 
        const std::vector<LHCb::Track*>* vct                    ,
        const std::string&               nam2 = ",_HLT_tracks"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindMin  
      ( const LoKi::Types::TTrFunc&      fun                    , 
        const LoKi::Types::TrFunc&       fun2                   , 
        const std::vector<LHCb::Track*>* vct                    ,
        const std::string&               nam2 = ",_HLT_tracks"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindAbsMin  
      ( const LoKi::Types::TTrFunc&      fun                    , 
        const std::vector<LHCb::Track*>* vct                    ,
        const std::string&               nam2 = ",_HLT_tracks"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindAbsMin  
      ( const LoKi::Types::TTrFunc&      fun                    , 
        const LoKi::Types::TrFunc&       fun2                   , 
        const std::vector<LHCb::Track*>* vct                    ,
        const std::string&               nam2 = ",_HLT_tracks"  ) ;
      // ======================================================================
      // bindMin Track-Track with source 
      static 
      LoKi::Types::TrFun 
      bindMin 
      ( const LoKi::Types::TTrFunc&   fun , 
        const LoKi::Types::TrSources& src ) ;
      // bindMin Track-Track with source 
      static 
      LoKi::Types::TrFun 
      bindMin 
      ( const LoKi::Types::TTrFunc&   fun  , 
        const LoKi::Types::TrSources& src  , 
        const LoKi::Types::TrFunc&    fun2 ) ;
      // bindMax Track-Track with source 
      static 
      LoKi::Types::TrFun 
      bindMax 
      ( const LoKi::Types::TTrFunc&   fun , 
        const LoKi::Types::TrSources& src ) ;
      // bindMax Track-Track with source 
      static 
      LoKi::Types::TrFun 
      bindMax
      ( const LoKi::Types::TTrFunc&   fun  , 
        const LoKi::Types::TrSources& src  , 
        const LoKi::Types::TrFunc&    fun2 ) ;
      // ======================================================================
      // bindMin Track-Track with source 
      static 
      LoKi::Types::TrFun 
      bindAbsMin 
      ( const LoKi::Types::TTrFunc&   fun , 
        const LoKi::Types::TrSources& src ) ;
      // bindMin Track-Track with source 
      static 
      LoKi::Types::TrFun 
      bindAbsMin 
      ( const LoKi::Types::TTrFunc&   fun  , 
        const LoKi::Types::TrSources& src  , 
        const LoKi::Types::TrFunc&    fun2 ) ;
      // bindMax Track-Track with source 
      static 
      LoKi::Types::TrFun 
      bindAbsMax 
      ( const LoKi::Types::TTrFunc&   fun , 
        const LoKi::Types::TrSources& src ) ;
      // bindMax Track-Track with source 
      static 
      LoKi::Types::TrFun 
      bindAbsMax
      ( const LoKi::Types::TTrFunc&   fun  , 
        const LoKi::Types::TrSources& src  , 
        const LoKi::Types::TrFunc&    fun2 ) ;
      // ======================================================================
      // bindMax : Track-Track
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindMax  
      ( const LoKi::Types::TTrFunc&      fun                    , 
        const std::vector<LHCb::Track*>* vct                    ,
        const std::string&               nam2 = ",_HLT_tracks"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindMax  
      ( const LoKi::Types::TTrFunc&      fun                    , 
        const LoKi::Types::TrFunc&       fun2                   , 
        const std::vector<LHCb::Track*>* vct                    ,
        const std::string&               nam2 = ",_HLT_tracks"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindAbsMax  
      ( const LoKi::Types::TTrFunc&      fun                    , 
        const std::vector<LHCb::Track*>* vct                    ,
        const std::string&               nam2 = ",_HLT_tracks"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindAbsMax  
      ( const LoKi::Types::TTrFunc&      fun                    , 
        const LoKi::Types::TrFunc&       fun2                   , 
        const std::vector<LHCb::Track*>* vct                    ,
        const std::string&               nam2 = ",_HLT_tracks"  ) ;
      // ======================================================================
      // bindMin:  Track-Vertex
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindMin  
      ( const LoKi::Types::TrVFunc&          fun                      , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindMin  
      ( const LoKi::Types::TrVFunc&          fun                      , 
        const LoKi::Types::RVFunc&           fun2                     , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindAbsMin  
      ( const LoKi::Types::TrVFunc&          fun                      , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindAbsMin  
      ( const LoKi::Types::TrVFunc&          fun                      , 
        const LoKi::Types::RVFunc&           fun2                     , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      // bindMax:  Track-Vertex
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindMax  
      ( const LoKi::Types::TrVFunc&          fun                      , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindMax  
      ( const LoKi::Types::TrVFunc&          fun                      , 
        const LoKi::Types::RVFunc&           fun2                     , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindAbsMax  
      ( const LoKi::Types::TrVFunc&          fun                      , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::TrFun 
      bindAbsMax  
      ( const LoKi::Types::TrVFunc&          fun                      , 
        const LoKi::Types::RVFunc&           fun2                     , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      // bindMin:  Vertex-Vertex
      // ======================================================================
      static 
      LoKi::Types::RVFun 
      bindMin  
      ( const LoKi::Types::RVVFunc&          fun                      , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::RVFun 
      bindMin  
      ( const LoKi::Types::RVVFunc&          fun                      , 
        const LoKi::Types::RVFunc&           fun2                     , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::RVFun 
      bindAbsMin  
      ( const LoKi::Types::RVVFunc&          fun                      , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::RVFun 
      bindAbsMin  
      ( const LoKi::Types::RVVFunc&          fun                      , 
        const LoKi::Types::RVFunc&           fun2                     , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      // bindMax:  Vertex-Vertex
      // ======================================================================
      static 
      LoKi::Types::RVFun 
      bindMax  
      ( const LoKi::Types::RVVFunc&          fun                      , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::RVFun 
      bindMax  
      ( const LoKi::Types::RVVFunc&          fun                      , 
        const LoKi::Types::RVFunc&           fun2                     , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::RVFun 
      bindAbsMax  
      ( const LoKi::Types::RVVFunc&          fun                      , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
      static 
      LoKi::Types::RVFun 
      bindAbsMax  
      ( const LoKi::Types::RVVFunc&          fun                      , 
        const LoKi::Types::RVFunc&           fun2                     , 
        const std::vector<LHCb::RecVertex*>* vct                      ,
        const std::string&                   nam2 = ",_HLT_vertices"  ) ;
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Dicts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTBINDERSDICTS_H
// ============================================================================
