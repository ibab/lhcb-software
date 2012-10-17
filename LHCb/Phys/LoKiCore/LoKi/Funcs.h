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
/** @file
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
 *  @date 2007-10-31 
 */
namespace LoKi 
{
  // ==========================================================================
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
      // ======================================================================
      typename LoKi::BasicFunctors<TYPE>::Constant               m_101 ;
      typename LoKi::BasicFunctors<TYPE>::BooleanConstant        m_102 ;
      typename LoKi::BasicFunctors<TYPE>::PredicateFromPredicate m_103 ;
      typename LoKi::BasicFunctors<TYPE>::FunctionFromFunction   m_104 ;
      // ======================================================================      
      LoKi::Valid<TYPE>                  m_301  ;      
      // ======================================================================      
      /// undefined constructor 
      Funcs () ; // undefined constructor 
      // ======================================================================
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
      // ======================================================================
      // "pipe"
      LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >   m_100 ;
      // "map"
      LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> > m_101 ;
      // "funval"
      LoKi::FunctorFromFunctor<std::vector<TYPE>,double>               m_103 ;
      // "cutval" 
      LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>                 m_104 ;
      // "source"
      LoKi::FunctorFromFunctor<void,std::vector<TYPE> >                m_105 ;
      // ======================================================================
      /// undefined constructor 
      VFuncs () ; // undefined constructor 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                             end of namespace LoKi::Dicts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_FUNCS_H
// ============================================================================
