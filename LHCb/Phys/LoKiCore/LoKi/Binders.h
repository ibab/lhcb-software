// $Id: Binders.h,v 1.1 2007-08-18 14:10:56 ibelyaev Exp $
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
    template <class TYPE1,class TYPE2> 
    class Binder1st: public LoKi::Function<TYPE2>
    {
    private:
      /// the actual type for data-holder 
      typedef typename LoKi::Holder<TYPE1,TYPE2>   Holder_ ;
      /// the actual type of
      typedef typename LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >  Fun_ ;
    public:
      /// contructor from the function and argument
      Binder1st 
      ( typename Fun_::first_argument_type value , 
        const    Fun_&                     fun   ) 
        : LoKi::Function<TYPE2> () 
        , m_first ( value ) 
        , m_fun   ( fun   ) 
      {}
      /// contructor from the function and argument
      Binder1st 
      ( const    Fun_&                     fun   ,
        typename Fun_::first_argument_type value )
        : LoKi::Function<TYPE2> () 
        , m_first ( value ) 
        , m_fun   ( fun   )  
      {}
      /// copy constructor 
      Binder1st ( const Binder1st& right ) 
        : LoKi::AuxFunBase      ( right ) 
        , LoKi::Function<TYPE2> ( right ) 
        , m_first ( right.m_first ) 
        , m_fun   ( right.m_fun   )  
      {}
      /// MANDATORY: virtual destructor
      virtual ~Binder1st () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Binder1st* clone() const { return new Binder1st(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Function<TYPE2>::result_type 
      operator() ( typename LoKi::Function<TYPE2>::argument a ) const 
      { return m_fun ( Holder_( m_first , a ) ) ; }
      /// OPTIONAL :: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "Bind1st(" << m_fun << ")" ; }
    private:
      // the default constructor is disabled 
      Binder1st() ; ///< the default constructor is disabled      
    private:
      // the fixed argument 
      typename Holder_::First m_first ;  ///< the fixed argument 
      /// the actual function 
      LoKi::FunctionFromFunction<LoKi::Holder<TYPE1,TYPE2> > m_fun ; ///< function 
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
    template <class TYPE1,class TYPE2> 
    class Binder2nd: public LoKi::Function<TYPE1>
    {
    private:
      /// the actual type for data-holder 
      typedef typename LoKi::Holder<TYPE1,TYPE2>   Holder_ ;
      /// the actual type of
      typedef typename LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >  Fun_ ;
    public:
      /// contructor from the function and argument
      Binder2nd 
      ( typename Fun_::second_argument_type value , 
        const    Fun_&                      fun   ) 
        : LoKi::Function<TYPE1> () 
        , m_second ( value ) 
        , m_fun    ( fun   ) 
      {}
      /// contructor from the function and argument
      Binder2nd 
      ( const    Fun_&                      fun   ,
        typename Fun_::second_argument_type value )
        : LoKi::Function<TYPE1> () 
        , m_second ( value ) 
        , m_fun    ( fun   )  
      {}
      /// copy constructor 
      Binder2nd ( const Binder2nd& right ) 
        : LoKi::AuxFunBase      ( right ) 
        , LoKi::Function<TYPE1> ( right ) 
        , m_second ( right.m_second ) 
        , m_fun    ( right.m_fun    )  
      {}
      /// MANDATORY: virtual destructor
      virtual ~Binder2nd () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Binder2nd* clone() const { return new Binder2nd(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Function<TYPE1>::result_type 
      operator() ( typename LoKi::Function<TYPE1>::argument a ) const 
      { return m_fun ( Holder_( a , m_second ) ) ; }
      /// OPTIONAL :: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "Bind2nd(" << m_fun << ")" ; }
    private:
      // the default constructor is disabled 
      Binder2nd() ; ///< the default constructor is disabled      
    private:
      // the fixed argument 
      typename Holder_::Second m_second ;  ///< the fixed argument 
      /// the actual function 
      LoKi::FunctionFromFunction<LoKi::Holder<TYPE1,TYPE2> > m_fun ; ///< function 
    } ;
    // ========================================================================
  } // end of namespace LoKi::Binders 
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_BINDERS_H
// ============================================================================
