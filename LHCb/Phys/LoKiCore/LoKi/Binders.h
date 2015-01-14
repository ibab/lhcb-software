// ============================================================================
#ifndef LOKI_BINDERS_H 
#define LOKI_BINDERS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BiFunctions.h"
// =========================================================l===================
/** @file 
 *  collection of various adapters for binding of the LoKi-functions
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
 *  @date 2007-08-17
 */
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi:Binders
   *  collection of various adapters for binding of the LoKi-functions
   *
   *  the concept has been inspired by generic binders from STL/Boost and 
   *  Est/EFunctions by Jose Angel Hernando Morata
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-17
   */
  namespace Binders 
  {
    // ========================================================================
    /** @class Binder1st
     *  helper class to bind the first argument of 2-argument LoKi-function
     *
     *  The concept has been inspired by generic binders from STL/Boost and 
     *  Est/EFunctions by Jose Angel Hernando Morata 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE1,class TYPE2, class TYPE3=double> 
    class Binder1st: public LoKi::Functor<TYPE2,TYPE3>
    {
    private:
      /// the actual type for data-holder 
      typedef typename LoKi::Holder<TYPE1,TYPE2>   Holder_ ;
      /// the actual type of
      typedef typename LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>  Fun_ ;
    public:
      /// contructor from the function and argument
      Binder1st 
      ( typename Fun_::first_argument_type value , 
        const    Fun_&                     fun   ) 
        : LoKi::AuxFunBase ( std::tie ( value , fun ) ) 
        , LoKi::Functor<TYPE2,TYPE3> () 
        , m_first ( value ) 
        , m_fun   ( fun   ) 
      {}
      /// contructor from the function and argument
      Binder1st 
      ( const    Fun_&                     fun   ,
        typename Fun_::first_argument_type value )
        : LoKi::AuxFunBase ( std::tie ( fun , value ) ) 
        , LoKi::Functor<TYPE2,TYPE3> () 
        , m_first ( value ) 
        , m_fun   ( fun   )  
      {}
      /// copy constructor 
      Binder1st ( const Binder1st& right ) 
        : LoKi::AuxFunBase      ( right ) 
        , LoKi::Functor<TYPE2,TYPE3> ( right ) 
        , m_first ( right.m_first ) 
        , m_fun   ( right.m_fun   )  
      {}
      /// MANDATORY: virtual destructor
      virtual ~Binder1st () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Binder1st* clone() const { return new Binder1st(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<TYPE2,TYPE3>::result_type 
      operator() ( typename LoKi::Functor<TYPE2,TYPE3>::argument a ) const 
      { return m_fun.fun ( Holder_ ( m_first , a ) ) ; }
      /// OPTIONAL :: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "Bind1st(" << m_fun << ")" ; }
    private:
      // ======================================================================
      // the default constructor is disabled 
      Binder1st() ; ///< the default constructor is disabled      
      // ======================================================================
    private:
      // ======================================================================
      // the fixed argument 
      typename Holder_::First m_first ;  ///< the fixed argument 
      /// the actual function 
      LoKi::FunctorFromFunctor<LoKi::Holder<TYPE1,TYPE2>,TYPE3> m_fun ; ///< function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Binder2nd
     *  helper class to bind the second argument of 2-argument LoKi-function
     *
     *  The concept has been inspired by generic binders from STL/Boost and 
     *  Est/EFunctions by Jose Angel Hernando Morata 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE1,class TYPE2,class TYPE3=double> 
    class Binder2nd: public LoKi::Functor<TYPE1,TYPE3>
    {
    private:
      // ======================================================================
      /// the actual type for data-holder 
      typedef typename LoKi::Holder<TYPE1,TYPE2>   Holder_ ;
      /// the actual type of
      typedef typename LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>  Fun_ ;
      // ======================================================================
    public:
      // ======================================================================
      /// contructor from the function and argument
      Binder2nd 
      ( typename Fun_::second_argument_type value , 
        const    Fun_&                      fun   ) 
        : LoKi::AuxFunBase ( std::tie ( value , fun ) ) 
        , LoKi::Functor<TYPE1,TYPE3> () 
        , m_second ( value ) 
        , m_fun    ( fun   ) 
      {}
      /// contructor from the function and argument
      Binder2nd 
      ( const    Fun_&                      fun   ,
        typename Fun_::second_argument_type value )
        : LoKi::AuxFunBase ( std::tie ( fun , value ) ) 
        , LoKi::Functor<TYPE1,TYPE3> () 
        , m_second ( value ) 
        , m_fun    ( fun   )  
      {}
      /// copy constructor 
      Binder2nd ( const Binder2nd& right ) 
        : LoKi::AuxFunBase      ( right ) 
        , LoKi::Functor<TYPE1,TYPE3> ( right ) 
        , m_second ( right.m_second ) 
        , m_fun    ( right.m_fun    )  
      {}
      /// MANDATORY: virtual destructor
      virtual ~Binder2nd () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Binder2nd* clone() const { return new Binder2nd(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<TYPE1,TYPE3>::result_type 
      operator() ( typename LoKi::Functor<TYPE1,TYPE3>::argument a ) const 
      { return m_fun.fun ( Holder_ ( a , m_second ) ) ; }
      /// OPTIONAL :: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "Bind2nd(" << m_fun << ")" ; }
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      Binder2nd() ; ///< the default constructor is disabled      
      // ======================================================================
    private:
      // ======================================================================
      // the fixed argument 
      typename Holder_::Second m_second ;  ///< the fixed argument 
      /// the actual function 
      LoKi::FunctorFromFunctor<LoKi::Holder<TYPE1,TYPE2>,TYPE3> m_fun ; 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Binders 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_BINDERS_H
// ============================================================================
