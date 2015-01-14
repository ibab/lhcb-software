// ============================================================================
#ifndef LOKI_FUNCCAST_H 
#define LOKI_FUNCCAST_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Functions.h"
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
 *  @date 2006-03-07 
 */
// ============================================================================
namespace LoKi 
{
  namespace Adapters
  {    
    // ========================================================================
    /** @class FuncCast FuncCast.h LoKi/FuncCast.h
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-07
     */
    template <class TYPE1,class TYPE2,class TYPE3=double>
    class FuncCast : public LoKi::Functor<TYPE1,TYPE3>
    {
    public: 
      // ======================================================================
      /// Standard constructor
      FuncCast ( const LoKi::Functor<TYPE2,TYPE3>& fun ) 
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , LoKi::Functor<TYPE1,TYPE3>()
        , m_fun ( fun ) 
      {} 
      /// copy constructor 
      FuncCast ( const FuncCast& right ) 
        : LoKi::AuxFunBase            ( right )
        , LoKi::Functor<TYPE1,TYPE3>  ( right ) 
        , m_fun ( right.m_fun ) 
      {} 
      /// MANDATORY: virtual destructor 
      virtual ~FuncCast() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  FuncCast* clone() const { return new FuncCast(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<TYPE1,TYPE3>::result_type 
      operator() ( typename LoKi::Functor<TYPE1,TYPE3>::argument a ) const
      {        
        typedef typename LoKi::Functor<TYPE2,TYPE3>::argument_type _arg2 ;
        return m_fun( dynamic_cast<_arg2>( a )  ) ;
      } 
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& stream ) const 
      { return stream << m_fun ; }
      /// OPTIONAL: helper method 
      typename LoKi::Functor<TYPE1,TYPE3>::result_type 
      operator() ( typename LoKi::Functor<TYPE2,TYPE3>::argument a ) const
      { return m_fun ( a ) ; } ;      
    private:
      // ======================================================================
      /// default constructor is disabled 
      FuncCast();
      // ======================================================================
    private:
      // ======================================================================
      LoKi::FunctorFromFunctor<TYPE2,TYPE3> m_fun ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class FuncStaticCast FuncCast.h LoKi/FuncCast.h
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-07
     */
    template <class TYPE1,class TYPE2, class TYPE3=double>
    class FuncStaticCast : public LoKi::Functor<TYPE1,TYPE3>
    {
    public: 
      // ======================================================================
      /// Standard constructor
      FuncStaticCast ( const LoKi::Functor<TYPE2,TYPE3>& fun ) 
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , LoKi::Functor<TYPE1,TYPE3>()
        , m_fun ( fun ) 
      {} 
      /// copy constructor 
      FuncStaticCast ( const FuncStaticCast& right ) 
        : LoKi::AuxFunBase           ( right )
        , LoKi::Functor<TYPE1,TYPE3> ( right ) 
        , m_fun ( right.m_fun ) 
      {} 
      /// MANDATORY: virtual destructor 
      virtual ~FuncStaticCast() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  FuncStaticCast* clone() const 
      { return new FuncStaticCast(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<TYPE1,TYPE3>::result_type 
      operator() ( typename LoKi::Functor<TYPE1,TYPE3>::argument a ) const
      {
        typedef typename LoKi::Functor<TYPE2,TYPE3>::argument_type _arg2 ;
        return m_fun.fun ( static_cast<_arg2>( a ) ) ;
      } ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& stream ) const 
      { return stream << m_fun ; }
      /// OPTIONAL: helper method 
      typename LoKi::Functor<TYPE2,TYPE3>::result_type 
      operator() ( typename LoKi::Functor<TYPE2,TYPE3>::argument a ) const
      { return m_fun.fun ( a ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      FuncStaticCast();
      // ======================================================================
    private:
      // ======================================================================
      LoKi::FunctorFromFunctor<TYPE2,TYPE3> m_fun ;
      // ======================================================================
    } ;
    // ========================================================================
  } // end of the namespace LoKi::Adapters
} // end of the namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCCAST_H
// ============================================================================
