// $Id: HltBinders.h,v 1.3 2007-08-20 09:39:45 ibelyaev Exp $
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
      // copy constructor
      HltBinder ( const HltBinder& right ) 
        : LoKi::AuxFunBase () 
        , m_fun ( right.m_fun ) 
        , m_vct ( right.m_vct ) 
      {}
      // destructor 
      virtual ~HltBinder() {}
    public:
      // find the extremal value with respect to the specified operation
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
            binop            ) ;
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
    /** @class BinderValue
     *  Auxillary class to represent various "binders"
     *  For the given argment, it funcd the exttream of "bi"-function
     *  with against all elements from the reference container and 
     *  returns the extremal value 
     *
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2, class BINOP = std::less<double> >
    class BinderValue : public LoKi::Function<TYPE1>
    {
    protected:
      typedef  typename LoKi::HltBinders::HltBinder<TYPE1,TYPE2> Binder ;
    public:
      /// constructor from the container and the function 
      BinderValue 
      ( const typename Binder::Function_& fun  , 
        const typename Binder::Container* vct  , 
        const std::string&                nam1 ,
        const std::string&                nam2 ) 
        : LoKi::Function<TYPE1> () 
        , m_binder ( fun , vct ) 
        , m_binop  ( BINOP ()  ) 
        , m_nam1   ( nam1     ) 
        , m_nam2   ( nam2     ) 
      {}
      /// constructor from the binder:
      BinderValue 
      ( const Binder&      binder , 
        const std::string& nam1   ,
        const std::string& nam2   ) 
        : LoKi::Function<TYPE1> () 
        , m_binder ( binder   ) 
        , m_binop  ( BINOP () ) 
        , m_nam1   ( nam1     ) 
        , m_nam2   ( nam2     ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~BinderValue () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BinderValue* clone () const { return new BinderValue(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Function<TYPE1>::result_type operator() 
      ( typename LoKi::Function<TYPE1>::argument a ) const 
      {
        typename Binder::Pair r = m_binder.extremum ( a , m_binop ) ;
        Assert ( m_binder.vct()-> end() != r.first ,"No valid tracks are found" ) ;
        return r.second ;
      }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << m_nam1 << fun() << m_nam2 ; }
    public:
      /// get access to the binder 
      const Binder& binder() const { return m_binder ; }
      /// cast to  the binder
      operator const Binder& () const { return binder() ; }
      // get access to binder function 
      const typename Binder::Function_& fun () const { return m_binder.fun() ; }   
    private:
      // the binder itself 
      Binder      m_binder ; /// the binder itself 
      // binary operation
      BINOP       m_binop  ; ///< binary operation
      // function name 
      std::string m_nam1   ;
      // function name 
      std::string m_nam2   ;
    };
    // ========================================================================
    /** @class BinderFun
     *  Auxillary class to represent various "binders"
     *  For the given argment, it funcd the exttream of "bi"-function
     *  with against all elements from the reference container, 
     *  and evaluates the second function for the found element,,
     *
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2, class BINOP>
    class BinderFun : public LoKi::Function<TYPE1>
    {
    protected:
      typedef  typename LoKi::HltBinders::HltBinder<TYPE1,TYPE2> Binder ;
      typedef  LoKi::Function<TYPE2>                    Fun2   ;
    public:
      /// constructor from the container and the functions 
      BinderFun
      ( const typename Binder::Function_& fun  ,
        const Fun2&                       fun2 ,
        const typename Binder::Container* vct  , 
        const std::string&                nam1 ,
        const std::string&                nam2 ) 
        : LoKi::Function<TYPE1> () 
        , m_binder ( fun , vct ) 
        , m_binop  ( BINOP ()  ) 
        , m_fun2   ( fun2      ) 
        , m_nam1   ( nam1     ) 
        , m_nam2   ( nam2     ) 
      {}
      /// constructor from the binder and function 
      BinderFun
      ( const Binder&      binder ,
        const Fun2&        fun2   ,
        const std::string& nam1   ,
        const std::string& nam2   ) 
        : LoKi::Function<TYPE1> () 
        , m_binder ( binder    ) 
        , m_binop  ( BINOP ()  ) 
        , m_fun2   ( fun2      ) 
        , m_nam1   ( nam1     ) 
        , m_nam2   ( nam2     ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~BinderFun () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BinderFun* clone () const { return new BinderFun(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Function<TYPE1>::result_type operator() 
      ( typename LoKi::Function<TYPE1>::argument a ) const 
      {
        typename Binder::Pair r = m_binder.extremum ( a , m_binop ) ;
        Assert ( m_binder.vct()-> end() != r.first , "No valid elements are found" ) ;
        return m_fun2 ( **(r.first) ) ;
      }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << m_nam1 << fun() << "," << fun2() << m_nam2 ; }
    public:
      /// cast to  the binder
      operator const Binder&          () const { return binder()       ; }
      /// get access to the binder 
      const Binder&            binder () const { return m_binder       ; }
      // get access to binder function 
      const typename Binder::Function_& fun () const { return m_binder.fun() ; }   
      // get access to own function 
      const Fun2&              fun2   () const { return m_fun2         ; }   
    private:
      // the binder itself 
      Binder m_binder ; /// the binder itself 
      // binary operation
      BINOP  m_binop  ; ///< binary operation
      // the function to be evaluated 
      LoKi::FunctionFromFunction<TYPE2>  m_fun2 ; /// the function to be evaluated 
      // function name 
      std::string m_nam1   ;
      // function name 
      std::string m_nam2   ;
    };
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bind 
     *            ( TTrDOCA ,                ///< 2-argument function 
     *              tracks  ,                ///< the container
     *              std::less<double>() ,    ///< binary operation 
     *              "bindMin("          ,    ///< needef for ptintout/ID 
     *              "_HLT_tracks)"      )    ///< needef for ptintout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2,class BINOP>
    inline 
    LoKi::HltBinders::BinderValue<TYPE1,TYPE2,BINOP>
    bind 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const std::vector<TYPE2*>*                        vct   , 
      BINOP                                             binop ,
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::BinderValue<TYPE1,TYPE2,BINOP> 
        ( fun , vct , nam1 , nam2 ) ;  
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun key = bind 
     *            ( TTrDOCA ,                ///< 2-argument function 
     *              TrKEY   ,                ///< the function
     *              tracks  ,                ///< the container
     *              std::less<double>() ,    ///< binary operation 
     *              "bindMin("          ,    ///< needef for ptintout/ID 
     *              "_HLT_tracks)"      )    ///< needef for ptintout/ID 
     *
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2,class BINOP>
    inline 
    LoKi::HltBinders::BinderFun<TYPE1,TYPE2,BINOP>
    bind 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const LoKi::Function<TYPE2>&                      fun2  , 
      const std::vector<TYPE2*>*                        vct   ,
      BINOP                                             binop ,
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::BinderFun<TYPE1,TYPE2,BINOP>
        ( fun , fun2 , vct , nam1 , nam2 ) ;
    }  
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bindMin 
     *            ( TTrDOCA             ,    ///< 2-argument function 
     *              tracks              ,    ///< the container
     *              "bindMin("          ,    ///< needed for ptintout/ID 
     *              "_HLT_tracks)"      )    ///< needed for ptintout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2>
    inline 
    LoKi::HltBinders::BinderValue<TYPE1,TYPE2,std::less<double> >
    bindMin 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const std::vector<TYPE2*>*                        vct   , 
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::bind 
        ( fun , vct , std::less<double>() , nam1 , nam2 ) ;
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun key = bindMin
     *            ( TTrDOCA             , ///< 2-argument function 
     *              TrKEY               , ///< the function
     *              tracks              , ///< the container
     *              "bindMin("          , ///< needed for ptintout/ID 
     *              "_HLT_tracks)"      ) ///< needed for ptintout/ID 
     * 
     *  @endcode 
     * 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2>
    inline 
    LoKi::HltBinders::BinderFun<TYPE1,TYPE2,std::less<double> >
    bindMin 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const LoKi::Function<TYPE2>&                      fun2  , 
      const std::vector<TYPE2*>*                        vct   ,
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::bind 
        ( fun , fun2 , vct , std::less<double>() , nam1 , nam2 ) ;
    }      
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bindAbsMin 
     *            ( TTrDOCA             ,    ///< 2-argument function 
     *              tracks              ,    ///< the container
     *              "bindAbsMin("       ,    ///< needed for printout/ID 
     *              "_HLT_tracks)"      )    ///< needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2>
    inline 
    LoKi::HltBinders::BinderValue
    <TYPE1,TYPE2,LoKi::StdMath::abs_less<double> >
    bindAbsMin 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const std::vector<TYPE2*>*                        vct   , 
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::bind 
        ( fun , vct , LoKi::StdMath::abs_less<double>() , nam1 , nam2 ) ;
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun key = bindAbsMin
     *            ( TTrDOCA             , ///< 2-argument function 
     *              TrKEY               , ///< the function
     *              tracks              , ///< the container
     *              "bindAbsMin("       , ///< needed for ptintout/ID 
     *              "_HLT_tracks)"      ) ///< needed for ptintout/ID 
     * 
     *  @endcode 
     * 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2>
    inline 
    LoKi::HltBinders::BinderFun
    <TYPE1,TYPE2,LoKi::StdMath::abs_less<double> >
    bindAbsMin 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const LoKi::Function<TYPE2>&                      fun2  , 
      const std::vector<TYPE2*>*                        vct   ,
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::bind 
        ( fun , fun2 , vct , LoKi::StdMath::abs_less<double>() , nam1 , nam2 ) ;
    }      
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bindMax 
     *            ( TTrDOCA             ,    ///< 2-argument function 
     *              tracks              ,    ///< the container
     *              "bindMax("          ,    ///< needed for ptintout/ID 
     *              "_HLT_tracks)"      )    ///< needed for ptintout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2>
    inline 
    LoKi::HltBinders::BinderValue
    <TYPE1,TYPE2,std::greater<double> >
    bindMax 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const std::vector<TYPE2*>*                        vct   , 
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::bind 
        ( fun , vct , std::greater<double>() , nam1 , nam2 ) ;
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun key = bindMax
     *            ( TTrDOCA             , ///< 2-argument function 
     *              TrKEY               , ///< the function
     *              tracks              , ///< the container
     *              "bindMax("          , ///< needed for ptintout/ID 
     *              "_HLT_tracks)"      ) ///< needed for ptintout/ID 
     * 
     *  @endcode 
     * 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2>
    inline 
    LoKi::HltBinders::BinderFun
    <TYPE1,TYPE2,std::greater<double> >
    bindMax 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const LoKi::Function<TYPE2>&                      fun2  , 
      const std::vector<TYPE2*>*                        vct   ,
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::bind 
        ( fun , fun2 , vct , std::greater<double>() , nam1 , nam2 ) ;
    }      
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bindAbsMax 
     *            ( TTrDOCA             ,    ///< 2-argument function 
     *              tracks              ,    ///< the container
     *              "bindAbsMax("       ,    ///< needed for printout/ID 
     *              "_HLT_tracks)"      )    ///< needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2>
    inline 
    LoKi::HltBinders::BinderValue
    <TYPE1,TYPE2,LoKi::StdMath::abs_greater<double> >
    bindAbsMax 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const std::vector<TYPE2*>*                        vct   , 
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::bind 
        ( fun , vct , LoKi::StdMath::abs_greater<double>() , nam1 , nam2 ) ;
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun key = bindAbsMax
     *            ( TTrDOCA             , ///< 2-argument function 
     *              TrKEY               , ///< the function
     *              tracks              , ///< the container
     *              "bindAbsMax("       , ///< needed for ptintout/ID 
     *              "_HLT_tracks)"      ) ///< needed for ptintout/ID 
     * 
     *  @endcode 
     * 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2>
    inline 
    LoKi::HltBinders::BinderFun
    <TYPE1,TYPE2,LoKi::StdMath::abs_greater<double> >
    bindAbsMax 
    ( const LoKi::Function<LoKi::Holder<TYPE1,TYPE2> >& fun   ,
      const LoKi::Function<TYPE2>&                      fun2  , 
      const std::vector<TYPE2*>*                        vct   ,
      const std::string&                                nam1  ,
      const std::string&                                nam2  )
    {
      return LoKi::HltBinders::bind 
        ( fun , fun2 , vct , LoKi::StdMath::abs_greater<double>() , nam1 , nam2 ) ;
    }      
    // ========================================================================
    
    
    
  } // end of namespace LoKi::HltBinders 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTBINDERS_H
// ============================================================================
