// $Id: LoKiTriggerDict.h,v 1.21 2010-06-06 08:33:35 ibelyaev Exp $
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
#include "LoKi/Tracks.h"
#include "LoKi/TrSources.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/LoKiTracks.h"
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
      ( const Fun& fun  , const LHCb::Track::Container&       o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
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
      ( const Fun& fun  , const Type::Container&            o ) 
      { 
        Type::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
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
    // the basic functions 
    LoKi::Dicts::Funcs<const LHCb::Track*>                    m_f1  ;
    //
    LoKi::Dicts::VFuncs<const LHCb::Track*>                   m_v1  ;
    // operators 
    LoKi::Dicts::FuncOps<const LHCb::Track*>                  m_o1  ;
    // operators 
    LoKi::Dicts::CutsOps<const LHCb::Track*>                  m_co1  ;
    // functionals 
    LoKi::Dicts::MapsOps   <const LHCb::Track*>               m_fo1  ;
    LoKi::Dicts::PipeOps   <const LHCb::Track*>               m_fo3  ;
    LoKi::Dicts::FunValOps <const LHCb::Track*>               m_fo5  ;
    LoKi::Dicts::ElementOps<const LHCb::Track*>               m_fo7  ;
    LoKi::Dicts::SourceOps <const LHCb::Track*>               m_fo9  ;
    LoKi::Dicts::CutValOps <const LHCb::Track*>               m_fo11 ;
    // infor 
    LoKi::Dicts::InfoOps   <const LHCb::Track*>               m_i1 ;
    // calls 
    LoKi::Dicts::FunCalls<LHCb::Track>                        m_cf1 ;
    LoKi::Dicts::CutCalls<LHCb::Track>                        m_cc1 ;
    /// trivia:
    LoKi::Functors::Empty <const LHCb::Track*>      m_e1 ;
    LoKi::Functors::Size  <const LHCb::Track*>      m_s1 ;
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
