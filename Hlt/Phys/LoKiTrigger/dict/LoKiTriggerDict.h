// $Id: LoKiTriggerDict.h,v 1.10 2008-11-17 17:38:49 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiTrigger.h"
#include "LoKi/TrackCuts.h"
#include "LoKi/HltCuts.h"
#include "LoKi/MoreFunctions.h"
// ============================================================================
#include "LoKi/Dicts.h"
// ============================================================================
#include "LoKi/ITrHybridTool.h"
#include "LoKi/TrHybridEngine.h"
#include "LoKi/InfoDicts.h"
#include "LoKi/HltBindersDicts.h"
// ============================================================================
#include "LoKi/Hlt1.h"
#include "LoKi/TrMatch.h"
#include "LoKi/TrUpgrade.h"
#include "LoKi/TrMatch.h"
#include "LoKi/VxMaker.h"
#include "LoKi/VxUpgrade.h"
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
      typedef LHCb::Track                         Type ;
      typedef LoKi::BasicFunctors<Type>::Function Fun  ;
    public:
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type&           o ) { return fun ( o ) ; }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( *o ) ; }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( *o.target() ) ; }
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
      ( const Fun& fun  , const Type&           o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( *o.target() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class FunCalls<LHCb::RecVertex>
    {
    private:
      typedef LHCb::RecVertex                      Type ;
      typedef LoKi::BasicFunctors <Type>::Function Fun  ;
    public:
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type&           o ) 
      { return fun ( o           ) ; }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) 
      { return fun ( *o.target() ) ; }
    public:
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::Vector&          o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::ConstVector&     o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::Container&       o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::RecVertex>&  o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ;
      }      
      // __rrshift__
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const Type&           o ) 
      { return fun ( o           ) ; }
      // __rrshift__
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) 
      { return fun ( *o.target() ) ; }
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class FunCalls<LoKi::TrackTypes::TrackPair>
    {
    private:
      typedef LoKi::TrackTypes::TrackPair         Type ;
      typedef LoKi::BasicFunctors<Type>::Function Fun  ;
    public:
      // __call__ 
      static Fun::result_type __call__ 
      ( const Fun& fun          , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    public:
      // __rrshift__ 
      static Fun::result_type __rrshift__ 
      ( const Fun& fun , Fun::argument  o ) { return fun ( o ) ; }
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class FunCalls<LoKi::TrackTypes::TrackVertexPair>
    {
    private:
      typedef LoKi::TrackTypes::TrackVertexPair   Type ;
      typedef LoKi::BasicFunctors<Type>::Function Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun          , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    public:
      // __rrshift__ 
      static Fun::result_type __rrshift__ 
      ( const Fun& fun , Fun::argument  o ) { return fun ( o ) ; }
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;    
    // ========================================================================
    template <>
    class FunCalls<LoKi::TrackTypes::VertexPair>
    {
    private:
      typedef LoKi::TrackTypes::VertexPair Type ;
      typedef LoKi::BasicFunctors<Type>::Function Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun          , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    public:
      // ======================================================================
      // __rrshift__ 
      static Fun::result_type __rrshift__ 
      ( const Fun& fun , Fun::argument  o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class CutCalls<LHCb::Track>
    {
    private:
      typedef LHCb::Track           Type ;
      typedef LoKi::BasicFunctors<Type>::Predicate Fun  ;
    public:
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        const Type&           o ) { return fun (  o           ) ;  }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        const Type*           o ) { return fun ( *o           ) ;  }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        const SmartRef<Type>& o ) { return fun ( *o.target()  ) ; }
    public:
      // ======================================================================
      // __rrshift__
      static Type::Vector __rrshift__ 
      ( const Fun& fun  , const Type::Vector&               o ) 
      { 
        Type::Vector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__
      static Type::Vector __rrshift__ 
      ( const Fun& fun  , const Type::ConstVector&          o ) 
      {
        /// yes! I know well that it is bad, but let me hope it is no so bad... 
        const Type::Vector& _o = reinterpret_cast<const Type::Vector&>  ( o ) ;
        Type::Vector res ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( _o.begin() , _o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__
      static Type::Vector __rrshift__ 
      ( const Fun& fun  , const Type::Container&            o ) 
      { 
        Type::Container& _o = const_cast<Type::Container&>( o ) ;
        Type::Vector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( _o.begin() , _o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }   
      // __rrshift__
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<Type>&       o ) 
      { 
        SmartRefVector<Type>& _o = const_cast<SmartRefVector<Type>&>( o ) ;
        Type::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( _o.begin() , _o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }      
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const Type&           o ) 
      { return fun (  o          ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) 
      { return fun ( *o.target() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      static LoKi::FunctorFromFunctor<Type,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) 
      { return fun >> o  ; }
    } ;
    // ========================================================================
    template <>
    class CutCalls<LHCb::RecVertex>
    {
    private:
      typedef LHCb::RecVertex       Type ;
      typedef LoKi::BasicFunctors<Type>::Predicate Fun  ;
    public:
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun , const Type&           o ) 
      { return fun ( o           ) ; }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun , const Type*           o ) 
      { return fun ( *o          ) ; }
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<Type>& o ) 
      { return fun ( *o.target() ) ; }
    public:      
      // ======================================================================
      // __rrshift__
      static Type::Vector __rrshift__ 
      ( const Fun& fun  , const Type::Vector&               o ) 
      { 
        Type::Vector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__
      static Type::Vector __rrshift__ 
      ( const Fun& fun  , const Type::ConstVector&          o ) 
      { 
        /// yes! I know well that it is bad, but let me hope it is no so bad... 
        const Type::Vector& _o = reinterpret_cast<const Type::Vector&>  ( o ) ;
        Type::Vector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( _o.begin() , _o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__
      static Type::Vector __rrshift__ 
      ( const Fun& fun  , const Type::Container&            o ) 
      { 
        Type::Container& _o = const_cast<Type::Container&>( o ) ;
        Type::Vector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( _o.begin() , _o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }   
      // __rrshift__
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<Type>&       o ) 
      { 
        SmartRefVector<Type>& _o = const_cast<SmartRefVector<Type>&>( o ) ;
        Type::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( _o.begin() , _o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }      
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const Type&           o ) 
      { return fun (  o          ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) 
      { return fun ( *o.target() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) 
      { return fun >> o  ; }
    } ;
    // ========================================================================
    template <>
    class CutCalls<LoKi::TrackTypes::TrackPair>
    {
    private:
      typedef LoKi::TrackTypes::TrackPair Type ;
      typedef LoKi::BasicFunctors<Type>::Predicate Fun  ;
    public:
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    public:
      // __rrshift__
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , Fun::argument a ) { return fun ( a ) ; }
    public:
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) 
      { return fun >> o  ; }      
    } ;
    // ========================================================================
    template <>
    class CutCalls<LoKi::TrackTypes::TrackVertexPair>
    {
    private:
      typedef LoKi::TrackTypes::TrackVertexPair Type ;
      typedef LoKi::BasicFunctors<Type>::Predicate Fun  ;
    public:
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    public:
      // __rrshift__
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , Fun::argument a ) { return fun ( a ) ; }
    public:
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) 
      { return fun >> o  ; }      
    } ;
    // ========================================================================
    template <>
    class CutCalls<LoKi::TrackTypes::VertexPair>
    {
    private:
      typedef LoKi::TrackTypes::VertexPair Type ;
      typedef LoKi::BasicFunctors<Type>::Predicate Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    public:
      // __rrshift__
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , Fun::argument a ) { return fun ( a ) ; }
    public:
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) 
      { return fun >> o  ; }      
    } ;
    // ========================================================================
  } // end of namespace LoKi::Dicts
  // ==========================================================================
} //end oif namespace LoKi
// ============================================================================
namespace
{
  struct _Instantiations 
  {
    // the basic functions 
    LoKi::Dicts::Funcs<LHCb::Track>                           m_f1  ;
    LoKi::Dicts::Funcs<LHCb::RecVertex>                       m_f2  ;
    LoKi::Dicts::Funcs<LoKi::TrackTypes::TrackPair>           m_f3  ;
    LoKi::Dicts::Funcs<LoKi::TrackTypes::TrackVertexPair>     m_f4  ;
    LoKi::Dicts::Funcs<LoKi::TrackTypes::VertexPair>          m_f5  ;
    //
    LoKi::Dicts::VFuncs<LHCb::Track*>                         m_v1  ;
    LoKi::Dicts::VFuncs<LHCb::RecVertex*>                     m_v2  ;
    // operators 
    LoKi::Dicts::FuncOps<LHCb::Track,LHCb::Track*>            m_o1  ;
    LoKi::Dicts::FuncOps<LHCb::RecVertex,LHCb::RecVertex*>    m_o2  ;
    LoKi::Dicts::FuncOps<LoKi::TrackTypes::TrackPair>         m_o3  ;
    LoKi::Dicts::FuncOps<LoKi::TrackTypes::TrackVertexPair>   m_o4  ;
    LoKi::Dicts::FuncOps<LoKi::TrackTypes::VertexPair>        m_o5  ;
    // operators 
    LoKi::Dicts::CutsOps<LHCb::Track,LHCb::Track*>            m_co1  ;
    LoKi::Dicts::CutsOps<LHCb::RecVertex,LHCb::RecVertex*>    m_co2  ;
    LoKi::Dicts::CutsOps<LoKi::TrackTypes::TrackPair>         m_co3  ;
    LoKi::Dicts::CutsOps<LoKi::TrackTypes::TrackVertexPair>   m_co4  ;
    LoKi::Dicts::CutsOps<LoKi::TrackTypes::VertexPair>        m_co5  ;
    // functionals 
    LoKi::Dicts::MapsOps<LHCb::Track*>                        m_fo1  ;
    LoKi::Dicts::MapsOps<LHCb::RecVertex*>                    m_fo2  ;    
    LoKi::Dicts::PipeOps<LHCb::Track*,LHCb::Track>            m_fo3  ;
    LoKi::Dicts::PipeOps<LHCb::RecVertex*,LHCb::RecVertex>    m_fo4  ;    
    LoKi::Dicts::FunValOps<LHCb::Track*>                      m_fo5  ;
    LoKi::Dicts::FunValOps<LHCb::RecVertex*>                  m_fo6  ;    
    LoKi::Dicts::ElementOps<LHCb::Track*,LHCb::Track>         m_fo7  ;
    LoKi::Dicts::ElementOps<LHCb::RecVertex*,LHCb::RecVertex> m_fo8  ;    
    LoKi::Dicts::SourceOps<LHCb::Track*,LHCb::Track>          m_fo9  ;
    LoKi::Dicts::SourceOps<LHCb::RecVertex*,LHCb::RecVertex>  m_fo10 ;    
    // infor 
    LoKi::Dicts::InfoOps<LHCb::Track>                         m_i1 ;
    LoKi::Dicts::InfoOps<LHCb::RecVertex>                     m_i2 ;
    // calls 
    LoKi::Dicts::FunCalls<LHCb::Track>                        m_cf1 ;
    LoKi::Dicts::FunCalls<LHCb::RecVertex>                    m_cf2 ;
    LoKi::Dicts::FunCalls<LoKi::TrackTypes::TrackPair>        m_cf3 ;
    LoKi::Dicts::FunCalls<LoKi::TrackTypes::TrackVertexPair>  m_cf4 ;
    LoKi::Dicts::FunCalls<LoKi::TrackTypes::VertexPair>       m_cf5 ;
    //
    LoKi::Dicts::CutCalls<LHCb::Track>                        m_cc1 ;
    LoKi::Dicts::CutCalls<LHCb::RecVertex>                    m_cc2 ;
    LoKi::Dicts::CutCalls<LoKi::TrackTypes::TrackPair>        m_cc3 ;
    LoKi::Dicts::CutCalls<LoKi::TrackTypes::TrackVertexPair>  m_cc4 ;
    LoKi::Dicts::CutCalls<LoKi::TrackTypes::VertexPair>       m_cc5 ;
    /// fictive constructor 
    _Instantiations () ;
  } ;  
} // end of anonymout namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
