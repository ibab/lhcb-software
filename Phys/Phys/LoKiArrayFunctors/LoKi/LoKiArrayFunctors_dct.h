// $Id:$
// ============================================================================
#ifndef LOKI_LOKIARRAY_DCT_H 
#define LOKI_LOKIARRAY_DCT_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MoreFunctions.h"
#include "LoKi/Dicts.h"
#include "LoKi/Operators.h"
#include "LoKi/FuncOps.h"
#include "LoKi/FuncOps.h"
// ============================================================================
#include "LoKi/ATypes.h"
#include "LoKi/AChild.h"
#include "LoKi/AKinematics.h"
#include "LoKi/AParticleCuts.h"
#include "LoKi/LoKiArrayFunctors.h"
// ============================================================================
#include "LoKi/IHybridTool.h"
#include "LoKi/HybridEngine.h"
#include "LoKi/DictTransform.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:177) //  variable ... was declared but never referenced
#pragma warning(disable:191) // type qualifier is meaningless on cast type
#endif
// ============================================================================
/** @file
 *  The dictionaries for the package Phys/LoKiArrayFunctors
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2007-12-01
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts 
  {
    // ========================================================================
    template <>
    class FunCalls<LoKi::ATypes::Combination> 
    {
    private:
      // ======================================================================
      typedef LoKi::ATypes::Combination           Type ;
      typedef LoKi::BasicFunctors<Type>::Function Fun  ;
      // ======================================================================
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type&            o ) { return fun ( o ) ; }
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type::Container& o ) 
      { return fun ( Type ( o.begin() , o.end() ) ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Particle>& o ) 
      {
        Type::Container c ( o.begin() , o.end() ) ;
        return fun ( Type ( c.begin() , c.end() ) ) ; 
      }      
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static Fun::result_type              
      __rrshift__ ( const Fun& fun  , const Type&              o  ) 
      { return o >> fun  ; }
      // __rrshift__ 
      static Fun::result_type
      __rrshift__ ( const Fun& fun  , const Type::Container&   o  ) 
      { return fun ( Type ( o.begin() , o.end() ) ) ; }
      // __rrshift__      
      //static std::vector<Fun::result_type> 
      //__rrshift__ ( const Fun& fun  , const std::vector<Type>& o  ) 
      //{ return o  >> fun  ; }      
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__ 
      static LoKi::FunctorFromFunctor<Type,double>
      __rshift__ ( const Fun& fun  , const LoKi::Functor<double,double>& o  ) 
      { return fun >> o   ; }      
      // ======================================================================
      // __rshift__ 
      static LoKi::FunctorFromFunctor<Type,bool>
      __rshift__ ( const Fun& fun  , const LoKi::Functor<double,bool>& o    ) 
      { return fun >> o   ; }      
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class CutCalls<LoKi::ATypes::Combination> 
    {
    private:
      // ======================================================================
      typedef LoKi::ATypes::Combination            Type ;
      typedef LoKi::BasicFunctors<Type>::Predicate Fun  ;
      // ======================================================================
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type&           o ) { return fun ( o ) ; }
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type::Container& o ) 
      { return fun ( Type ( o.begin() , o.end() ) ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Particle>& o ) 
      {
        Type::Container c ( o.begin() , o.end() ) ;
        return fun ( Type ( c.begin() , c.end() ) ) ; 
      }      
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static Fun::result_type              
      __rrshift__ ( const Fun& fun  , const Type&               o ) 
      { return o >> fun  ; }
      // __rrshift__ 
      static Fun::result_type              
      __rrshift__ ( const Fun& fun  , const Type::Container&    o ) 
      { return fun ( Type ( o.begin() , o.end() ) ) ; }
      // __rrshift__ 
      //static std::vector<Type>
      //__rrshift__ ( const Fun& fun  , const  std::vector<Type>& o ) 
      //{ return o >> fun  ; }      
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__ 
      static LoKi::FunctorFromFunctor<Type,bool>
      __rshift__ ( const Fun& fun  , const Fun& o ) 
      { return fun >> o   ; }      
      // ======================================================================
    } ;
    // ========================================================================
    /// Wrapper class for operations with functions 
    template <>
    class FuncOps<LoKi::ATypes::Combination> 
      : public FuncOps_<LoKi::ATypes::Combination>
    {};
    template <>
    class CutsOps<LoKi::ATypes::Combination> 
      : public CutsOps_<LoKi::ATypes::Combination>
    {};
    // ========================================================================
  } // end of namespace Dicts 
  // ==========================================================================
}
// ============================================================================
namespace
{
  // ==========================================================================
  struct _Instantiations 
  {
    // ========================================================================
    /// fictive constructor 
    _Instantiations () ;
    // ========================================================================
    // the basic functions 
    LoKi::Dicts::Funcs<LoKi::ATypes::Combination>          m_f1  ;
    LoKi::Dicts::VFuncs<LoKi::ATypes::Combination>         m_f2  ;
    // operators 
    LoKi::Dicts::FuncOps<LoKi::ATypes::Combination>        m_o1  ;
    LoKi::Dicts::CutsOps<LoKi::ATypes::Combination>        m_o2  ;
    
    //LoKi::Dicts::MapsOps<LoKi::ATypes::Combination>        m_o3  ;
    //LoKi::Dicts::PipeOps<LoKi::ATypes::Combination>        m_o4  ;
    //LoKi::Dicts::FunValOps<LoKi::ATypes::Combination>      m_o5  ;
    //LoKi::Dicts::CutValOps<LoKi::ATypes::Combination>      m_o51 ;
    //LoKi::Dicts::ElementOps<LoKi::ATypes::Combination>     m_o6  ;
    
    /// mathematics:
    LoKi::Dicts::FunCalls<LoKi::ATypes::Combination>       m_c1 ;
    LoKi::Dicts::CutCalls<LoKi::ATypes::Combination>       m_c2 ;
    // ========================================================================
    
    //     /// @todo to removed as sooas fPOOL is fixed for "const "/"std::" 
    //     LoKi::Range_<std::vector<LHCb::Particle*> > m_tmp1 ;
    //     std::unary_function<double,double>                 m_tmp2 ;
    //     std::unary_function<double,bool>                   m_tmp3 ;
    //     std::unary_function<LHCb::Particle*const,double>   m_tmp4 ;
    //     std::unary_function<LHCb::Particle*const,bool>     m_tmp5 ;
    //     std::unary_function<LHCb::VertexBase*const,double> m_tmp6 ;
    //     std::unary_function<LHCb::VertexBase*const,bool>   m_tmp7 ;
    //     std::unary_function<LoKi::Range_<std::vector<LHCb::Particle*>   > , double> m_tmp8  ;
    //     std::unary_function<LoKi::Range_<std::vector<LHCb::Particle*>   > , bool>   m_tmp81 ;
    //     std::unary_function<LoKi::Range_<std::vector<LHCb::VertexBase*> > , double> m_tmp9  ;
    //     std::unary_function<LoKi::Range_<std::vector<LHCb::VertexBase*> > , bool>   m_tmp91 ;
    //     std::unary_function<std::vector<LHCb::Particle*>   , LHCb::Particle*>   m_tmp10  ;
    //     std::unary_function<std::vector<LHCb::VertexBase*> , LHCb::VertexBase*> m_tmp101 ;
    //     std::unary_function<std::vector<LHCb::Particle*>  ,
    //                         std::vector<LHCb::Particle*> >   m_tmp11 ;
    //     std::unary_function<std::vector<LHCb::VertexBase*>,
    //                         std::vector<LHCb::VertexBase*> > m_tmp12 ;
    
    //     std::unary_function<std::vector<LHCb::Particle*>   , double >   m_tmp13  ;
    //     std::unary_function<std::vector<LHCb::Particle*>   , bool   >   m_tmp131  ;
    //     std::unary_function<std::vector<LHCb::VertexBase*> , double >   m_tmp14  ;
    //     std::unary_function<std::vector<LHCb::VertexBase*> , bool   >   m_tmp141  ;
    //     // dictionries, asked by Marco & Gerhard ..
    //     std::unary_function<const double,bool>                                               m_aux_1 ;
    //     std::unary_function<const double,double>                                             m_aux_2 ;
    //     std::unary_function<const LoKi::Range_<std::vector<const LHCb::Particle*> >,bool>    m_aux_3 ;
    //     std::unary_function<const LoKi::Range_<std::vector<const LHCb::Particle*> >,double>  m_aux_4 ;
    //
  } ;  
  // ==========================================================================
} // end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKIARRAY_DCT_H
// ============================================================================
