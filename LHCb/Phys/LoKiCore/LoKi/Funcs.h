
// $Id: Funcs.h,v 1.8 2007-12-09 17:56:42 ibelyaev Exp $
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
#include "LoKi/BasicFunctors.h"
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
    /** @struct Funcs 
     *  The fictive structure used for creation of Reflex dictionaries 
     *  The structure is moved from Phys/LoKiDict package 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-05-27
     */ 
    template <class TYPE>
    struct Funcs
    {
      //
      typename LoKi::BasicFunctors<TYPE>::Constant               m_101 ;
      typename LoKi::BasicFunctors<TYPE>::BooleanConstant        m_102 ;
      typename LoKi::BasicFunctors<TYPE>::PredicateFromPredicate m_103 ;
      typename LoKi::BasicFunctors<TYPE>::FunctionFromFunction   m_104 ;
      
      LoKi::Valid<TYPE>                  m_301  ;      
      
      Funcs () ; // undefined constructor 
    } ;
    // ========================================================================
    /** @struct VFuncs 
     *  The fictive structure used for creation of Reflex dictionaries 
     *  The structure is moved from Phys/LoKiDict package 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-05-27
     */ 
    template <class TYPE>
    struct VFuncs
    {
      LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >   m_100 ;
      LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> > m_101 ;
      LoKi::FunctorFromFunctor<std::vector<TYPE>,TYPE>                 m_102 ;
      LoKi::FunctorFromFunctor<std::vector<TYPE>,double>               m_103 ;
      LoKi::FunctorFromFunctor<void,std::vector<TYPE> >                m_104 ;
      //
      VFuncs () ; // undefined constructor 
    } ;
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCS_H
// ============================================================================
