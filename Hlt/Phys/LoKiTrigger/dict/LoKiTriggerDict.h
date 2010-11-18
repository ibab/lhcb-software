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
#include "LoKi/TrUpgrade.h"
#include "LoKi/TrMatch.h"
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
 *  By usage of this code one clearly states the disagreement
 *  with the campain of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 */

namespace LoKi
{

  namespace Dicts
  {

//     /** @class PipeOps
//      *  Wrapper class for operations with mapping functors
//      *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
//      *  @date   2007-11-30
//      */

//     template <>
//     class  PipeOps<const Hlt::Candidate*>
//     {
//     private:
//       // ======================================================================
//       typedef  const Hlt::Candidate* TYPE ;
//       //
//       typedef LoKi::BasicFunctors<TYPE>::Pipe          Pipe    ;
//       typedef LoKi::BasicFunctors<TYPE>::Map           Map     ;
//       typedef LoKi::BasicFunctors<TYPE>::Element       Element ;
//       typedef LoKi::BasicFunctors<TYPE>::FunVal        FunVal  ;
//       typedef LoKi::BasicFunctors<TYPE>::CutVal        CutVal  ;
//       typedef LoKi::BasicFunctors<TYPE>::Function     Func    ;
//       typedef LoKi::BasicFunctors<TYPE>::Predicate    Cuts    ;
//       // ======================================================================
//     public:
//       // ======================================================================
//       static  Pipe::result_type __call__
//       ( const Pipe& fun , Pipe::argument a ) { return fun ( a )  ; }
//       // ======================================================================
//     public:
//       // ======================================================================
//       // __rshift__
//        static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
//        __rshift__
//        ( const Pipe& fun , const Map&       fun2 )
//        { return fun >>                      fun2 ; }
//        // __rshift__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       __rshift__
//       ( const Pipe& fun , const Pipe&      fun2 )
//       { return fun >>                      fun2 ; }
//       // __rshift__
//        static LoKi::FunctorFromFunctor<std::vector<TYPE>,double>
//        __rshift__
//        ( const Pipe& fun , const FunVal&    fun2 )
//        { return fun >>                      fun2 ; }
//        // __rshift__
//        static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
//        __rshift__
//        ( const Pipe& fun , const CutVal&    fun2 )
//        { return fun >>                      fun2 ; }
//        // __rshift__
//        static LoKi::FunctorFromFunctor<std::vector<TYPE>,TYPE>
//        __rshift__
//        ( const Pipe& fun , const Element&   fun2 )
//        { return fun >>                      fun2 ; }
//       // __rshift__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       __rshift__
//       ( const Pipe& fun , const Cuts&      fun2 )
//       { return fun >> LoKi::filter<TYPE> ( fun2 ) ; }
//       //
//       // __rshift__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
//       __rshift__
//       ( const Pipe& fun , const Func&      fun2 )
//       { return fun >> LoKi::yields<TYPE> ( fun2 ) ; }
//       // ======================================================================

//       // __rshift__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       __rshift__
//       ( const Pipe& fun ,
//         const LoKi::Functor<const LHCb::L0MuonCandidate*,bool>&  fun2 )
//       { return fun >> fun2 ;  }



//       // __rshift__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       __rshift__
//       ( const Pipe& fun ,
//         const LoKi::Functor<const LHCb::L0CaloCandidate*,bool>&  fun2 )
//       { return fun >> fun2;  }

//       // __rshift__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       __rshift__
//       ( const Pipe& fun ,
//         const LoKi::Functor<const LHCb::Track*,bool>&  fun2 )
//       { return fun >> fun2 ;  }


//        // __rshift__
//        static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//        __rshift__
//        ( const Pipe& fun ,
//          const LoKi::Functor<const LHCb::RecVertex*,bool>&  fun2 )
//        { return fun >> fun2; }

//       // __rshift__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       __rshift__
//       ( const Pipe& fun ,
//         const LoKi::Functor<const Hlt::MultiTrack*,bool>&  fun2 )
//       { return fun >> fun2 ;  }

//       // __rshift__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       __rshift__
//       ( const Pipe& fun ,
//         const LoKi::Functor<const Hlt::Stage*,bool>&  fun2 )
//       { return fun >> fun2 ;  }


//       // ======================================================================
//     public:
//       // ======================================================================
//       // __rrshift__
//       static std::vector<TYPE>
//       __rrshift__ ( const Pipe& fun , const std::vector<TYPE>& val )
//       { return fun ( val ) ; }

//       // __rrshift__
//       static std::vector<TYPE>
//       __rrshift__ ( const Pipe& fun ,
//                     const std::vector<TYPE>::value_type& val )
//       { return fun ( std::vector<TYPE>( 1 , val ) ) ; }
//       // __rrshift__
//       static std::vector<TYPE>
//       __rrshift__ ( const Pipe&                                   fun ,
//                     const Gaudi::Range_<std::vector<TYPE> >&      val )
//       { return val >> fun ; }
//       // __rrshift__
//       static std::vector<TYPE>
//       __rrshift__ ( const Pipe&                                   fun ,
//                     const Gaudi::NamedRange_<std::vector<TYPE> >& val )
//       { return val >> fun ; }

//       // ======================================================================
//     public:

//       // ======================================================================
//       // __tee__
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       __tee__     ( const Pipe& fun )
//       { return LoKi::tee<TYPE>( fun ) ; }
//       // ======================================================================
//     public:

//       // ======================================================================
//       // _union_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       _union_          ( const Pipe& fun  , const Pipe& fun2 )
//       { return LoKi::Functors::Union         <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
//       // _union_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       _union_          ( const Pipe& fun  , const Cuts& fun2 )
//       { return LoKi::Functors::Union         <std::vector<TYPE>,TYPE>
//           ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
//       // _intersection_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       _intersection_   ( const Pipe& fun  , const Pipe& fun2 )
//       { return LoKi::Functors::Intersection  <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
//       // _intersection_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       _intersection_   ( const Pipe& fun  , const Cuts& fun2 )
//       { return LoKi::Functors::Intersection  <std::vector<TYPE>,TYPE>
//           ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }

//      // _difference_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       _difference_     ( const Pipe& fun  , const Pipe& fun2 )
//       { return LoKi::Functors::Difference    <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
//       // _difference_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       _difference_     ( const Pipe& fun  , const Cuts& fun2 )
//       { return LoKi::Functors::Difference    <std::vector<TYPE>,TYPE>
//           ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
//       // _sym_difference_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       _sym_difference_ ( const Pipe& fun  , const Pipe& fun2 )
//       { return LoKi::Functors::SymDifference <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
//       // _sym_difference_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
//       _sym_difference_ ( const Pipe& fun  , const Cuts& fun2 )
//       { return LoKi::Functors::SymDifference <std::vector<TYPE>,TYPE>
//           ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
//       // _includes_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
//       _includes_       ( const Pipe& fun  , const Pipe& fun2 )
//       { return LoKi::Functors::Includes <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
//       // _includes_
//       static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
//       _includes_       ( const Pipe& fun  , const Cuts& fun2 )
//       { return LoKi::Functors::Includes <std::vector<TYPE>,TYPE>
//           ( fun , LoKi::filter<TYPE>  ( fun2 ) ) ; }
//       // ======================================================================

//     };
// ========================================================================
}
}


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
    LoKi::Dicts::ElementOps <const Hlt::Candidate*>  m_fo5 ;
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
