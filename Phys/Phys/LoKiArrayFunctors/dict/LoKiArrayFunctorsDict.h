// $Id: LoKiArrayFunctorsDict.h,v 1.1.1.1 2007-06-15 08:04:18 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiArrayFunctors.h"
// ============================================================================
#include "LoKi/Dicts.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    template <>
    class FunCalls<LoKi::ATypes::Combination> 
    {
    private:
      typedef LoKi::ATypes::Combination Type ;
      typedef LoKi::Function<Type>      Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type&           o ) { return fun ( o ) ; }
    } ;
    template <>
    class CutCalls<LoKi::ATypes::Combination> 
    {
    private:
      typedef LoKi::ATypes::Combination Type ;
      typedef LoKi::Predicate<Type>     Fun ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type&           o ) { return fun ( o ) ; }
    } ;
  } 
}
// ============================================================================
namespace
{
  struct _Instantiations 
  {
    // the basic functions 
    LoKi::Dicts::Funcs<LoKi::ATypes::Combination>          m_f1 ;
    // operators 
    LoKi::Dicts::FuncOps<LoKi::ATypes::Combination>        m_o1 ;
    /// mathematics:
    LoKi::Dicts::FunCalls<LoKi::ATypes::Combination>       m_c1 ;
    LoKi::Dicts::CutCalls<LoKi::ATypes::Combination>       m_c2 ;
    /// mathematics:
    LoKi::Dicts::Maths<LoKi::ATypes::Combination>          m_m1 ;   
    /// fictive constructor 
    _Instantiations () ;
  } ;  
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
