// $Id: HltBinders.h,v 1.9 2009-05-09 19:33:47 ibelyaev Exp $
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
#include "LoKi/apply.h"
#include "LoKi/Objects.h"
#include "LHCbMath/LHCbMath.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace HltBinders
  {    
    // ========================================================================
    /** @class LightBinder1st 
     *  "Ligth" (optimized) version of class LoKi::Binder1st
     *  The class is not supposed to be used as LoKi-function, 
     *  it is just a useful intermediate object for efficient evaluations 
     *  
     *  @attention the class does not own the argument and the function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE1,class TYPE2, class TYPE3=double>
    class LightBinder1st
    {
    private:
      // ======================================================================
      /// the actual type for data-holder 
      typedef typename LoKi::Holder<TYPE1,TYPE2>                      Holder_ ;
      /// the actual type of
      typedef typename LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>    Fun_ ;
      // ======================================================================
    public: // needed for adapters 
      // ======================================================================
      typedef typename LoKi::Functor<TYPE2,TYPE3>::argument_type argument_type ;
      typedef typename LoKi::Functor<TYPE2,TYPE3>::result_type   result_type   ;
      // ======================================================================      
    public:
      // ======================================================================
      /// contructor from the function and argument
      LightBinder1st 
      ( typename Fun_::first_argument value , 
        const    Fun_&                fun   ) 
        : m_first ( value ) 
        , m_fun   ( &fun   ) 
      {}
      /// contructor from the function and argument
      LightBinder1st 
      ( const    Fun_&                fun   ,
        typename Fun_::first_argument value )
        : m_first (  value ) 
        , m_fun   ( &fun   )  
      {}
      /// evaluate the  function
      inline typename LoKi::Functor<TYPE2,TYPE3>::result_type operator() 
        ( typename  LoKi::Functor<TYPE2,TYPE3>::argument a ) const 
      {
        LoKi::Apply<Holder_,TYPE3> applicator ( m_fun ) ;
        return  applicator ( Holder_( m_first ,  a ) ) ; 
      }
      /// evaluate the  function (HltSpecific) 
      inline typename LoKi::Functor<TYPE2,TYPE3>::result_type operator() 
        ( const TYPE2* a ) const 
      {
        LoKi::Apply<Holder_,TYPE3> applicator ( m_fun ) ;
        return  applicator ( Holder_( m_first ,  *a ) ) ; 
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      LightBinder1st();                  // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the fixed argument 
      typename Holder_::First  m_first  ;                 // the fixed argument 
      /// the actual function 
      const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>* m_fun    ; //  func
      // ======================================================================
    } ;
    // ========================================================================
    /** @class LigthBinder2nd 
     *  "Ligth" (optimized) version of class LoKi::Binder2nd
     *  The class is not supposed to be used as LoKi-function, 
     *  it is just a useful intermediate object for efficient evaluations 
     *  
     *  @attention the class does not own the argument and the function
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE1,class TYPE2,class TYPE3=double>
    class LightBinder2nd
    {
    private:
      // ======================================================================
      /// the actual type for data-holder 
      typedef typename LoKi::Holder<TYPE1,TYPE2>                      Holder_ ;
      /// the actual type of
      typedef typename LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>    Fun_ ;
      // ======================================================================
    public: // needed for adapters 
      // ======================================================================
      typedef typename LoKi::Functor<TYPE1,TYPE3>::argument_type argument_type ;
      typedef typename LoKi::Functor<TYPE1,TYPE3>::result_type   result_type   ;
      // ======================================================================
    public:
      // ======================================================================
      /// contructor from the function and argument
      LightBinder2nd 
      ( typename Fun_::second_argument value , 
        const    Fun_&                 fun   ) 
        : m_second (  value ) 
        , m_fun    ( &fun   ) 
      {}
      /// contructor from the function and argument
      LightBinder2nd 
      ( const    Fun_&                 fun   ,
        typename Fun_::second_argument value )
        : m_second (  value ) 
          , m_fun    ( &fun   )  
      {}
      /// evaluate the  function
      inline typename LoKi::Functor<TYPE1,TYPE3>::result_type operator() 
        ( typename  LoKi::Functor<TYPE1,TYPE3>::argument a ) const 
      {
        LoKi::Apply<Holder_,TYPE3> applicator ( m_fun ) ;
        return applicator ( Holder_( a , m_second ) ) ;
      }
      /// evaluate the  function (Hlt-specific) 
      inline typename LoKi::Functor<TYPE1,TYPE3>::result_type operator() 
        ( const TYPE1* a ) const 
      {
        LoKi::Apply<Holder_,TYPE3> applicator ( m_fun ) ;
        return applicator ( Holder_( *a , m_second ) ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      LightBinder2nd ();                 // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      // the fixed argument 
      typename Holder_::Second            m_second ;      // the fixed argument 
      /// the actual function 
      const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>* m_fun    ; //  func 
      // ======================================================================
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
    template <class TYPE,class TYPE2, class TYPE3=double>
    class HltBinder : public virtual LoKi::AuxFunBase
    {
    public:
      // ======================================================================
      typedef typename LoKi::Holder<TYPE,TYPE2>                    Holder_    ;
      typedef typename LoKi::Functor<Holder_,TYPE3>                Function_  ;
      typedef std::vector<TYPE2*>                                  Container  ;
      typedef std::pair<typename Container::const_iterator,double> Pair       ;
      // ======================================================================
    public :
      // ======================================================================
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
      /// constructor from this function and container:
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
      /// destructor 
      virtual ~HltBinder() {}
      // ======================================================================
    public:
      // ======================================================================
      /// protected constructor with INVALID constainer 
      HltBinder
      ( const Function_&  fun ) 
        : LoKi::AuxFunBase () 
        , m_fun ( fun ) 
        , m_vct ( 0   ) 
      {}
      // ======================================================================
    public:
      // ======================================================================
      /// find the extremal value with respect to the specified operation
      template <class BINOP>
      inline Pair extremum 
      ( typename LoKi::Functor<TYPE,TYPE3>::argument a     , 
        BINOP                                        binop ) const 
      {
        Assert ( 0 != m_vct , "Invalid container!" ) ; 
        return LoKi::Algs::extremum 
          ( m_vct -> begin () , 
            m_vct -> end   () , 
            LoKi::HltBinders::LightBinder1st<TYPE,TYPE2,TYPE3> 
            ( m_fun.func()  , a )  , 
            LoKi::Objects::_VALID<TYPE2*> () , // only valid pointers!
            binop            ) ;
      }
      // ======================================================================
    public:
      // ======================================================================
      /// get access to function 
      const Function_&  fun () const { return m_fun ; }
      /// get access to container 
      const Container*  vct () const { return m_vct ; }      
      // ======================================================================
    protected:
      // ======================================================================
      /// set the container 
      void setContainer ( Container* vct ) const { m_vct = vct ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled  
      HltBinder() ;                     //  the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      typename LoKi::FunctorFromFunctor<Holder_,TYPE3> m_fun ; // the  function 
      mutable const Container*                         m_vct ; //     container 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class HltBinder2
     *  helper class to organize "binders-for-Hlt"
     *
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE,class TYPE2, class TYPE3=double>
    class HltBinder2 : public virtual LoKi::AuxFunBase
    {
    public:
      // ======================================================================
      typedef typename LoKi::Holder<TYPE,TYPE2>                    Holder_    ;
      typedef typename LoKi::Functor<Holder_,TYPE3>                Function_  ;
      typedef std::vector<TYPE*>                                   Container  ;
      typedef std::pair<typename Container::const_iterator,double> Pair       ;
      // ======================================================================
    public :
      // ======================================================================
      /// constructor from this function and container:
      HltBinder2
      ( const Function_&  fun , 
        const Container*  vct ) 
        : LoKi::AuxFunBase () 
        , m_fun ( fun ) 
        , m_vct ( vct ) 
      {
        Assert ( 0 != m_vct , "Invalid container!" ) ; 
      }
      /// constructor from this function and container:
      HltBinder2
      ( const Container*  vct ,
        const Function_&  fun )
        : LoKi::AuxFunBase () 
        , m_fun ( fun ) 
        , m_vct ( vct ) 
      {
        Assert ( 0 != m_vct , "Invalid container!" ) ; 
      }
      // copy constructor
      HltBinder2 ( const HltBinder2& right ) 
        : LoKi::AuxFunBase () 
        , m_fun ( right.m_fun ) 
        , m_vct ( right.m_vct ) 
      {}
      /// destructor 
      virtual ~HltBinder2 () {}
      // ======================================================================
    public:
      // ======================================================================
      /// protected constructor with INVALID constainer 
      HltBinder2
      ( const Function_&  fun ) 
        : LoKi::AuxFunBase () 
        , m_fun ( fun ) 
        , m_vct ( 0   ) 
      {}
      // ======================================================================
    public:
      // ======================================================================
      /// find the extremal value with respect to the specified operation
      template <class BINOP>
      inline Pair extremum 
      ( typename LoKi::Functor<TYPE2,TYPE3>::argument a     , 
        BINOP                                         binop ) const 
      {
        Assert ( 0 != m_vct , "Invalid container!" ) ; 
        return LoKi::Algs::extremum 
          ( m_vct -> begin () , 
            m_vct -> end   () , 
            LoKi::HltBinders::LightBinder2nd<TYPE,TYPE2,TYPE3> 
            ( m_fun.func()  , a )  , 
            LoKi::Objects::_VALID<TYPE*> () , // only valid pointers!
            binop            ) ;
      }
      // ======================================================================
    public:
      // ======================================================================
      /// get access to function 
      const Function_&  fun () const { return m_fun ; }
      /// get access to container 
      const Container*  vct () const { return m_vct ; }      
      // ======================================================================
    public:
      // ======================================================================
      /// set the container 
      void setContainer ( Container* vct ) const { m_vct = vct ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled  
      HltBinder2() ;                    //  the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function 
      typename LoKi::FunctorFromFunctor<Holder_,TYPE3> m_fun ; // the  function 
      mutable const Container*                         m_vct ; //     container 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BinderValue
     *  Auxillary class to represent various "binders"
     *  For the given argment, it funds the extremum of "bi"-function
     *  with against all elements from the reference container and 
     *  returns the extremal value 
     *
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1                  ,
              class TYPE2                  , 
              class TYPE3=double           , 
              class BINOP=std::less<TYPE3> > 
    class BinderValue : public LoKi::Functor<TYPE1,TYPE3>
    {
    protected:
      // ======================================================================
      typedef  typename LoKi::HltBinders::HltBinder<TYPE1,TYPE2,TYPE3> Binder ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the container and the function 
      BinderValue 
      ( const typename Binder::Function_& fun  , 
        const typename Binder::Container* vct  , 
        const std::string&                nam1 ,
        const std::string&                nam2 ) 
        : LoKi::Functor<TYPE1,TYPE3> () 
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
        : LoKi::Functor<TYPE1,TYPE3> () 
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
      virtual typename LoKi::Functor<TYPE1,TYPE3>::result_type operator() 
        ( typename LoKi::Functor<TYPE1,TYPE3>::argument a ) const 
      {
        typename Binder::Pair r = m_binder.extremum ( a , m_binop ) ;
        Assert ( m_binder.vct()-> end() != r.first ,"No valid tracks are found" ) ;
        return r.second ;
      }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << m_nam1 << "(" << fun() << m_nam2 << ")" ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get access to the binder 
      const Binder& binder() const { return m_binder ; }
      /// cast to  the binder
      operator const Binder& () const { return binder() ; }
      // get access to binder function 
      const typename Binder::Function_& fun () const { return m_binder.fun() ; }   
      // ======================================================================
    private:
      // ======================================================================
      /// the binder itself 
      Binder      m_binder ;                               // the binder itself 
      /// binary operation
      BINOP       m_binop  ;                                // binary operation
      /// function name 
      std::string m_nam1   ;
      /// function name 
      std::string m_nam2   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class BinderValue2
     *  Auxillary class to represent various "binders"
     *  For the given argment, it funds the extremum of "bi"-function
     *  with against all elements from the reference container and 
     *  returns the extremal value 
     *
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1                  ,
              class TYPE2                  , 
              class TYPE3=double           , 
              class BINOP=std::less<TYPE3> > 
    class BinderValue2 : public LoKi::Functor<TYPE2,TYPE3>
    {
    protected:
      // ======================================================================
      typedef  typename LoKi::HltBinders::HltBinder2<TYPE1,TYPE2,TYPE3> Binder ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the container and the function 
      BinderValue2 
      ( const typename Binder::Function_& fun  , 
        const typename Binder::Container* vct  , 
        const std::string&                nam1 ,
        const std::string&                nam2 ) 
        : LoKi::Functor<TYPE2,TYPE3> () 
        , m_binder ( fun , vct ) 
        , m_binop  ( BINOP ()  ) 
        , m_nam1   ( nam1     ) 
        , m_nam2   ( nam2     ) 
      {}
      /// constructor from the binder:
      BinderValue2 
      ( const Binder&      binder , 
        const std::string& nam1   ,
        const std::string& nam2   ) 
        : LoKi::Functor<TYPE2,TYPE3> () 
        , m_binder ( binder   ) 
        , m_binop  ( BINOP () ) 
        , m_nam1   ( nam1     ) 
        , m_nam2   ( nam2     ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~BinderValue2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BinderValue2* clone () const { return new BinderValue2(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<TYPE2,TYPE3>::result_type operator() 
        ( typename LoKi::Functor<TYPE2,TYPE3>::argument a ) const 
      {
        typename Binder::Pair r = m_binder.extremum ( a , m_binop ) ;
        Assert ( m_binder.vct()-> end() != r.first ,"No valid tracks are found" ) ;
        return r.second ;
      }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << m_nam1 << "(" << fun() << m_nam2 << ")" ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get access to the binder 
      const Binder& binder() const { return m_binder ; }
      /// cast to  the binder
      operator const Binder& () const { return binder() ; }
      // get access to binder function 
      const typename Binder::Function_& fun () const { return m_binder.fun() ; }   
      // ======================================================================
    private:
      // ======================================================================
      /// the binder itself 
      Binder      m_binder ;                               // the binder itself 
      /// binary operation
      BINOP       m_binop  ;                                // binary operation
      /// function name 
      std::string m_nam1   ;
      /// function name 
      std::string m_nam2   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class BinderFun
     *  Auxillary class to represent various "binders"
     *  For the given argment, it finds the extremum of "bi"-function
     *  with against all elements from the reference container, 
     *  and evaluates the second function for the found element,,
     *
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1                  ,
              class TYPE2                  , 
              class TYPE3=double           , 
              class BINOP=std::less<TYPE3> ,
              class TYPE4=double           > 
    class BinderFun : public LoKi::Functor<TYPE1,TYPE4>
    {
    protected:
      // ======================================================================
      typedef  typename LoKi::HltBinders::HltBinder<TYPE1,TYPE2,TYPE3> Binder ;
      typedef  LoKi::Functor<TYPE2,TYPE4>                              Fun2   ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the container and the functions 
      BinderFun
      ( const typename Binder::Function_& fun  ,
        const Fun2&                       fun2 ,
        const typename Binder::Container* vct  , 
        const std::string&                nam1 ,
        const std::string&                nam2 ) 
        : LoKi::Functor<TYPE1,TYPE4> () 
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
        : LoKi::Functor<TYPE1,TYPE4> () 
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
      virtual typename LoKi::Functor<TYPE1,TYPE4>::result_type operator() 
        ( typename LoKi::Functor<TYPE1,TYPE4>::argument a ) const 
      {
        typename Binder::Pair r = m_binder.extremum ( a , m_binop ) ;
        Assert ( m_binder.vct()-> end() != r.first , "No valid elements are found" ) ;
        //
        LoKi::Apply<TYPE2,TYPE4> applicator ( & m_fun2.func() ) ;
        return applicator ( *r.first ) ;
      }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << m_nam1 << "("<< fun() << "," << fun2() << m_nam2 << ")"; }
      // ======================================================================
    public:
      // ======================================================================
      /// cast to  the binder
      operator const Binder&          () const { return binder()       ; }
      /// get access to the binder 
      const Binder&            binder () const { return m_binder       ; }
      /// get access to binder function 
      const typename Binder::Function_& fun () const { return m_binder.fun() ; }   
      /// get access to own function 
      const Fun2&              fun2   () const { return m_fun2         ; }   
      // ======================================================================
    private:
      // ======================================================================
      /// the binder itself 
      Binder m_binder ;                                    // the binder itself 
      /// binary operation
      BINOP  m_binop  ;                                     // binary operation
      /// the function to be evaluated 
      LoKi::FunctorFromFunctor<TYPE2,TYPE4>  m_fun2 ;           // the function 
      /// function name 
      std::string m_nam1   ;
      /// function name 
      std::string m_nam2   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class BinderFun2
     *  Auxillary class to represent various "binders"
     *  For the given argment, it finds the extremum of "bi"-function
     *  with against all elements from the reference container, 
     *  and evaluates the second function for the found element,,
     *
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1                  ,
              class TYPE2                  , 
              class TYPE3=double           , 
              class BINOP=std::less<TYPE3> , 
              class TYPE4=double           >
    class BinderFun2 : public LoKi::Functor<TYPE2,TYPE4>
    {
    protected:
      // ======================================================================
      typedef  typename LoKi::HltBinders::HltBinder2<TYPE1,TYPE2,TYPE3> Binder ;
      typedef  LoKi::Functor<TYPE1,TYPE4>                               Fun2   ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the container and the functions 
      BinderFun2
      ( const typename Binder::Function_& fun  ,
        const Fun2&                       fun2 ,
        const typename Binder::Container* vct  , 
        const std::string&                nam1 ,
        const std::string&                nam2 ) 
        : LoKi::Functor<TYPE2,TYPE4> () 
        , m_binder ( fun , vct ) 
        , m_binop  ( BINOP ()  ) 
        , m_fun2   ( fun2      ) 
        , m_nam1   ( nam1     ) 
        , m_nam2   ( nam2     ) 
      {}
      /// constructor from the binder and function 
      BinderFun2
      ( const Binder&      binder ,
        const Fun2&        fun2   ,
        const std::string& nam1   ,
        const std::string& nam2   ) 
        : LoKi::Functor<TYPE2,TYPE4> () 
        , m_binder ( binder    ) 
        , m_binop  ( BINOP ()  ) 
        , m_fun2   ( fun2      ) 
        , m_nam1   ( nam1     ) 
        , m_nam2   ( nam2     ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~BinderFun2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BinderFun2* clone () const { return new BinderFun2(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<TYPE2,TYPE4>::result_type operator() 
        ( typename LoKi::Functor<TYPE2,TYPE4>::argument a ) const 
      {
        typename Binder::Pair r = m_binder.extremum ( a , m_binop ) ;
        Assert ( m_binder.vct()-> end() != r.first , "No valid elements are found" ) ;
        //
        LoKi::Apply<TYPE1,TYPE4> applicator ( & m_fun2.func() ) ;
        return applicator ( *r.first ) ;
      }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << m_nam1 << "("<< fun() << "," << fun2() << m_nam2 << ")"; }
      // ======================================================================
    public:
      // ======================================================================
      /// cast to  the binder
      operator const Binder&          () const { return binder()       ; }
      /// get access to the binder 
      const Binder&            binder () const { return m_binder       ; }
      /// get access to binder function 
      const typename Binder::Function_& fun () const { return m_binder.fun() ; }   
      /// get access to own function 
      const Fun2&              fun2   () const { return m_fun2         ; }   
      // ======================================================================
    private:
      // ======================================================================
      /// the binder itself 
      Binder m_binder ;                                    // the binder itself 
      /// binary operation
      BINOP  m_binop  ;                                     // binary operation
      /// the function to be evaluated 
      LoKi::FunctorFromFunctor<TYPE1,TYPE4>  m_fun2 ;           // the function 
      /// function name 
      std::string m_nam1   ;
      /// function name 
      std::string m_nam2   ;
      // ======================================================================
    };
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    const std::vector<LHCb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bind 
     *            ( TTrDOCA ,                //  2-argument function 
     *              tracks  ,                //  the container
     *              std::less<double>() ,    //  binary operation 
     *              "bindMin("          ,    //  needed for ptintout/ID 
     *              "_HLT_tracks)"      )    //  needed for ptintout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3,class BINOP >
    inline 
    LoKi::HltBinders::BinderValue<TYPE1,TYPE2,TYPE3,BINOP>
    bind 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>&   fun      ,
      const std::vector<TYPE2*>*                              vct      , 
      BINOP                                                /* binop */ ,
      const std::string&                                      nam1     ,
      const std::string&                                      nam2     )
    {
      return LoKi::HltBinders::BinderValue<TYPE1,TYPE2,TYPE3,BINOP> 
        ( fun , vct , nam1 , nam2 ) ;  
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHCb::Track*>* tracks = ... ;
     *
     *    TrFun key = bind 
     *            ( TTrDOCA ,                //  2-argument function 
     *              TrKEY   ,                //  the function
     *              tracks  ,                //  the container
     *              std::less<double>() ,    //  binary operation 
     *              "bindMin"           ,    //  needed for printout/ID 
     *              ",_HLT_tracks"      )    //  needed for printout/ID 
     *
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3,class BINOP, class TYPE4>
    inline 
    LoKi::HltBinders::BinderFun<TYPE1,TYPE2,TYPE3,BINOP,TYPE4>
    bind 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3 >& fun      ,
      const LoKi::Functor<TYPE2,TYPE4>&                      fun2     , 
      const std::vector<TYPE2*>*                             vct      ,
      BINOP                                               /* binop */ ,
      const std::string&                                     nam1     ,
      const std::string&                                     nam2     )
    {
      return LoKi::HltBinders::BinderFun<TYPE1,TYPE2,TYPE3,BINOP,TYPE4>
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
     *            ( TTrDOCA             ,    //  2-argument function 
     *              tracks              ,    //  the container
     *              ",_HLT_tracks"      )    //  needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2, class TYPE3>
    inline 
    LoKi::HltBinders::BinderValue<TYPE1,TYPE2,TYPE3,std::less<TYPE3> >
    bindMin 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun   ,
      const std::vector<TYPE2*>*                            vct   , 
      const std::string&                                    nam2  ,
      const std::string&                         nam1 = "bindMin" ) 
    {
      return LoKi::HltBinders::bind 
        ( fun , vct , std::less<TYPE3>() , nam1 , nam2 ) ;
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun key = bindMin
     *            ( TTrDOCA             , //  2-argument function 
     *              TrKEY               , //  the function
     *              tracks              , //  the container`
     *              ",_HLT_tracks"      ) //  needed for printout/ID 
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
    template <class TYPE1,class TYPE2, class TYPE3, class TYPE4>
    inline 
    LoKi::HltBinders::BinderFun
    <TYPE1,TYPE2,TYPE3,std::less<TYPE3>,TYPE4>
    bindMin 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun   ,
      const LoKi::Functor<TYPE2,TYPE4>&                     fun2  , 
      const std::vector<TYPE2*>*                            vct   ,
      const std::string&                                    nam2  ,
      const std::string&                         nam1 = "bindMin" ) 
    {
      return LoKi::HltBinders::bind 
        ( fun , fun2 , vct , std::less<TYPE3>() , nam1 , nam2 ) ;
    }      
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bindAbsMin 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              tracks              ,    //  the container
     *              ",_HLT_tracks"      )    //  needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2, class TYPE3>
    inline 
    LoKi::HltBinders::BinderValue
    <TYPE1,TYPE2,TYPE3,LHCb::Math::abs_less<TYPE3> >
    bindAbsMin 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun   ,
      const std::vector<TYPE2*>*                            vct   , 
      const std::string&                                    nam2  ,
      const std::string&                      nam1 = "bindAbsMin" ) 
    {
      return LoKi::HltBinders::bind 
        ( fun , vct , LHCb::Math::abs_less<TYPE3>() , nam1 , nam2 ) ;
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun key = bindAbsMin
     *            ( TTrDOCA             , //   2-argument function 
     *              TrKEY               , //   the function
     *              tracks              , //   the container
     *              ",_HLT_tracks"      ) //   needed for ptintout/ID 
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
    template <class TYPE1,class TYPE2, class TYPE3, class TYPE4>
    inline 
    LoKi::HltBinders::BinderFun
    <TYPE1,TYPE2,TYPE3,LHCb::Math::abs_less<TYPE3>, TYPE4>
    bindAbsMin 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun   ,
      const LoKi::Functor<TYPE2,TYPE4>&                     fun2  , 
      const std::vector<TYPE2*>*                            vct   ,
      const std::string&                                    nam2  ,
      const std::string&                      nam1 = "bindAbsMin" ) 
    {
      return LoKi::HltBinders::bind 
        ( fun , fun2 , vct , LHCb::Math::abs_less<TYPE3>() , nam1 , nam2 ) ;
    }      
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bindMax 
     *            ( TTrDOCA             ,    //  2-argument function 
     *              tracks              ,    //  the container
     *              ",_HLT_tracks"      ) ;  //  needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3>
    inline 
    LoKi::HltBinders::BinderValue
    <TYPE1,TYPE2,TYPE3,std::greater<TYPE3> >
    bindMax 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun   ,
      const std::vector<TYPE2*>*                            vct   , 
      const std::string&                                    nam2  ,
      const std::string&                         nam1 = "bindMax" ) 
    {
      return LoKi::HltBinders::bind 
        ( fun , vct , std::greater<TYPE3>() , nam1 , nam2 ) ;
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHCb::Track*>* tracks = ... ;
     *
     *    TrFun key = bindMax
     *            ( TTrDOCA             , //  2-argument function 
     *              TrKEY               , //  the function
     *              tracks              , //  the container
     *              ",_HLT_tracks"      ) //  needed for printout/ID 
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
    template <class TYPE1,class TYPE2, class TYPE3, class TYPE4>
    inline 
    LoKi::HltBinders::BinderFun
    <TYPE1,TYPE2,TYPE3,std::greater<TYPE3>,TYPE4>
    bindMax 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun   ,
      const LoKi::Functor<TYPE2,TYPE4>&                     fun2  , 
      const std::vector<TYPE2*>*                            vct   ,
      const std::string&                                    nam2  ,
      const std::string&                         nam1 = "bindMax" ) 
    {
      return LoKi::HltBinders::bind 
        ( fun , fun2 , vct , std::greater<TYPE3>() , nam1 , nam2 ) ;
    }      
    // ========================================================================
    /** helper function to create the the metatunction:
     *
     *  @code
     *
     *    const std::vector<LHCb::Track*>* tracks = ... ;
     *
     *    TrFun docaMin = bindAbsMax 
     *            ( TTrDOCA             ,    // 2-argument function 
     *              tracks              ,    // the container
     *              ",_HLT_tracks)"     )    // needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2, class TYPE3>
    inline 
    LoKi::HltBinders::BinderValue
    <TYPE1,TYPE2,TYPE3,LHCb::Math::abs_greater<TYPE3> >
    bindAbsMax 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun   ,
      const std::vector<TYPE2*>*                            vct   , 
      const std::string&                                    nam2  ,
      const std::string&                      nam1 = "bindAbsMax" ) 
    {
      return LoKi::HltBinders::bind 
        ( fun , vct , LHCb::Math::abs_greater<TYPE3>() , nam1 , nam2 ) ;
    }
    // ========================================================================
    /** helper function to create the the metafunction:
     *
     *  @code
     *
     *    const std::vector<LHcb::Track*>* tracks = ... ;
     *
     *    TrFun key = bindAbsMax
     *            ( TTrDOCA             ,   // 2-argument function 
     *              TrKEY               ,   // the function
     *              tracks              ,   // the container
     *              ",_HLT_tracks"      ) ; // needed for printout/ID 
     * 
     *  @endcode 
     * 
     *  The code is inspired by Estd/EFunctions package 
     *  by Jose Angel Hernado Morata
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */    
    template <class TYPE1,class TYPE2,class TYPE3, class TYPE4>
    inline 
    LoKi::HltBinders::BinderFun
    <TYPE1,TYPE2,TYPE3,LHCb::Math::abs_greater<TYPE3>,TYPE4>
    bindAbsMax 
    ( const LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>& fun   ,
      const LoKi::Functor<TYPE2,TYPE4>&                     fun2  , 
      const std::vector<TYPE2*>*                            vct   ,
      const std::string&                                    nam2  ,
      const std::string&                      nam1 = "bindAbsMax" ) 
    {
      return LoKi::HltBinders::bind 
        ( fun , fun2 , vct , LHCb::Math::abs_greater<TYPE3>() , nam1 , nam2 ) ;
    }      
    // ========================================================================
  } // end of namespace LoKi::HltBinders 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTBINDERS_H
// ============================================================================
