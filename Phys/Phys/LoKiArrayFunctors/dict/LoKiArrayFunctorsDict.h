// $Id: LoKiArrayFunctorsDict.h,v 1.9 2008-11-28 17:08:25 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKIARRAYDICT_H 
#define LOKI_LOKIARRAYDICT_H 1
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
#include "LoKi/LoKiArrayFunctors.h"
// ============================================================================
#include "LoKi/IHybridTool.h"
#include "LoKi/HybridEngine.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
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
      static std::vector<Fun::result_type> 
      __rrshift__ ( const Fun& fun  , const std::vector<Type>& o  ) 
      { return o  >> fun  ; }      
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
      static std::vector<Type>
      __rrshift__ ( const Fun& fun  , const  std::vector<Type>& o ) 
      { return o >> fun  ; }      
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
    LoKi::Dicts::MapsOps<LoKi::ATypes::Combination>        m_o3  ;
    LoKi::Dicts::PipeOps<LoKi::ATypes::Combination>        m_o4  ;
    LoKi::Dicts::FunValOps<LoKi::ATypes::Combination>      m_o5  ;
    LoKi::Dicts::CutValOps<LoKi::ATypes::Combination>      m_o51 ;
    LoKi::Dicts::ElementOps<LoKi::ATypes::Combination>     m_o6  ;
    /// mathematics:
    LoKi::Dicts::FunCalls<LoKi::ATypes::Combination>       m_c1 ;
    LoKi::Dicts::CutCalls<LoKi::ATypes::Combination>       m_c2 ;
    // ========================================================================
  } ;  
  // ==========================================================================
} // end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKIARRAYDICT_H
// ============================================================================
