
// $Id: Funcs.h,v 1.4 2007-08-27 12:27:52 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_FUNCS_H 
#define LOKI_FUNCS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Operators.h"
#include "LoKi/FuncCast.h"
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::Dicts
   *  Helper namespace for the building thee proper dictionries for LoKi
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-05-28
   */
  namespace Dicts
  {
    // ========================================================================
    /** @struct Funcs Funcs.h LoKi/Funcs.h
     *  The fictive structure used for creation of Reflex dictionaries 
     *  The structure is moved from Phys/LoKiDict package 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-05-27
     */ 
    template <class TYPE>
    struct Funcs
    {
      //
      LoKi::Constant<TYPE>               m_101  ;
      LoKi::BooleanConstant<TYPE>        m_102  ;
      LoKi::PredicateFromPredicate<TYPE> m_103  ;
      LoKi::FunctionFromFunction<TYPE>   m_104  ;
      //
      LoKi::Valid<TYPE>                  m_201  ;
      // LoKi::SimpleSwitch<TYPE>           m_202  ;
      // LoKi::Switch<TYPE>                 m_203  ;
      LoKi::TheSame<TYPE>                m_204  ;
      // LoKi::EqualToValue<TYPE>           m_205  ;
      // LoKi::Min<TYPE>                    m_301 ;
      // LoKi::Max<TYPE>                    m_302 ;
      // LoKi::And<TYPE>                    m_303 ;
      // LoKi::Or<TYPE>                     m_304 ;
      // LoKi::Not<TYPE>                    m_305 ;
      // LoKi::Less<TYPE>                   m_306 ;
      // LoKi::Equal<TYPE>                  m_307 ;
      // LoKi::LessOrEqual<TYPE>            m_308 ;
      // LoKi::Plus<TYPE>                   m_309 ;
      // LoKi::Minus<TYPE>                  m_310 ;
      // LoKi::Multiply<TYPE>               m_311 ;
      // LoKi::Divide<TYPE>                 m_312 ;
      // LoKi::ComposeFunction<TYPE>        m_313 ;
      // LoKi::ComposeFunction2<TYPE>       m_314 ;
      // LoKi::NotEqual<TYPE>               m_315 ;
      // LoKi::NotEqualToValue<TYPE>        m_316 ;
      //
      Funcs () ; // undefined constructor 
    } ;
    // ========================================================================
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCS_H
// ============================================================================
