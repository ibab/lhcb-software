// $Id: LoKiTriggerDict.h,v 1.2 2007-08-13 13:36:54 ibelyaev Exp $
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
// ============================================================================
namespace LoKi
{
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
      typedef LHCb::Track                Type ;
      typedef LoKi::Function<const Type> Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type& o ) { return fun ( o ) ; }
    } ;
    // ========================================================================
    template <>
    class FunCalls<LHCb::RecVertex>
    {
    private:
      typedef LHCb::RecVertex             Type ;
      typedef LoKi::Function<const Type>  Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type& o ) { return fun ( o ) ; }
    } ;
    // ========================================================================
    template <>
    class FunCalls<LoKi::TrackTypes::TrackPair>
    {
    private:
      typedef LoKi::TrackTypes::TrackPair Type ;
      typedef LoKi::Function<Type>        Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun          , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    } ;
    // ========================================================================
    template <>
    class FunCalls<LoKi::TrackTypes::TrackVertexPair>
    {
    private:
      typedef LoKi::TrackTypes::TrackVertexPair Type ;
      typedef LoKi::Function<Type>              Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun          , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    } ;    
    // ========================================================================
    template <>
    class FunCalls<LoKi::TrackTypes::VertexPair>
    {
    private:
      typedef LoKi::TrackTypes::VertexPair Type ;
      typedef LoKi::Function<Type>         Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun          , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    } ;
    // ========================================================================
    template <>
    class CutCalls<LHCb::Track>
    {
    private:
      typedef LHCb::Track           Type ;
      typedef LoKi::Predicate<Type> Fun ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        const Type&           o ) { return fun ( o ) ; }
    } ;
    // ========================================================================
    template <>
    class CutCalls<LHCb::RecVertex>
    {
    private:
      typedef LHCb::RecVertex       Type ;
      typedef LoKi::Predicate<Type> Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun , const Type& o ) { return fun ( o ) ; }
    } ;
    // ========================================================================
    template <>
    class CutCalls<LoKi::TrackTypes::TrackPair>
    {
    private:
      typedef LoKi::TrackTypes::TrackPair Type ;
      typedef LoKi::Predicate<Type>       Fun ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    } ;
    // ========================================================================
    template <>
    class CutCalls<LoKi::TrackTypes::TrackVertexPair>
    {
    private:
      typedef LoKi::TrackTypes::TrackVertexPair Type ;
      typedef LoKi::Predicate<Type>             Fun ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
    } ;
    // ========================================================================
    template <>
    class CutCalls<LoKi::TrackTypes::VertexPair>
    {
    private:
      typedef LoKi::TrackTypes::VertexPair Type ;
      typedef LoKi::Predicate<Type>             Fun ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , 
        Fun::first_argument  a1 , 
        Fun::second_argument a2 ) { return fun ( a1 , a2 ) ; }
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
    // operators 
    LoKi::Dicts::FuncOps<LHCb::Track>                         m_o1  ;
    LoKi::Dicts::FuncOps<LHCb::RecVertex>                     m_o2  ;
    LoKi::Dicts::FuncOps<LoKi::TrackTypes::TrackPair>         m_o3  ;
    LoKi::Dicts::FuncOps<LoKi::TrackTypes::TrackVertexPair>   m_o4  ;
    LoKi::Dicts::FuncOps<LoKi::TrackTypes::VertexPair>        m_o5  ;
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
    /// 
    LoKi::Monitoring::Counter<LHCb::Track>                    m_mc1 ;
    LoKi::Monitoring::Plot<LHCb::Track>                       m_mp1 ;
    LoKi::Monitoring::Stat<LHCb::Track>                       m_ms1 ;
    LoKi::Monitoring::Counter<LHCb::RecVertex>                m_mc2 ;
    LoKi::Monitoring::Plot<LHCb::RecVertex>                   m_mp2 ;
    LoKi::Monitoring::Stat<LHCb::RecVertex>                   m_ms2 ;
    
    /// fictive constructor 
    _Instantiations () ;
  } ;  
} // end of anonymout namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
