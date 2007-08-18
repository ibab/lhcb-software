// $Id: HltBinders.h,v 1.1 2007-08-18 14:36:50 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLTBINDERS_H 
#define LOKI_HLTBINDERS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Binders.h"
#include "LoKi/Algs.h"
#include "LoKi/Objects.h"
#include "LoKi/StdMath.h"
// ============================================================================
namespace LoKi 
{
  namespace HltBinders
  {    
    // ========================================================================
    /** @class LigthBinder1st 
     *  "Ligth" (optimized) version of class LoKi::Binder1st
     *  The class is not suppose dto be used as LoKi-function, 
     *  it is just a useful intermediate object for efficient evaluations 
     *  
     *  @attention the class does not own the arguemt and the function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE1,class TYPE2>
    class LightBinder1st 
      : public std::unary_function
    <
      typename LoKi::Function<TYPE2>::argument_type ,
      typename LoKi::Function<TYPE2>::result_type   > 
    {
    private:
      /// the actual type for data-holder 
      typedef typename LoKi::Holder<TYPE1,TYPE2>   Holder_ ;
      /// the actual type of
      typedef typename LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >  Fun_ ;
    public:
      /// contructor from the function and argument
      LightBinder1st 
      ( typename Fun_::first_argument_type value , 
        const    Fun_&                     fun   ) 
        : m_first ( value ) 
        , m_fun   ( &fun   ) 
      {}
      /// contructor from the function and argument
      LightBinder1st 
      ( const    Fun_&                     fun   ,
        typename Fun_::first_argument_type value )
        : m_first (  value ) 
        , m_fun   ( &fun   )  
      {}
      /// evaluate the  function
      inline typename LoKi::Function<TYPE2>::result_type operator() 
      ( typename  LoKi::Function<TYPE2>::argument a ) const 
      { return (*m_fun) ( Holder_( m_first ,  a ) ) ; }
      /// evaluate the  function (HltSpecific) 
      inline typename LoKi::Function<TYPE2>::result_type operator() 
      ( const TYPE2* a ) const 
      { return (*m_fun) ( Holder_( m_first , *a ) ) ; }
    private:
      // the default constructor is disabled 
      LightBinder1st(); ///< the default constructor is disabled 
    private:
      // the fixed argument 
      typename Holder_::First                           m_first  ;  ///< the fixed argument 
      /// the actual function 
      const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >* m_fun    ; ///< function 
    } ;
    // ========================================================================
    /** @class LigthBinder2nd 
     *  "Ligth" (optimized) version of class LoKi::Binder2nd
     *  The class is not suppose dto be used as LoKi-function, 
     *  it is just a useful intermediate object for efficient evaluations 
     *  
     *  @attention the class does not own the argument and the function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE1,class TYPE2>
    class LightBinder2nd 
      : public std::unary_function
    <
      typename LoKi::Function<TYPE1>::argument_type ,
      typename LoKi::Function<TYPE1>::resul_type    > 
    {
    private:
      /// the actual type for data-holder 
      typedef typename LoKi::Holder<TYPE1,TYPE2>   Holder_ ;
      /// the actual type of
      typedef typename LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >  Fun_ ;
    public:
      /// contructor from the function and argument
      LightBinder2nd 
      ( typename Fun_::second_argument_type value , 
        const    Fun_&                      fun   ) 
        : m_second (  value ) 
        , m_fun    ( &fun   ) 
      {}
      /// contructor from the function and argument
      LightBinder2nd 
      ( const    Fun_&                      fun   ,
        typename Fun_::second_argument_type value )
        : m_second (  value ) 
        , m_fun    ( &fun   )  
      {}
      /// evaluate the  function
      inline typename LoKi::Function<TYPE1>::result_type operator() 
      ( typename  LoKi::Function<TYPE1>::argument a ) const 
      { return (*m_fun) ( Holder_( a , m_second ) ) ; }
      /// evaluate the  function (Hlt-specific) 
      inline typename LoKi::Function<TYPE1>::result_type operator() 
      ( const TYPE1* a ) const 
      { return (*m_fun) ( Holder_( *a , m_second ) ) ; }
    private:
      // the default constructor is disabled 
      LightBinder2nd (); ///< the default constructor is disabled 
    private:
      // the fixed argument 
      typename Holder_::Second                          m_second ;  ///< the fixed argument 
      /// the actual function 
      const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >* m_fun    ; ///< function 
    } ;
    // ========================================================================
    /** @class HltBinder
     *  helper class to organize "binders-for-Hlt"
     *
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE,class TYPE2>
    class HltBinder : public LoKi::AuxFunBase
    {
    public:
      typedef typename LoKi::Holder<TYPE,TYPE2>   Holder_    ;
      typedef typename LoKi::Function<Holder_>    Function_  ;
      typedef std::vector<TYPE2*>                 Container  ;
      typedef std::pair<typename Container::const_iterator,double> Pair ;
    public :
      /// constructor from thiie function and container:
      HltBinder
      ( const Function_&  fun , 
        const Container*  vct ) 
        : LoKi::AuxFunBase () 
        , m_fun ( fun ) 
        , m_vct ( vct ) 
      { 
        Assert ( 0 != m_vct , "Invalid container!" ) ; 
      }
      /// constructor from thiie function and container:
      HltBinder
      ( const Container*  vct ,
        const Function_&  fun )
        : LoKi::AuxFunBase () 
        , m_fun ( fun ) 
        , m_vct ( vct ) 
      { 
        Assert ( 0 != m_vct , "Invalid container!" ) ; 
      }
      // copy contructor
      HltBinder ( const HltBinder& right ) 
        : LoKi::AuxFunBase () 
        , m_fun ( right.m_fun ) 
        , m_vct ( right.m_vct ) 
      {}
      // destructor 
      virtual ~HltBinder() {}
    public:
      // minimize the function:
      inline Pair minimum 
      ( typename LoKi::Function<TYPE>::argument a ) const 
      { return this -> extremum ( a , std::less                  <double> () ) ; }
      // maximize the function:
      inline Pair maximum 
      ( typename LoKi::Function<TYPE>::argument a ) const 
      { return this -> extremum ( a , std::greater               <double> () ) ; }
      // minimize the absoluet value of the function:
      inline Pair minimum_abs 
      ( typename LoKi::Function<TYPE>::argument a ) const 
      { return this -> extremum ( a , LoKi::StdMath::abs_less    <double> () ) ; }
      // maximize the absoluet value of the function:
      inline Pair maximum_abs 
      ( typename LoKi::Function<TYPE>::argument a ) const 
      { return this -> extremum ( a , LoKi::StdMath::abs_greater <double> () ) ; }
    public:
      // find the extremal value:
      template <class BINOP>
      inline Pair extremum 
      ( typename LoKi::Function<TYPE>::argument a     , 
        BINOP                                   binop ) const 
      {
        return LoKi::Algs::extremum 
          ( m_vct -> begin () , 
            m_vct -> end   () , 
            LoKi::HltBinders::LightBinder1st<TYPE,TYPE2> ( m_fun , a )  , 
            LoKi::Objects::_VALID<TYPE2*> () , // only valid pointers!
            binop             ) ;
      }
    public:
      /// get access to function 
      const Function_&  fun () const { return m_fun ; }
      // get access to container 
      const Container*  vct () const { return m_vct ; }      
    private:
      // the default constructor is disabled  
      HltBinder() ; ///<  the default constructor is disabled
    private:
      // the actual function 
      typename LoKi::FunctionFromFunction<Holder_>  m_fun ; // the actual function 
      const Container*                              m_vct;
    } ;
    // ========================================================================
  } // end of namespace LoKi::HltBinders 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTBINDERS_H
// ============================================================================
