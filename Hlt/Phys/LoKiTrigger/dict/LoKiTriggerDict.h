// $Id$
// ============================================================================
#ifndef LOKI_LOKITRIGGERDICT_H
#define LOKI_LOKITRIGGERDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// HtlBase
// ============================================================================
#include "Event/HltCandidate.h"
#include "Event/HltStage.h"
// ============================================================================
// HtlBase
// ============================================================================
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
#include "HltBase/IHltInspector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/HltTool.h"
// ============================================================================
#include "LoKi/TriggerTypes.h"
#include "LoKi/HltStages.h"
#include "LoKi/Hlt1.h"
#include "LoKi/HltL0.h"
#include "LoKi/HltCandidates.h"
// ============================================================================
#include "LoKi/RecVertices.h"
// ============================================================================
#include "LoKi/TrUpgrade.h"
#include "LoKi/TrMatch.h"
#include "LoKi/TrFilter.h"
#include "LoKi/VxMaker.h"
#include "LoKi/UpgradeVertices.h"
#include "LoKi/Hlt1Wrappers.h"
#include "LoKi/Hlt1Functions.h"
#include "LoKi/TrackCutAsRecVertexCut.h"
#include "LoKi/CacheFuncs.h"
// ============================================================================
#include "LoKi/Dicts.h"
#include "LoKi/FuncOps.h"
// ============================================================================
#include "LoKi/ITrgFunctorFactory.h"
#include "LoKi/ITrgFunctorAntiFactory.h"
#include "LoKi/TrgEngine.h"
// ============================================================================
/** @file
 *  Reflex dictionary for LoKiTrigger package
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
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$     
 */
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:191) // type qualifier is meaningless on cast type
#endif
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts
  {
    // ========================================================================
    /** @class PipeOps
     *  Wrapper class for operations with streamers functors
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-30
     */
    template <>
    class  PipeOps<const Hlt::Candidate*>
    {
    private:
      // ======================================================================
      typedef  const Hlt::Candidate* TYPE ;
      //
      typedef LoKi::BasicFunctors<TYPE>::Pipe          Pipe    ;
      typedef LoKi::BasicFunctors<TYPE>::Map           Map     ;
      typedef LoKi::BasicFunctors<TYPE>::FunVal        FunVal  ;
      typedef LoKi::BasicFunctors<TYPE>::CutVal        CutVal  ;
      typedef LoKi::BasicFunctors<TYPE>::Function     Func    ;
      typedef LoKi::BasicFunctors<TYPE>::Predicate    Cuts    ;
      // ======================================================================
    public:
      // ======================================================================
      static  Pipe::result_type __call__
      ( const Pipe& fun , Pipe::argument a ) { return fun ( a )  ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
      __rshift__
      ( const Pipe& fun , const Map&       fun2 )
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe& fun , const Pipe&      fun2 )
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,double>
      __rshift__
      ( const Pipe& fun , const FunVal&    fun2 )
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
      __rshift__
      ( const Pipe& fun , const CutVal&    fun2 )
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe& fun , const Cuts&      fun2 )
      { return fun >> LoKi::filter<TYPE> ( fun2 ) ; }
      //
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
      __rshift__
      ( const Pipe& fun , const Func&      fun2 )
      { return fun >> LoKi::yields<TYPE> ( fun2 ) ; }
      // ======================================================================
    public:  // gate & dump 
      // ======================================================================
      // __rshift__   : gate 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const Pipe& fun , const LoKi::Functor<void,bool>& gate )  
      { return fun >> LoKi::gate<TYPE> ( gate ) ; }
      // __rshift__   : dump 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const Pipe& fun , const LoKi::Dump& dump  ) 
      { return fun >> LoKi::Functors::Dump_<TYPE> ( dump ) ; }
      // ======================================================================
    public: // gate-2
      // ======================================================================      
      // __rrshift__  : gate 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rrshift__ 
      ( const Pipe& fun , const LoKi::Functor<void,bool>& gate )  
      { return LoKi::gate<TYPE> ( gate ) >> fun ; }      
      // __rmul__  : gate 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rmul__ 
      ( const Pipe& fun , const LoKi::Functor<void,bool>& gate )  
      { return LoKi::gate<TYPE> ( gate ) >> fun ; }      
      // ======================================================================
    public: // get the first N 
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const  Pipe& fun , const LoKi::FirstN& fn  ) 
      { return fun >> LoKi::Functors::FirstN_<TYPE> ( fn ) ; }
      // ======================================================================
    public : // reverse 
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const  Pipe& fun , const LoKi::Reverse& /* fn */ ) 
      { return fun >> LoKi::Functors::Reverse_<TYPE>() ; }
      // ======================================================================
    public: // timing
      // ======================================================================
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __timer__   ( const Pipe&              c , 
                    ChronoEntity*            t ) 
      { return LoKi::timer   ( c , t ) ; }
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __timer__   ( const Pipe&              c , 
                    IChronoSvc*              s , 
                    const std::string&       t )
      { return LoKi::timer   ( c , s , t  ) ; }
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __timer__   ( const Pipe&              c , 
                    const std::string&       t )
      { return LoKi::timer   ( c , t  ) ; } 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rmod__    ( const Pipe&              c , 
                    const LoKi::Timer&       t ) { return t % c  ; }
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rmod__    ( const Pipe&              c , 
                    ChronoEntity*            t ) { return t % c  ; }
      // ======================================================================
    public: // specific for HltCandidates
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe& fun ,
        const LoKi::Functor<const LHCb::L0MuonCandidate*,bool>&  fun2 )
      { return fun >> LoKi::Candidates::SlotFilter(fun2) ;  }
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe& fun ,
        const LoKi::Functor<const LHCb::L0CaloCandidate*,bool>&  fun2 )
      { return fun >> LoKi::Candidates::SlotFilter(fun2);  }
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe& fun ,
        const LoKi::Functor<const LHCb::Track*,bool>&  fun2 )
      { return fun >> LoKi::Candidates::SlotFilter(fun2) ;  }       
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe& fun ,
        const LoKi::Functor<const LHCb::VertexBase*,bool>&  fun2 )
      { return fun >> LoKi::Candidates::SlotFilter(fun2) ;  }       
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe& fun ,
        const LoKi::Functor<const Hlt::Stage*,bool>&  fun2 )
      { return fun >> LoKi::Candidates::SlotFilter(fun2) ;  }
      // __rshift__  for MultiTrack
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe& fun ,
        const LoKi::BasicFunctors<const LHCb::Track*>::CutVal&  fun2 )
      { return fun >> LoKi::Candidates::SlotFilter(fun2) ;  }
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
      __rshift__
      ( const Pipe& fun ,
        const LoKi::Functor<const LHCb::Track*,double>&  fun2 )
      { return fun >> LoKi::Candidates::SlotMap(fun2) ; }
      // __rshift__ for MultiTrack
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
      __rshift__
      ( const Pipe& fun ,
        const LoKi::BasicFunctors<const LHCb::Track*>::FunVal&  fun2 )
      { return fun >> LoKi::Candidates::SlotMap(fun2) ; }
      // __rshift__   "sink"
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__
      ( const Pipe&        fun       ,
        const std::string& selection )
      { return fun >> LoKi::Hlt1::Sink ( selection ) ; }
      // ======================================================================       
    public:
      // ======================================================================
      // __rrshift__
      static std::vector<TYPE>
      __rrshift__ ( const Pipe& fun , const std::vector<TYPE>& val )
      { return fun ( val ) ; }
      // __rrshift__
      static std::vector<TYPE>
      __rrshift__ ( const Pipe& fun ,
                    const std::vector<TYPE>::value_type& val )
      { return fun ( std::vector<TYPE>( 1 , val ) ) ; }
      // __rrshift__
      static std::vector<TYPE>
      __rrshift__ ( const Pipe&                                   fun ,
                    const Gaudi::Range_<std::vector<TYPE> >&      val )
      { return val >> fun ; }
      // __rrshift__
      static std::vector<TYPE>
      __rrshift__ ( const Pipe&                                   fun ,
                    const Gaudi::NamedRange_<std::vector<TYPE> >& val )
      { return val >> fun ; }
      // ======================================================================
    public:
      // ======================================================================
      // __tee__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __tee__     ( const Pipe& fun )
      { return LoKi::tee<TYPE>( fun ) ; }
      // ======================================================================
    public: 
      // ======================================================================
      // _union_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __union__          ( const Pipe& fun  , const Pipe& fun2 )
      { return LoKi::Functors::Union         <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _union_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __union__          ( const Pipe& fun  , const Cuts& fun2 )
      { return LoKi::Functors::Union         <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
      // _intersection_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __intersection__   ( const Pipe& fun  , const Pipe& fun2 )
      { return LoKi::Functors::Intersection  <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _intersection_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __intersection__   ( const Pipe& fun  , const Cuts& fun2 )
      { return LoKi::Functors::Intersection  <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
      // _difference_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __difference__     ( const Pipe& fun  , const Pipe& fun2 )
      { return LoKi::Functors::Difference    <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _difference_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __difference__     ( const Pipe& fun  , const Cuts& fun2 )
      { return LoKi::Functors::Difference    <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
      // _sym_difference_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __sym_difference__ ( const Pipe& fun  , const Pipe& fun2 )
      { return LoKi::Functors::SymDifference <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _sym_difference_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __sym_difference__ ( const Pipe& fun  , const Cuts& fun2 )
      { return LoKi::Functors::SymDifference <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
      // _includes_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
      __includes__       ( const Pipe& fun  , const Pipe& fun2 )
      { return LoKi::Functors::Includes <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _includes_
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
      __includes__       ( const Pipe& fun  , const Cuts& fun2 )
      { return LoKi::Functors::Includes <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE>  ( fun2 ) ) ; }
      // =====================================================================
    };
    // ========================================================================
    template <>
    class SourceOps<const Hlt::Candidate*,const Hlt::Candidate*>
    {
    private:
      // ======================================================================
      typedef const Hlt::Candidate* TYPE  ;
      typedef const Hlt::Candidate* TYPE2 ;
      // ======================================================================
    private:
      // ======================================================================
      typedef LoKi::BasicFunctors<TYPE>::Source        Source ;
      typedef LoKi::BasicFunctors<TYPE>::Pipe          Pipe    ;
      typedef LoKi::BasicFunctors<TYPE>::Map           Map     ;
      typedef LoKi::BasicFunctors<TYPE>::FunVal        FunVal  ;
      typedef LoKi::BasicFunctors<TYPE>::CutVal        CutVal  ;
      typedef LoKi::BasicFunctors<TYPE2>::Function     Func    ;
      typedef LoKi::BasicFunctors<TYPE2>::Predicate    Cuts    ;      
      // ======================================================================
    public:
      // ======================================================================
      // __call__
      static Source::result_type __call__ 
      ( const Source& fun ) { return fun() ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static Source::result_type& __rshift__ 
      ( const Source& fun , std::vector<TYPE>& res ) 
      { res = fun() ; return res ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source& fun , const Pipe&    fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<double> >
      __rshift__ 
      ( const Source& fun , const Map&     fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,double>
      __rshift__ 
      ( const Source& fun , const FunVal&  fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,bool>
      __rshift__ 
      ( const Source& fun , const CutVal&  fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source& fun , const Cuts&    fun2 ) 
      { return fun >> LoKi::filter<TYPE> ( fun2 ) ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<double> >
      __rshift__ 
      ( const Source& fun , const Func&    fun2 ) 
      { return fun >> LoKi::yields<TYPE> ( fun2 ) ; }
      // ======================================================================
    public:  // gate & dump 
      // ======================================================================
      // __rshift__ : gate
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source& fun , const LoKi::Functor<void,bool>& gate ) 
      { return fun >> LoKi::gate<TYPE> ( gate ) ; }
      // __rshift__ : dump 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source& fun , const LoKi::Dump& dump  ) 
      { return fun >> LoKi::Functors::Dump_<TYPE> ( dump ) ; }
      // ======================================================================
    public: // get the first N 
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const  Source& fun , const LoKi::FirstN& fn  ) 
      { return fun >> LoKi::Functors::FirstN_<TYPE> ( fn ) ; }
      // ======================================================================
    public : // reverse 
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const  Source& fun , const LoKi::Reverse& /* fn */ ) 
      { return fun >> LoKi::Functors::Reverse_<TYPE>() ; }
      // ======================================================================
    public: // cause 
      // ======================================================================
      // __rmul__ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rmul__  ( const Source& fun  , const LoKi::Functor<void,bool>& fun2 ) 
      { return LoKi::cause ( fun , fun2 ) ; }      
      // _start_if_
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __cause__ ( const Source& fun  , const LoKi::Functor<void,bool>& fun2 ) 
      { return LoKi::cause ( fun , fun2 ) ; }      
      // ======================================================================
    public: // timing
      // ======================================================================
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __timer__   ( const Source&            c , 
                    ChronoEntity*            t ) 
      { return LoKi::timer   ( c , t ) ; }
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __timer__   ( const Source&            c , 
                    IChronoSvc*              s , 
                    const std::string&       t )
      { return LoKi::timer   ( c , s , t  ) ; }
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __timer__   ( const Source&            c , 
                    const std::string&       t )
      { return LoKi::timer   ( c , t  ) ; } 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rmod__    ( const Source&            c , 
                    const LoKi::Timer&       t ) { return t % c  ; }
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rmod__    ( const Source&            c , 
                    ChronoEntity*            t ) { return t % c  ; }
      // ======================================================================
    public: // specific for Hlt::Candidate 
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source&                                          fun , 
        const LoKi::Functor<const LHCb::L0MuonCandidate*,bool>& cut )
      { return fun >> LoKi::Candidates::SlotFilter ( cut ) ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source&                                          fun , 
        const LoKi::Functor<const LHCb::L0CaloCandidate*,bool>& cut )
      { return fun >> LoKi::Candidates::SlotFilter ( cut ) ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source&                                          fun , 
        const LoKi::Functor<const LHCb::Track*,bool>&          cut )
      { return fun >> LoKi::Candidates::SlotFilter ( cut ) ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source&                                          fun , 
        const LoKi::Functor<const LHCb::VertexBase*,bool>&     cut )
      { return fun >> LoKi::Candidates::SlotFilter ( cut ) ; }
      // __rshift__ for MultiTracks
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source&                                          fun , 
        const LoKi::BasicFunctors<const LHCb::Track*>::CutVal& cut )
      { return fun >> LoKi::Candidates::SlotFilter ( cut ) ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<double> >
      __rshift__
      ( const Source& fun ,
        const LoKi::Functor<const LHCb::Track*,double>&  fun2 )
      { return fun >> LoKi::Candidates::SlotMap(fun2) ; }
      // __rshift__ for MultiTrack
      static LoKi::FunctorFromFunctor<void,std::vector<double> >
      __rshift__
      ( const Source& fun ,
        const LoKi::BasicFunctors<const LHCb::Track*>::FunVal&  fun2 )
      { return fun >> LoKi::Candidates::SlotMap(fun2) ; }
      // ===================================================================      
      // __rshift__  "sink"
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source&      fun      , 
        const std::string& selection )
      { return fun >> LoKi::Hlt1::Sink ( selection ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __union__ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __union__          ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::Union         <void,TYPE>( fun , fun2 ) ; }
      // __intersection__ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __intersection__   ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::Intersection  <void,TYPE>( fun , fun2 ) ; }
      // __difference__ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __difference__     ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::Difference    <void,TYPE>( fun , fun2 ) ; }
      // __sym_difference__ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __sym_difference__ ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::SymDifference <void,TYPE>( fun , fun2 ) ; }
      // __includes__ 
      static LoKi::FunctorFromFunctor<void,bool>
      __includes__       ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::Includes      <void,TYPE>( fun , fun2 ) ; }
      // ======================================================================
    }; 
    // ========================================================================
    template <>
    class InfoOps<const Hlt::Candidate*>
    {
    public:
      // ======================================================================
      typedef const Hlt::Candidate* TYPE ;
      // ======================================================================
    public:
      // ======================================================================
      // __logging__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __logging__ ( const LoKi::Functor<TYPE,double>& fun ) 
      { return LoKi::log_ ( fun , true ) ; }
      // __logging__
      static LoKi::FunctorFromFunctor<TYPE,double>
      __logging__ ( const LoKi::Functor<TYPE,double>& fun , 
                    const std::string&                key ) 
      { return LoKi::log_ ( fun , key ) ; }
      // ======================================================================
      // __logging__ 
      static LoKi::FunctorFromFunctor<TYPE,bool>
      __logging__ ( const LoKi::Functor<TYPE,bool>& fun ) 
      { return LoKi::log_ ( fun , true ) ; }
      // __logging__
      static LoKi::FunctorFromFunctor<TYPE,bool>
      __logging__ ( const LoKi::Functor<TYPE,bool>& fun ,
                    const std::string&              key )
      { return LoKi::log_ ( fun , key ) ; }
      // ======================================================================
    public: // info 
      // ======================================================================      
      // __info__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __info__ ( const LoKi::Functor<TYPE,double>& fun    , 
                 const std::string&                key    , 
                 const bool                        update ) 
      { return LoKi::cache_ ( fun , key , update  ) ; }
      // __info__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __info__ ( const LoKi::Functor<TYPE,double>& fun , 
                 const std::string&                key )
      { return LoKi::cache_ ( fun , key , true    ) ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class InfoOps<const Hlt::Stage*>
    {
    public:
      // ======================================================================
      typedef const Hlt::Stage* TYPE ;
      // ======================================================================
    public: // logging 
      // ======================================================================
      // __logging__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __logging__ ( const LoKi::Functor<TYPE,double>& fun ) 
      { return LoKi::log_ ( fun , true ) ; }
      // __logging__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __logging__ ( const LoKi::Functor<TYPE,double>& fun , 
                    const std::string&                key ) 
      { return LoKi::log_ ( fun , key ) ; }
      // ======================================================================
      // __logging__ 
      static LoKi::FunctorFromFunctor<TYPE,bool>
      __logging__ ( const LoKi::Functor<TYPE,bool>& fun )
      { return LoKi::log_ ( fun , true ) ; }
      // __logging__ 
      static LoKi::FunctorFromFunctor<TYPE,bool>
      __logging__ ( const LoKi::Functor<TYPE,bool>& fun , 
                    const std::string&              key )
      { return LoKi::log_ ( fun , key ) ; }
      // ======================================================================
    public: // info 
      // ======================================================================      
      // __info__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __info__ ( const LoKi::Functor<TYPE,double>& fun    , 
                 const std::string&                key    , 
                 const bool                        update ) 
      { return LoKi::cache_ ( fun , key , update  ) ; }
      // __info__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __info__ ( const LoKi::Functor<TYPE,double>& fun , 
                 const std::string&                key )
      { return LoKi::cache_ ( fun , key , true ) ; }
      // ======================================================================
    } ;
    // ========================================================================
  } //                                             end of namespace LoKi::Dicts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
namespace
{
  // ==========================================================================
  struct _Instantiations
  {
    // ========================================================================
    // Functors & Operations for Hlt::Candidates
    // ========================================================================
    // operators
    LoKi::Dicts::FuncOps    <const Hlt::Candidate*>  m_o1  ;
    LoKi::Dicts::CutsOps    <const Hlt::Candidate*>  m_o3  ;
    // functionals:
    LoKi::Dicts::MapsOps    <const Hlt::Candidate*>  m_fo1 ;
    LoKi::Dicts::PipeOps    <const Hlt::Candidate*>  m_fo2 ;
    LoKi::Dicts::FunValOps  <const Hlt::Candidate*>  m_fo3 ;
    LoKi::Dicts::CutValOps  <const Hlt::Candidate*>  m_fo4 ;
    LoKi::Dicts::SourceOps  <const Hlt::Candidate*>  m_fo6 ;
    // calls
    LoKi::Dicts::FunCalls   <Hlt::Candidate>         m_c1  ;
    LoKi::Dicts::CutCalls   <Hlt::Candidate>         m_c2  ;
    // the basic functions
    LoKi::Dicts::Funcs      <const Hlt::Candidate*>  m_f1 ;
    LoKi::Dicts::VFuncs     <const Hlt::Candidate*>  m_f3 ;
    // ========================================================================
    // Functors & Operations for Hlt::Stage
    // ========================================================================
    // operators
    LoKi::Dicts::FuncOps    <const Hlt::Stage*>  m_o1_  ;
    LoKi::Dicts::CutsOps    <const Hlt::Stage*>  m_o3_  ;
    // functionals: - no functionals here!
    // calls
    LoKi::Dicts::FunCalls   <Hlt::Stage>         m_c1_  ;
    LoKi::Dicts::CutCalls   <Hlt::Stage>         m_c2_  ;
    // the basic functions
    LoKi::Dicts::Funcs      <const Hlt::Stage*>  m_f1_  ;
    // ========================================================================
    // Logging & Info 
    // ========================================================================
    LoKi::Dicts::InfoOps <const Hlt::Candidate*> m_o10 ;
    LoKi::Dicts::InfoOps <const Hlt::Stage*>     m_o12 ;    
    // ========================================================================
    /// needed for various Hlt interfaces
    LoKi::Interface<Hlt::IRegister>          m_htl1 ;
    LoKi::Interface<Hlt::IData>              m_htl2 ;
    LoKi::Interface<Hlt::IInspector>         m_htl3 ;
    Hlt::IInspector::SelList                 m_hlt4 ;
    Hlt::IInspector::AlgList                 m_hlt5 ;
    /// fictive constructor
    _Instantiations () ;
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                 end of anonymous namespace
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_LOKITRIGGERDICT_H
// ============================================================================
