// $Id: Hlt1Binders.h,v 1.3 2009-04-01 12:49:18 ibelyaev Exp $
// ============================================================================
#ifndef LOKITRIGGER_HLT1BINDERS_H 
#define LOKITRIGGER_HLT1BINDERS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <climits>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/apply.h"
#include "LoKi/HltBinders.h"
// ============================================================================
// BOOST
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace HltBinders 
  {
    // ========================================================================
    /** @struct BinOps 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-17
     */        
    template <class TYPE> struct BinOps  ;
    template <class TYPE> 
    struct BinOps<std::less<TYPE>    >
    { 
      BOOST_STATIC_ASSERT( std::numeric_limits<TYPE>::is_specialized ) ;
      /// the exceptional value 
      static TYPE value() { return std::numeric_limits<TYPE>::max() ; } 
      /// the name of the operation
      static std::string name() 
      {
        const static std::string s_name  = "Min" ; 
        return s_name ;
      }
    };
    // ========================================================================
    template <class TYPE> 
    struct BinOps<std::greater<TYPE> >
    { 
      BOOST_STATIC_ASSERT( std::numeric_limits<TYPE>::is_specialized ) ;
      /// the exceptional value 
      static TYPE value() { return 1 - std::numeric_limits<TYPE>::max() ; } 
      /// the name of the operation
      static std::string name() 
      {
        const static std::string s_name  = "Max" ; 
        return s_name ;
      }
    };
    // ========================================================================
    template <class TYPE> 
    struct  BinOps<LHCb::Math::abs_less<TYPE> >
    { 
      BOOST_STATIC_ASSERT( std::numeric_limits<TYPE>::is_specialized ) ;
      /// the exceptional value 
      static TYPE value() { return std::numeric_limits<TYPE>::max() ; } 
      /// the name of the operation
      static std::string name() 
      {
        const static std::string s_name  = "AbsMin" ; 
        return s_name ;
      }
    };
    // ========================================================================
    template <class TYPE> 
    struct  BinOps<LHCb::Math::abs_greater<TYPE> >
    { 
      BOOST_STATIC_ASSERT( std::numeric_limits<TYPE>::is_specialized ) ;
      /// the exceptional value 
      static TYPE value() { return 0 ; } 
      /// the name of the operation
      static std::string name() 
      {
        const static std::string s_name  = "AbsMax" ; 
        return s_name ;
      }
    };
    // ========================================================================
    /** @class BinderValueWithTheSource
     *  Simple generalization of class LoKi::HltBinders::BinderValue
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-17
     */
    template <class TYPE1                  ,
              class TYPE2                  ,
              class TYPE3=double           ,
              class BINOP=std::less<TYPE3> > 
    class BinderValueWithTheSource 
      : public LoKi::Functor<TYPE1,TYPE3>
      , public LoKi::HltBinders::HltBinder<TYPE1,TYPE2,TYPE3>  
    {
    protected:
      // ======================================================================
      typedef  typename LoKi::HltBinders::HltBinder<TYPE1,TYPE2,TYPE3> Binder ;
      /// the source of  
      typedef  typename LoKi::BasicFunctors<TYPE2*>::Source           Source_ ;
      typedef  typename LoKi::Assignable<Source_>::Type               Source2 ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor 
       *  @param fun     2-arguemnt function 
       *  @param source  the source 
       *  @param name    the functor name 
       */
      BinderValueWithTheSource 
      ( const typename Binder::Function_& fun    , //      2-argument function 
        const Source_&                    source , //               the source
        const std::string&                name   ) //         the functor name 
        : LoKi::Functor<TYPE1,TYPE3> ()
        , LoKi::HltBinders::HltBinder<TYPE1,TYPE2,TYPE3> ( fun ) 
        //
        , m_binop  ( BINOP () ) 
        , m_source ( source   ) 
        , m_name   ( name     ) 
      {}
      /// MANDATORY: virtual desctructor 
      virtual ~BinderValueWithTheSource () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BinderValueWithTheSource* clone () const 
      { return new BinderValueWithTheSource ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual typename  LoKi::Functor<TYPE1,TYPE3>::result_type operator() 
        ( typename  LoKi::Functor<TYPE1,TYPE3>::argument a ) const 
      {
        /// use the source 
        typename Source_::result_type value = m_source() ;
        /// setup the binder 
        this->setContainer ( &value ) ;
        /// use the binder:        
        typename Binder::Pair r = this->extremum ( a , m_binop ) ;
        Assert ( this->vct()-> end() != r.first ,"No valid elements are found" ) ;
        return r.second ;
      }
      // ======================================================================
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << m_name << "(" << this->fun() << "," << m_source << ")" ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the defautl constructo is disabled 
      BinderValueWithTheSource () ;   //     the defautl constructo is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// binary operation
      BINOP       m_binop  ;                         //        binary operation
      /// The source 
      Source2     m_source ;                         //              the source 
      /// function name 
      std::string m_name   ;                         // the actual functor name
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class BinderFunWithTheSource
     *  Simple generalization of class LoKi::HltBinders::BinderFun
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-17
     */
    template <class TYPE1                  ,
              class TYPE2                  ,
              class TYPE3=double           ,
              class BINOP=std::less<TYPE3> > 
    class BinderFunWithTheSource 
      : public LoKi::Functor<TYPE1,TYPE3>
      , public LoKi::HltBinders::HltBinder<TYPE1,TYPE2,TYPE3>  
    {
    protected:
      // ======================================================================
      typedef  typename LoKi::HltBinders::HltBinder<TYPE1,TYPE2,TYPE3> Binder ;
      /// the source of  
      typedef  typename LoKi::BasicFunctors<TYPE2*>::Source           Source_ ;
      typedef  typename LoKi::Assignable<Source_>::Type               Source2 ;
      typedef  typename LoKi::Functor<TYPE2,TYPE3>                    Fun2    ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor 
       *  @param fun     2-arguemnt function 
       *  @param source  the source 
       *  @param name    the functor name 
       */
      BinderFunWithTheSource 
      ( const typename Binder::Function_& fun    , //      2-argument function 
        const Source_&                    source , //               the source
        const Fun2&                       fun2   , //      the second function 
        const std::string&                name   ) //         the functor name 
        : LoKi::Functor<TYPE1,TYPE3> ()
        , LoKi::HltBinders::HltBinder<TYPE1,TYPE2,TYPE3> ( fun ) 
        //
        , m_binop  ( BINOP () ) 
        , m_source ( source   ) 
        , m_fun2   ( fun2     )
        , m_name   ( name     ) 
      {}
      /// MANDATORY: virtual desctructor 
      virtual ~BinderFunWithTheSource () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BinderFunWithTheSource* clone () const 
      { return new BinderFunWithTheSource ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual typename  LoKi::Functor<TYPE1,TYPE3>::result_type operator() 
        ( typename  LoKi::Functor<TYPE1,TYPE3>::argument a ) const 
      {
        /// use the source 
        typename Source_::result_type value = m_source() ;
        /// setup the binder 
        this->setContainer ( &value ) ;
        /// use the binder:        
        typename Binder::Pair r = this->extremum ( a , m_binop ) ;
        Assert ( this->vct()-> end() != r.first ,"No valid elements are found" ) ;
        //
        LoKi::Apply<TYPE2,TYPE3> applicator ( &m_fun2.func() ) ;
        return applicator ( *r.first ) ;
      }
      // ======================================================================
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << m_name << "(" << this-> fun() << "," 
                 << m_fun2 << "," << m_source << ")" ; }
      // ======================================================================
    public:
      // ======================================================================
      /// binary operation
      BINOP       m_binop  ;                         //        binary operation
      /// The source 
      Source2     m_source ;                         //              the source 
      /// the function to be evaluated 
      typename LoKi::Assignable<Fun2>::Type m_fun2 ; //            the function 
      /// function name 
      std::string m_name   ;                         // the actual functor name
      // ======================================================================      
    } ;
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types:;TrSources& source = ... ;
     *
     *    TrFun docaMin = bind 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              source              ,    //  the source 
     *              std::less<double>() ,    //  binary operation 
     *              "bindMin"           ) ;  //  needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3,class BINOP >
    inline 
    LoKi::HltBinders::BinderValueWithTheSource<TYPE1,TYPE2,TYPE3,BINOP>
    bind 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>&   fun      ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&     src      ,
      BINOP                                                /* binop */ ,
      const std::string&                                      name     )
    {
      return LoKi::HltBinders::BinderValueWithTheSource<TYPE1,TYPE2,TYPE3,BINOP> 
        ( fun , src , name ) ;  
    }    
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun pt = bind 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              source              ,    //  the source 
     *              TrPT                ,    //  the second function 
     *              std::less<double>() ,    //  binary operation 
     *              "bindMin"           ) ;  //  needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3,class BINOP >
    inline 
    LoKi::HltBinders::BinderFunWithTheSource<TYPE1,TYPE2,TYPE3,BINOP>
    bind 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>&   fun      ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&     src      ,
      const LoKi::Functor<TYPE2,TYPE3>&                       fun2     , 
      BINOP                                                /* binop */ ,
      const std::string&                                      name     )
    {
      return LoKi::HltBinders::BinderFunWithTheSource<TYPE1,TYPE2,TYPE3,BINOP> 
        ( fun , src , fun2 , name ) ;  
    }   
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun docaMin = bindMin 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              source              );   //  the source 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderValueWithTheSource<TYPE1,TYPE2,TYPE3,std::less<TYPE3> >
    bindMin 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun              ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&   src              ,
      const std::string&                                    name = "bindMin" )
    {      
      return LoKi::HltBinders::bind ( fun , src , std::less<TYPE3>() , name ) ;
    }    
    // ===========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun pt = bindMin 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              source              ,    //  the source 
     *              TrPT                ) ;  //  the second function 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderFunWithTheSource<TYPE1,TYPE2,TYPE3,std::less<TYPE3> >
    bindMin 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun              ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&   src              ,
      const LoKi::Functor<TYPE2,TYPE3>&                     fun2             , 
      const std::string&                                    name = "bindMin" )
    {      
      return LoKi::HltBinders::bind ( fun , src , fun2 , std::less<TYPE3>() , name ) ;
    }
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun docaAbsMin = bindAbsMin 
     *            ( TTrDOCA            ,    //  2-argument function 
     *              source             ) ;    //  the source 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderValueWithTheSource<TYPE1,TYPE2,TYPE3,
                                               LHCb::Math::abs_less<TYPE3> >
    bindAbsMin 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun              ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&   src              ,
      const std::string&                                 name = "bindAbsMin" )
    {      
      return LoKi::HltBinders::bind ( fun , src , 
                                      LHCb::Math::abs_less<TYPE3>() , name ) ;
    }    
    // ===========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun pt = bindAbsMin 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              source              ,    //  the source 
     *              TrPT                ) ;  //  the second function 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderFunWithTheSource<TYPE1,TYPE2,TYPE3,
                                             LHCb::Math::abs_less<TYPE3> >
    bindAbsMin 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun              ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&   src              ,
      const LoKi::Functor<TYPE2,TYPE3>&                     fun2             , 
      const std::string&                                 name = "bindAbsMin" )
    {      
      return LoKi::HltBinders::bind 
        ( fun , src , fun2 , LHCb::Math::abs_less<TYPE3>() , name ) ;
    }
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun docaMax = bindMax 
     *            ( TTrDOCA             ,  //  2-argument function 
     *              source              ); //  the source 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderValueWithTheSource<TYPE1,TYPE2,TYPE3,std::greater<TYPE3> >
    bindMax 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun              ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&   src              ,
      const std::string&                                    name = "bindMax" )
    {      
      return LoKi::HltBinders::bind 
        ( fun , src , std::greater<TYPE3>() , name ) ;
    }    
    // ===========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun pt = bindMax 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              source              ,    //  the source 
     *              TrPT                );   //  the second function 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderFunWithTheSource<TYPE1,TYPE2,TYPE3,std::greater<TYPE3> >
    bindMax 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun              ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&   src              ,
      const LoKi::Functor<TYPE2,TYPE3>&                     fun2             , 
      const std::string&                                    name = "bindMax" )
    {      
      return LoKi::HltBinders::bind 
        ( fun , src , fun2 , std::greater<TYPE3>() , name ) ;
    }    
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun docaMax = bindAbsMax 
     *            ( TTrDOCA             ,     //  2-argument function 
     *              source              );    //  the source 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderValueWithTheSource<TYPE1,TYPE2,TYPE3,
                                               LHCb::Math::abs_greater<TYPE3> >
    bindAbsMax 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun              ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&   src              ,
      const std::string&                                 name = "bindAbsMax" )
    {      
      return LoKi::HltBinders::bind 
        ( fun , src , LHCb::Math::abs_greater<TYPE3>() , name ) ;
    }    
    // ===========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    // the source 
     *    const LoKi::Types::TrSources& source = ... ;
     *
     *    TrFun pt = bindAbsMax 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              source              ,    //  the source 
     *              TrPT                ) ;  //  the second function 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@nikhef.nl
     *  @date 2008-11-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderFunWithTheSource<TYPE1,TYPE2,TYPE3,
                                             LHCb::Math::abs_greater<TYPE3> >
    bindAbsMax 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun              ,
      const typename LoKi::BasicFunctors<TYPE2*>::Source&   src              ,
      const LoKi::Functor<TYPE2,TYPE3>&                     fun2             , 
      const std::string&                                 name = "bindAbsMax" )
    {      
      return LoKi::HltBinders::bind 
        ( fun , src , fun2 , LHCb::Math::abs_greater<TYPE3>() , name ) ;
    }    
    // ========================================================================
  } // end of namespace LoKi::HltBinders 
  // ==========================================================================
} // end of namespadce LoKi 
// ============================================================================
// The END  
// ============================================================================
#endif // LOKITRIGGER_HLT1BINDERS_H
// ============================================================================
