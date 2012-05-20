// $Id$
// ============================================================================
#ifndef LOKI_LOKITRACKSDICT_H 
#define LOKI_LOKITRACKSDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Dicts.h"
#include "LoKi/TrackIDs.h"
#include "LoKi/VeloHitPatternFunctions.h"
#include "LoKi/Tracks.h"
#include "LoKi/TrSources.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/LoKiTracks.h"
// ============================================================================
#include "LoKi/ITrackFunctorFactory.h"
#include "LoKi/ITrackFunctorAntiFactory.h"
#include "LoKi/TrackEngine.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:177) //  variable ... was declared but never referenced
#pragma warning(disable:191) // type qualifier is meaningless on cast type
#endif
// ============================================================================
/** @file
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts 
  {
    // ========================================================================
    /** @class FunCalls Calls.h LoKi/Calls.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-29
     */
    template <>
    class FunCalls<LHCb::Track>
    {
    private:
      // ======================================================================
      typedef LHCb::Track                                       Type ;
      typedef LoKi::BasicFunctors<const LHCb::Track*>::Function Fun  ;
      // ======================================================================
    public:
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::Track::Vector&          o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::Track::ConstVector&     o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::Track::Range&            o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::Track::Container*       o ) 
      {
        std::vector<Fun::result_type> res  ;
        if ( 0 == o ) { return res ; }
        res.reserve ( o->size () ) ;
        LoKi::apply ( o->begin() , o->end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::Track::Selection*       o ) 
      {
        std::vector<Fun::result_type> res  ;
        if ( 0 == o ) { return res ; }
        res.reserve ( o->size () ) ;
        LoKi::apply ( o->begin() , o->end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Track>&  o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<const Type*,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<const Type*,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class CutCalls<LHCb::Track>
    {
    private:
      // ======================================================================
      typedef LHCb::Track                                        Type ;
      typedef LoKi::BasicFunctors<const LHCb::Track*>::Predicate Fun  ;
      // ======================================================================
    public:
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun&            fun ,  
        const Type*           o   ) { return fun ( o ) ;  }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun&            fun , 
        const SmartRef<Type>& o   ) { return fun ( o ) ; }
      //
      // __call__ as filter 
      // 
      // __call__
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const Type::ConstVector&          o )
      { return __rrshift__ ( fun , o ) ; }
      // __call__
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const Type::Range&                o ) 
      { return __rrshift__ ( fun , o ) ; }
      // __call__
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const Type::Container*            o ) 
      { return __rrshift__ ( fun , o ) ; }
      // __call__
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const Type::Selection*            o ) 
      { return __rrshift__ ( fun , o ) ; }
      // __call__
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const SmartRefVector<Type>&       o ) 
      { return __rrshift__ ( fun , o ) ; }
      // __call__ 
      static Type::Vector      __call__ 
      ( const Fun& fun  , const Type::Vector&               o ) 
      { return __rrshift__ ( fun , o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::ConstVector&               o ) 
      { 
        Type::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::Range&                     o ) 
      { 
        Type::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::Container*            o ) 
      { 
        Type::ConstVector res  ;
        if ( 0 == o ) { return res ; }
        res.reserve ( o->size () ) ;
        LoKi::apply_filter 
          ( o->begin() , o->end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }   
      // __rrshift__
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::Selection*            o ) 
      { 
        Type::ConstVector res  ;
        if ( 0 == o ) { return res ; }
        res.reserve ( o->size () ) ;
        LoKi::apply_filter 
          ( o->begin() , o->end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }   
      // __rrshift__
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<Type>&       o ) 
      { 
        Type::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      } 
      // __rrshift__ 
      static Type::Vector      __rrshift__ 
      ( const Fun& fun  , const Type::Vector&                   o ) 
      { 
        Type::Vector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }     
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      static LoKi::FunctorFromFunctor<const Type*,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) 
      { return fun >> o  ; }
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
    // Monitoring & Extra Info    
    LoKi::ExtraInfo::GetInfo<const LHCb::Track*>     m_53 ;
    LoKi::ExtraInfo::CheckInfo<const LHCb::Track*>   m_54 ;
    // the basic functions 
    LoKi::Dicts::Funcs     <const LHCb::Track*>               m_f1   ;
    LoKi::Dicts::VFuncs    <const LHCb::Track*>               m_v1   ;
    // operators 
    LoKi::Dicts::FuncOps   <const LHCb::Track*>               m_o1   ;
    LoKi::Dicts::CutsOps   <const LHCb::Track*>               m_co1  ;
    // functionals 
    LoKi::Dicts::MapsOps   <const LHCb::Track*>               m_fo1  ;
    LoKi::Dicts::PipeOps   <const LHCb::Track*>               m_fo3  ;
    LoKi::Dicts::FunValOps <const LHCb::Track*>               m_fo5  ;
    LoKi::Dicts::SourceOps <const LHCb::Track*>               m_fo9  ;
    LoKi::Dicts::CutValOps <const LHCb::Track*>               m_fo11 ;
    // infor 
    LoKi::Dicts::InfoOps   <const LHCb::Track*>               m_i1   ;
    // calls 
    LoKi::Dicts::FunCalls<LHCb::Track>                        m_cf1  ;
    LoKi::Dicts::CutCalls<LHCb::Track>                        m_cc1  ;
    /// trivia:
    LoKi::Functors::Empty <const LHCb::Track*>                m_e1   ;
    LoKi::Functors::Size  <const LHCb::Track*>                m_s1   ;
    /// fictive constructor 
    _Instantiations () ;
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_LOKITRACKSDICT_H
// ============================================================================
