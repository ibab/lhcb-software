// $Id: Funcs.h,v 1.1 2007-06-01 11:35:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
   *  Helper namespace fopr buildingth eproper dictionries for LoKi
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-05-28
   */
  namespace Dicts
  {
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
      LoKi::Constant<TYPE>          m_003  ;
      LoKi::BooleanConstant<TYPE>   m_004  ;
      //
      LoKi::PredicateFromPredicate<TYPE> m_005  ;
      LoKi::FunctionFromFunction<TYPE>   m_006  ;
      //
      LoKi::And<TYPE>               m_3  ;
      LoKi::Or<TYPE>                m_4  ;
      LoKi::Not<TYPE>               m_5  ;
      LoKi::Less<TYPE>              m_6  ;
      LoKi::Equal<TYPE>             m_7  ;
      LoKi::LessOrEqual<TYPE>       m_8  ;
      LoKi::Plus<TYPE>              m_9  ;
      LoKi::Minus<TYPE>             m_10 ;
      LoKi::Multiply<TYPE>          m_11 ;
      LoKi::Divide<TYPE>            m_12 ;
      LoKi::Min<TYPE>               m_13 ;
      LoKi::Max<TYPE>               m_14 ;
      LoKi::ComposeFunction<TYPE>   m_15 ;
      LoKi::ComposeFunction2<TYPE>  m_16 ;
      LoKi::SimpleSwitch<TYPE>      m_18 ;
      LoKi::Switch<TYPE>            m_19 ;
      LoKi::Valid<TYPE>             m_20 ;
      LoKi::TheSame<TYPE>           m_21 ;
      LoKi::EqualToValue<TYPE>      m_22 ;
      LoKi::NotEqual<TYPE>          m_23 ;
      LoKi::NotEqualToValue<TYPE>   m_24 ;
      //
      Funcs () ; // undefined constructor 
    } ;
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCS_H
// ============================================================================
