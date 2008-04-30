// $Id: ExtCalls.h,v 1.5 2008-04-30 11:11:45 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_EXTCALLS_H 
#define LOKI_EXTCALLS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Operators.h"
#include "LoKi/Algo.h"
#include "LoKi/Loop.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts 
  {
    // ========================================================================
    /** @class ExtFunCalls ExtCalls.h LoKi/ExtCalls.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-30
     */
    // ========================================================================
    template <class TYPE> class ExtFunCalls ;
    // ========================================================================
    /** @class ExtCutCalls ExtCalls.h LoKi/ExtCalls.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-30
     */
    // ========================================================================
    template <class TYPE> class ExtCutCalls ;
    // ========================================================================
    template <>
    class ExtFunCalls<LHCb::Particle>
    {
    private:
      // ======================================================================
      typedef LHCb::Particle                             Type ;
      typedef LoKi::BasicFunctors<const Type*>::Function Fun  ;      
      // ======================================================================
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::Particle*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&               p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Particle>& p ) { return fun ( p ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const Type::ConstVector& o ) 
      { return o >> fun  ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const Type*              o ) 
      { return fun ( o )   ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const LoKi::Loop&        o ) 
      { return fun ( o ) ; }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const Type::Container& o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<Type>& o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
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
    class ExtCutCalls<LHCb::Particle>
    {
    private:
      // ======================================================================
      typedef LHCb::Particle                              Type ;
      typedef LoKi::BasicFunctors<const Type*>::Predicate Fun  ;      
      // ======================================================================
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::Particle*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&               p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Particle>& p ) { return fun ( p ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::ConstVector&          o ) { return o >> fun  ; }
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
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const Type*                       o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const LoKi::Loop&                 o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>&             o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<const Type*,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class ExtFunCalls<LHCb::VertexBase>
    {
    private:
      // ======================================================================
      typedef LHCb::VertexBase            Type ;
      typedef LoKi::BasicFunctors<const Type*>::Function Fun  ;      
      // ======================================================================
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::VertexBase*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&                 p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::VertexBase>& p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Vertex>&     p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::RecVertex>&  p ) { return fun ( p ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const Type::ConstVector& o ) 
      { return o >> fun  ; }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const Type::Container& o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<Type>& o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::Vertex::Container& o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Vertex>& o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::Container& o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::RecVertex>& o ) 
      {
        std::vector<Fun::result_type> res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const Type*              o ) 
      { return fun ( o )   ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const LoKi::Loop&        o ) 
      { return fun ( o ) ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>&    o ) { return fun ( o ) ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const SmartRef<LHCb::Vertex>& o ) { return fun ( o ) ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const SmartRef<LHCb::RecVertex>& o ) { return fun ( o ) ; }
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
    };
    // ========================================================================
    template <>
    class ExtCutCalls<LHCb::VertexBase>
    {
    private:
      typedef LHCb::VertexBase                            Type ;
      typedef LoKi::BasicFunctors<const Type*>::Predicate Fun  ;      
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::VertexBase*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&                 p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::VertexBase>& p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Vertex>&     p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::RecVertex>&  p ) { return fun ( p ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::ConstVector& o ) { return o >> fun  ; }
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
      static LHCb::Vertex::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::Vertex::Container&            o ) 
      { 
        LHCb::Vertex::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }   
      // __rrshift__
      static LHCb::Vertex::ConstVector __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Vertex>&       o ) 
      { 
        LHCb::Vertex::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__
      static LHCb::RecVertex::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::Container&            o ) 
      { 
        LHCb::RecVertex::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }   
      // __rrshift__
      static LHCb::RecVertex::ConstVector __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::RecVertex>&       o ) 
      { 
        LHCb::RecVertex::ConstVector res  ;
        res.reserve ( o.size () ) ;
        LoKi::apply_filter 
          ( o.begin() , o.end() , fun , std::back_inserter ( res ) ) ;
        return res ; 
      }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const Type*                       o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const LoKi::Loop&                 o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>&             o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<LHCb::Vertex>      o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<LHCb::RecVertex>&  o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__ 
      static LoKi::FunctorFromFunctor<const Type*,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================    
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXTCALLS_H
// ============================================================================
