// $Id: Filters.h,v 1.9 2010-06-05 20:13:30 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_FILTERS_H 
#define LOKI_FILTERS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD &STL
// ============================================================================
#include <algorithm>
#include <climits>
// ============================================================================
// LHCb
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functor.h"
#include "LoKi/Functions.h"
#include "LoKi/select.h"
#include "LoKi/Algs.h"
#include "LoKi/Primitives.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
#include "boost/type_traits/is_pointer.hpp"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Functors
   *  helper namespace for implementation of Gerhard's ideas
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
   *  @date 2007-10-30
   */
  namespace Functors
  {
    // ========================================================================
    /** @class Select
     *
     *  Simple functor for "filtering" or "selection".
     *
     *  @code 
     *
     *   typedef Select<LHCb::Track*,LHCb::Track> SELECT ;
     *
     *   // the selection criteria (predicate)  itself
     *   const LoKi::Functor<LHCb::Track,bool>& good = ... ;
     *
     *   std::vector<LHCb::Track*> input = ... ;
     *
     *   std::vector<LHCb::Track*> filtered = input >> SELECT ( good )  ;
     *
     *  @endcode 
     * 
     *  The concept belongs to the Gerhard "The Great" Raven.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-02
     */
    template <class TYPE, class TYPE2=TYPE> 
    class Select : public LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > 
    {
    protected:
      // ======================================================================
      /// the base class 
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > uBase    ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor form the basic predicate:
      Select ( const LoKi::Functor<TYPE2,bool>& fun ) 
        : uBase       (     ) 
        , m_predicate ( fun ) 
      {}
      /// copy constructor 
      Select ( const Select& right ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_predicate      ( right.m_predicate ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Select() {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Select* clone() const { return new Select ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typename uBase::result_type out ;
        out.reserve ( a.size() ) ;
        LoKi::apply_filter 
          ( a.begin () , 
            a.end   () , 
            m_predicate.func() , 
            std::back_inserter ( out ) ) ;
        return out ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "select(" << m_predicate << ")" ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      Select () ;                                      // no default contructor
      // ======================================================================
    private:
      // ======================================================================
      /// the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,bool> m_predicate ; // the functor itself 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Yields
     *
     *  Simple functor for "mapping" of the containers.
     *
     *  @code 
     *
     *   typedef Yields<LHCb::Track*,double,LHCb::Track> YIELD ;
     *
     *   const LoKi::Functor<LHCb::Track,double>& pt = ... ;
     *
     *   std::vector<LHCb::Track*> input = ... ;
     * 
     *   std::vector<double>       results = input >> YIELD( pt )  ;
     *
     *  @endcode 
     * 
     *  The concept belongs to the Gerhard "The Great" Raven and the 
     *  name comes from Marcel Merk.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-02
     */
    template <class TYPE,class TYPE1, class TYPE2=TYPE>
    class Yields : public LoKi::Functor<std::vector<TYPE>,std::vector<TYPE1> >
    {
    public:
      // ======================================================================
      /// the underlying basic functor 
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE1> >      uBase ;
      // ======================================================================
    public:
      // ======================================================================
      /// contructor from the basic functor 
      Yields ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase       (     ) 
        , m_functor   ( fun ) 
      {}
      /// copy constriuctor 
      Yields ( const Yields& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Yields () {}
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Yields* clone() const { return new Yields ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const 
      {
        typename uBase::result_type out ;
        out.reserve ( a.size () ) ;
        LoKi::apply 
          ( a.begin () , 
            a.end   () , 
            m_functor.func() , 
            std::back_inserter ( out ) ) ;
        return out ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "yields(" << m_functor << ")"; };
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      Yields() ;                                       // no default contructor
      // ======================================================================
    private:
      // ======================================================================
      /// the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ;   // the functor itself 
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class Process
     *
     *  Simple functor for "processing" of the containers.
     *
     *  @code 
     *
     *   typedef Process<LHCb::Track*,double,LHCb::Track> PROCESS ;
     *
     *    // get some functor for plotting the TrPT 
     *    AIDA::IHistogram1D* histo = ...  
     *    FunctorFromFunctor<LHCb::Track,double> plotter = monitor ( TrPT , histo )  ;
     *
     *    std::vector<LHCb::Track*> input = ... ;
     *
     *    input >> PROCESS( plotter ) ;
     *
     *  @endcode 
     *
     *  The concept belongs to the Gerhard "The Great" Raven.
     * 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-02
     */    
    template <class TYPE,class TYPE1, class TYPE2=TYPE>
    class Process: public LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> >
    {
    protected:
      // ======================================================================
      /// the underlying basic functor 
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> >       uBase ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor form the basic predicate:
      Process ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      Process ( const Process& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Process() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Process* clone() const { return new Process ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        LoKi::apply ( a.begin() , a.end() , m_functor.func () ) ;
        return a ;        
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "process(" << m_functor << ")"; };
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Process() ;                                      // no default contructor
      // ======================================================================
    private:
      // ======================================================================
      /// the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ;   // the functor itself 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Tee
     *  Simple functor for "tee" of the containers.
     *
     *  @code
     *
     *   // get some functor for plotting the TrPT 
     *   AIDA::IHistogram1D* histo = ...  
     *   FunctorFromFunctor<LHCb::Track,double> plotter = monitor ( TrPT , histo )  ;
     *
     *   // get some functor for selection of track with maximal pt 
     *   MaxElement<LHCb::Track*>  maxPt ( TrPT ) ;
     *
     *   // container of input tracks:
     *   std::vector<LHCb::Track*> input = ... ;
     * 
     *   // "tee"
     *   input >> Tee<LHCb::Track*> ( compose ( maxPt , plotter ) ) ;   
     *  
     *  @endcode 
     *
     *  The concept belongs to the Gerhard "The Great" Raven.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-02
     */    
    template <class TYPE, class TYPE2=std::vector<TYPE> >
    class Tee : public LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > 
    {
    public:
      // ======================================================================
      /// the underlying basic functor 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE2>              uFunctor ;
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > uBase    ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor form the basic predicate:
      Tee ( const uFunctor& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constriuctor 
      Tee ( const Tee& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Tee() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Tee* clone() const { return new Tee ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typename uBase::result_type out = a ;
        LoKi::apply ( m_functor.func () , a ) ;
        return out ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "tee(" << m_functor << ")"; };
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is private:
      Tee () ;                                         // no default contructor
      // ======================================================================
    private:
      // ======================================================================
      /// the basic functor itself 
      LoKi::FunctorFromFunctor<std::vector<TYPE>,TYPE2> m_functor ; //  functor 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Max 
     *  simple meta-functor to find the maximum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class Max : public LoKi::Functor<std::vector<TYPE>,TYPE1> 
    {
      // ======================================================================
    protected:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE1> uBase    ;     // the base 
      /// the base 
      typedef LoKi::Functor<TYPE2,TYPE1>             uFunctor ;  // the functor
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      Max ( const LoKi::Functor<TYPE2,TYPE1>& fun                                      , 
            const TYPE1                       val = -std::numeric_limits<TYPE1>::max() )
        : uBase     (     ) 
        , m_functor ( fun )
        , m_value   ( val )
      {
        BOOST_STATIC_ASSERT( std::numeric_limits<TYPE1>::is_specialized ) ;
      }
      /// MANDATORY: virtual destructor 
      virtual ~Max() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Max* clone() const { return new Max ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        // return the value 
        if ( a.empty() ) { return this->val() ; } // RETURN 
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        LoKi::Apply<TYPE2,TYPE1> app ( &this->func() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin () , 
                                 a.end   () , 
                                 app        , 
                                 std::greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return res.second ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { 
        s << " max_value(" << this->func()  ; 
        BOOST_STATIC_ASSERT( std::numeric_limits<TYPE1>::is_specialized ) ;
        if ( -std::numeric_limits<TYPE1>::max() != m_value ) 
        { s << "," << this->val()  ; }
        return s << ") " ;
      }  
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      Max () ;                                         // no default contructor
      // ======================================================================
    protected:
      // ======================================================================
      /// get the functor 
      const LoKi::Functor<TYPE2,TYPE1>& func() const { return m_functor.func() ; }
      /// get the value 
      const TYPE1&                      val () const { return m_value ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ;          // the functor
      /// the value 
      TYPE1                                 m_value   ;          //   the value 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AbsMax 
     *  simple meta-functor to find the maximum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class AbsMax : public LoKi::Functors::Max<TYPE,TYPE2,TYPE1>
    {
      // ======================================================================
    protected:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE1> uBase     ; // the base 
      /// the base 
      typedef LoKi::Functor<TYPE2,TYPE1>             uFunctor  ; // the functor
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      AbsMax 
      ( const LoKi::Functor<TYPE2,TYPE1>& fun     , 
        const TYPE1                       val = 0 )
        : LoKi::Functors::Max<TYPE,TYPE2,TYPE1>  ( fun , val ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~AbsMax() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsMax* clone() const { return new AbsMax ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        // 
        if ( a.empty() ) { return this->val() ; } // REUTRN 
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        LoKi::Apply<TYPE2,TYPE1> app ( &this->func () ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin () , 
                                 a.end   () , 
                                 app        , 
                                 LHCb::Math::abs_greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return res.second ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      {
        s << " max_abs_value(" << this->func()  ; 
        if ( 0 != this->val() ) { s << "," << this->val() ; }
        return s << ") " ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      AbsMax () ;                                      // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Min
     *  simple meta-functor to find the minimum 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class Min : public LoKi::Functors::AbsMax<TYPE,TYPE2,TYPE1>
    {
    protected:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE1> uBase     ; //    the base 
      /// the base 
      typedef LoKi::Functor<TYPE2,TYPE1> uFunctor              ; // the functor
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      Min ( const LoKi::Functor<TYPE2,TYPE1>& fun                                     , 
            const TYPE1                       val = std::numeric_limits<TYPE1>::max() )
        : LoKi::Functors::AbsMax<TYPE,TYPE2,TYPE1> ( fun , val )  
      {
        BOOST_STATIC_ASSERT( std::numeric_limits<TYPE1>::is_specialized ) ;
      }
      /// MANDATORY: virtual destructor 
      virtual ~Min() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Min* clone() const { return new Min ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        // return the value 
        if ( a.empty() ) { return this->val() ; }
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        LoKi::Apply<TYPE2,TYPE1> app ( &this->func() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin () , 
                                 a.end   () , 
                                 app        , 
                                 std::less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return res.second ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { 
        s << " min_value(" << this->func() ; 
        BOOST_STATIC_ASSERT( std::numeric_limits<TYPE1>::is_specialized ) ;
        if ( std::numeric_limits<TYPE1>::max() != this->val() ) 
        { s << "," << this->val() ; }
        return s << ") " ;
      }  
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      Min () ;                                         // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AbsMin
     *  simple meta-functor to find the minimum 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class AbsMin : public LoKi::Functors::Min<TYPE,TYPE2,TYPE1>
    {
    protected:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE1> uBase    ; //     the base 
      // the base 
      typedef LoKi::Functor<TYPE2,TYPE1>             uFunctor ; //  the functor
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      AbsMin
      ( const LoKi::Functor<TYPE2,TYPE1>& fun ,
        const TYPE1                       val = std::numeric_limits<TYPE1>::max() )
        : LoKi::Functors::Min<TYPE,TYPE2,TYPE1> ( fun , val ) 
      {
        BOOST_STATIC_ASSERT( std::numeric_limits<TYPE1>::is_specialized ) ;
      }
      /// MANDATORY: virtual destructor 
      virtual ~AbsMin() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsMin* clone() const { return new AbsMin ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        //
        if  ( a.empty() ) { return this->val() ; }
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        LoKi::Apply<TYPE2,TYPE1> app ( &this->func() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            LHCb::Math::abs_less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return res.second ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      {
        s << " min_abs_value(" << this->func() ; 
        BOOST_STATIC_ASSERT( std::numeric_limits<TYPE1>::is_specialized ) ;
        if (  std::numeric_limits<TYPE1>::max() != this->val() ) 
        { s << "," << this->val() ; }
        return s << ") " ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      AbsMin () ;                                      // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MaxElement 
     *  simple meta-functor to find the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class MaxElement : public LoKi::Functor<std::vector<TYPE>,TYPE> 
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE>   uBase    ; //    the base
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; // the functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      MaxElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~MaxElement() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxElement* clone() const { return new MaxElement ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const 
      {
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.func () ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            std::greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "max_element(" << m_functor << ")"; } ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      MaxElement() ;                                   // no default contructor
      // ======================================================================
    protected:
      // ======================================================================
      const LoKi::Functor<TYPE2,TYPE1>& func() const { return m_functor.func() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ;          // the functor
      // ======================================================================
    } ;
    // ========================================================================
    template <class TYPE,class TYPE2,class TYPE1>
    class MaxElement<TYPE*,TYPE2,TYPE1>
      : public LoKi::Functor<std::vector<TYPE*>,TYPE*> 
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE*>,TYPE*> uBase    ; //    the base
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; // the functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      MaxElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~MaxElement() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxElement* clone() const { return new MaxElement ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const 
      {
        //
        if ( a.empty() ) { return 0 ; } // return 0 
        //
        typedef typename std::vector<TYPE*>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func () ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            std::greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << " max_element( " << this->func() << ") "; } ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      MaxElement() ;                                   // no default contructor
      // ======================================================================
    protected:
      // ======================================================================
      const LoKi::Functor<TYPE2,TYPE1>& func() const { return m_functor.func() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ;          // the functor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinElement
     *  simple meta-functor to find the minimal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class MinElement : public LoKi::Functors::MaxElement<TYPE,TYPE2,TYPE1>
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE>  uBase    ;     // the base 
      typedef LoKi::Functor<TYPE2,TYPE1>             uFunctor ;     //  functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      MinElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : LoKi::Functors::MaxElement<TYPE,TYPE2,TYPE1> ( fun ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~MinElement() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinElement* clone() const { return new MinElement ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin () , 
                                 a.end   () , 
                                 app        , 
                                 std::less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << " min_element(" << this->func() << ") "; } ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      MinElement() ;                                   // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    template <class TYPE,class TYPE2,class TYPE1>
    class MinElement<TYPE*,TYPE2,TYPE1> 
      : public LoKi::Functors::MaxElement<TYPE*,TYPE2,TYPE1> 
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE*>,TYPE*> uBase    ;    // the base 
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ;    //  functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      MinElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : MaxElement<TYPE*,TYPE2,TYPE1>   ( fun ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~MinElement() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinElement* clone() const { return new MinElement ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        //
        if ( a.empty() ) { return 0 ; } // RETURN 
        //
        typedef typename std::vector<TYPE*>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func () ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin () , 
                                 a.end   () , 
                                 app        ,
                                 std::less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << " min_element(" << this->func() << ") "; } ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      MinElement() ;                                   // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AbsMaxElement 
     *  simple meta-functor to find the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class AbsMaxElement : public LoKi::Functors::MinElement<TYPE,TYPE2,TYPE1>
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE>  uBase    ; //     the base 
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>             uFunctor ; //  the functor
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      AbsMaxElement ( const LoKi::Functor<TYPE2,TYPE1>& fun )
        : LoKi::Functors::MinElement<TYPE,TYPE2,TYPE1> ( fun ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~AbsMaxElement() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsMaxElement* clone() const 
      { return new AbsMaxElement ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func () ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            LHCb::Math::abs_greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << " max_abs_element( " << this->func() << ") "; } ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      AbsMaxElement() ;                                // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    template <class TYPE,class TYPE2,class TYPE1>
    class AbsMaxElement<TYPE*,TYPE2,TYPE1> 
      : public LoKi::Functors::MinElement<TYPE*,TYPE2,TYPE1>
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE*>,TYPE*> uBase    ; //    the base 
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; // the functor
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      AbsMaxElement ( const LoKi::Functor<TYPE2,TYPE1>& fun )
        : LoKi::Functors::MinElement<TYPE*,TYPE2,TYPE1>( fun ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~AbsMaxElement() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsMaxElement* clone() const 
      { return new AbsMaxElement ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        // 
        if ( a.empty() ) { return 0 ; }                            // RETURN 
        // 
        typedef typename std::vector<TYPE*>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func () ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            LHCb::Math::abs_greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << " max_abs_element(" << this->func() << ") "; } ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      AbsMaxElement() ;                                // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AbsMinElement
     *  simple meta-functor to find the minimal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class AbsMinElement 
      : public LoKi::Functors::AbsMaxElement<TYPE,TYPE2,TYPE1>
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE>   uBase    ; //    the base 
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; // the functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      AbsMinElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : LoKi::Functors::AbsMaxElement<TYPE,TYPE2,TYPE1> ( fun ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~AbsMinElement() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsMinElement* clone() const 
      { return new AbsMinElement ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            LHCb::Math::abs_less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << " min_abs_element(" << this->func() << ") "; } ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      AbsMinElement() ;                                // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    template <class TYPE,class TYPE2,class TYPE1>
    class AbsMinElement<TYPE*,TYPE2,TYPE1> 
      : public LoKi::Functors::AbsMaxElement<TYPE*,TYPE2,TYPE1>
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE*>,TYPE*> uBase    ; //    the base 
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; // the functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      AbsMinElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : LoKi::Functors::AbsMaxElement<TYPE*,TYPE2,TYPE1> ( fun ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~AbsMinElement() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsMinElement* clone() const 
      { return new AbsMinElement ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        // 
        if ( a.empty() ) { return 0 ; }
        //
        typedef typename std::vector<TYPE*>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            LHCb::Math::abs_less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << " min_abs_element(" << this->func() << ") "; } ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default construct is private:
      AbsMinElement() ;                                // no default contructor
      // ======================================================================
    } ;
    // ========================================================================
    template <class TYPE,class TYPE1=TYPE, class TYPE2=bool>
    class Count : public LoKi::Functor<std::vector<TYPE>,double> 
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,double>  uBase    ; // the base 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the predicate 
      Count ( const LoKi::Functor<TYPE1,TYPE2>& cut ) 
        :  LoKi::Functor<std::vector<TYPE>,double> () 
        , m_cut ( cut ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Count () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Count* clone() const { return new Count ( *this ) ; }
      /// MANDATORY: the only one essential method:
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        const LoKi::Apply<TYPE1,TYPE2> app ( &m_cut.func() ) ;
        //
        size_t count = 0 ;
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        for ( _I it = a.begin() ; a.end() != it ; ++it ) 
        { if ( app ( *it ) ) { ++count ; } }
        //
        return count ; 
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "count(" << m_cut << ")" ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Count() ; /// default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================
      /// the predicate 
      LoKi::FunctorFromFunctor<TYPE1,TYPE2> m_cut ;            // the predicate 
      // ======================================================================
    };
    // ========================================================================
    template <class TYPE,class TYPE1=TYPE, class TYPE2=bool>
    class Has : public LoKi::Functor<std::vector<TYPE>,bool> 
    {
    private:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,bool>  uBase    ;     // the base 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the predicate 
      Has ( const LoKi::Functor<TYPE1,TYPE2>& cut ) 
        :  LoKi::Functor<std::vector<TYPE>,bool> () 
        , m_cut ( cut ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Has () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Has* clone() const { return new Has ( *this ) ; }
      /// MANDATORY: the only one essential method:
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        const LoKi::Apply<TYPE1,TYPE2> app ( &m_cut.func() ) ;
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        for ( _I it = a.begin() ; a.end() != it ; ++it ) 
        { if ( app ( *it ) ) { return true ; } }  // RETURN 
        //
        return false; 
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "has(" << m_cut << ")" ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Has() ; /// default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================
      /// the predicate 
      LoKi::FunctorFromFunctor<TYPE1,TYPE2> m_cut ;            // the predicate 
      // ======================================================================
    };
    // ========================================================================
    /** @class Empty
     *  Somple functor which checks the emptiness of the container 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-20
     */
    template <class TYPE>
    class Empty : public LoKi::Functor<std::vector<TYPE>,bool>
    {
    private:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,bool>  uBase    ;     // the base 
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Empty() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Empty* clone() const { return new Empty(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const { return a.empty() ; }
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Size
     *  Somple functor which checks the size of the container 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-20
     */
    template <class TYPE>
    class Size : public LoKi::Functor<std::vector<TYPE>,double>
    {
    private:
      // ======================================================================       
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,double>  uBase  ;     // the base 
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Size () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Size* clone() const { return new Size ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const { return a.size () ; }
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Union 
     *  simle functor to represent the "union" for two vector-functors
     *  @see LoKi::Operations::Union 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-06
     */
    template <class TYPE, class TYPE2> 
    class Union : public LoKi::Functor<TYPE,std::vector<TYPE2> >
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      Union 
      ( const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functor     <TYPE,std::vector<TYPE2> > () 
        , m_two ( fun1 , fun2 )  
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Union() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Union* clone() const { return new Union ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functor<TYPE,std::vector<TYPE2> >::result_type operator() 
        ( typename LoKi::Functor<TYPE,std::vector<TYPE2> >::argument a ) const 
      {
        LoKi::Operations::Union<TYPE2> _union ;
        return _union ( this->fun1 ( a ) , this->fun2 ( a ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " union( " << this->func1() 
                 << ","        << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Union () ;                         // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      typedef  LoKi::Functor     <TYPE,std::vector<TYPE2> > functor ;
      // ======================================================================
      /// evaluate the first functor 
      typename functor::result_type fun1 
      ( typename functor::argument a ) const { return m_two.fun1 ( a ) ; }
      /// evaluate the first functor 
      typename functor::result_type fun2 
      ( typename functor::argument a ) const { return m_two.fun2 ( a ) ; }
      // ======================================================================
      /// get the first functor 
      const functor& func1 ()           const { return m_two.func1 () ; }
      /// get the second functor 
      const functor& func2 ()           const { return m_two.func2 () ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the holder for two functors
      LoKi::TwoFunctors <TYPE,std::vector<TYPE2> > m_two ;      // two functors 
      // ======================================================================
    } ;
    // ========================================================================
    template <class TYPE2> 
    class Union<void,TYPE2> : public LoKi::Functor<void,std::vector<TYPE2> >
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      Union 
      ( const LoKi::Functor <void,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <void,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functor     <void,std::vector<TYPE2> > () 
        , m_two ( fun1 , fun2 )  
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Union() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Union* clone() const { return new Union ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functor<void,std::vector<TYPE2> >::result_type operator() 
        ( /* typename LoKi::Functor<void,std::vector<TYPE2> >::argument a */ ) const 
      {
        LoKi::Operations::Union<TYPE2> _union ;
        return _union ( this->fun1 ( /* a */ ) , this->fun2 ( /* a */ ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " union(" << this->func1() 
                 << ","       << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Union () ;                         // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      typedef  LoKi::Functor     <void,std::vector<TYPE2> > functor ;
      // ======================================================================
      /// evaluate the first functor 
      typename functor::result_type fun1 
      ( /* typename functor::argument a */ ) const { return m_two.fun1 ( /* a */ ) ; }
      /// evaluate the first functor 
      typename functor::result_type fun2 
      ( /* typename functor::argument a */ ) const { return m_two.fun2 ( /* a */ ) ; }
      // ======================================================================
      /// get the first functor 
      const functor& func1 ()           const { return m_two.func1 () ; }
      /// get the second functor 
      const functor& func2 ()           const { return m_two.func2 () ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the holder for two functors
      LoKi::TwoFunctors <void,std::vector<TYPE2> > m_two ;      // two functors 
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class Difference
     *  simle functor to represent the "difference" for two vector-functors
     *  @see LoKi::Operations::Difference 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-06
     */
    template <class TYPE, class TYPE2> 
    class Difference : public LoKi::Functors::Union <TYPE,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor two streamers 
      Difference
      ( const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functors::Union <TYPE,TYPE2> ( fun1 , fun2 ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Difference() {}
      /// MANDATORY: clone method ("virtual consturctor")
      virtual  Difference* clone() const { return new Difference ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functors::Union<TYPE,TYPE2>::result_type operator() 
        ( typename LoKi::Functors::Union<TYPE,TYPE2>::argument a ) const 
      {
        LoKi::Operations::Difference<TYPE2> _diff ;
        return _diff ( this->fun1 ( a ) , this->fun2 ( a ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " difference(" << this->func1() 
                 << ","            << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Difference () ;                    // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    template <class TYPE2> 
    class Difference<void,TYPE2> : public LoKi::Functors::Union <void,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      Difference
      ( const LoKi::Functor <void,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <void,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functors::Union <void,TYPE2> ( fun1 , fun2 ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Difference() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Difference* clone() const { return new Difference ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functors::Union<void,TYPE2>::result_type operator() 
        ( /* typename LoKi::Functors::Union<TYPE,TYPE2>::argument a */ ) const 
      {
        LoKi::Operations::Difference<TYPE2> _diff ;
        return _diff ( this->fun1 ( /* a */ ) , this->fun2 ( /* a */ ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " difference(" << this->func1() 
                 << ","            << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Difference () ;                   // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class Intersection
     *  simle functor to represent the "intersection" for two vector-functors
     *  @see LoKi::Operations::Intersection
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-06
     */
    template <class TYPE, class TYPE2> 
    class Intersection : public LoKi::Functors::Difference <TYPE,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      Intersection
      ( const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functors::Difference <TYPE,TYPE2> ( fun1 , fun2 ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Intersection () {}
      /// MANDATORY: clone method ("virtual consturctor")
      virtual  Intersection* clone() const { return new Intersection ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functors::Union<TYPE,TYPE2>::result_type operator() 
        ( typename LoKi::Functors::Union<TYPE,TYPE2>::argument a ) const 
      {
        LoKi::Operations::Intersection<TYPE2> _intersection ;
        return _intersection ( this->fun1 ( a ) , this->fun2 ( a ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " intersection(" << this->func1() 
                 << ","              << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Intersection () ;                  // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    template <class TYPE2> 
    class Intersection<void,TYPE2> : public LoKi::Functors::Difference <void,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      Intersection
      ( const LoKi::Functor <void,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <void,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functors::Difference  <void,TYPE2> ( fun1 , fun2 ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Intersection () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Intersection* clone() const { return new Intersection ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functors::Union<void,TYPE2>::result_type operator() 
        ( /* typename LoKi::Functors::Union<void,TYPE2>::argument a */ ) const 
      {
        LoKi::Operations::Intersection<TYPE2> _intersection ;
        return _intersection ( this->fun1 ( /* a */ ) , this->fun2 ( /* a */ ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " intersection(" << this->func1() 
                 << ","              << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Intersection () ;                 // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class SymDifference
     *  simle functor to represent the "symmetric-difference" for two vector-functors
     *  @see LoKi::Operations::SymDifference
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-06
     */
    template <class TYPE, class TYPE2> 
    class SymDifference : public LoKi::Functors::Intersection <TYPE,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      SymDifference
      ( const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functors::Intersection <TYPE,TYPE2> ( fun1 , fun2 ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~SymDifference () {}
      /// MANDATORY: clone method ("virtual consturctor")
      virtual  SymDifference* clone() const { return new SymDifference ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functors::Union<TYPE,TYPE2>::result_type operator() 
        ( typename LoKi::Functors::Union<TYPE,TYPE2>::argument a ) const 
      {
        LoKi::Operations::SymDifference<TYPE2> _symdiff ;
        return _symdiff ( this->fun1 ( a ) , this->fun2 ( a ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " sym_difference(" << this->func1() 
                 << ","                << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SymDifference () ;                 // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    template <class TYPE2> 
    class SymDifference<void,TYPE2> : public LoKi::Functors::Intersection <void,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      SymDifference
      ( const LoKi::Functor  <void,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor  <void,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functors::Intersection <void,TYPE2> ( fun1 , fun2 ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~SymDifference () {}
      /// MANDATORY: clone method ("virtual consturctor")
      virtual  SymDifference* clone() const { return new SymDifference ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functors::Union<void,TYPE2>::result_type operator() 
        ( /* typename LoKi::Functors::Union<void,TYPE2>::argument a */ ) const 
      {
        LoKi::Operations::SymDifference<TYPE2> _symdiff ;
        return _symdiff ( this->fun1 ( /* a */ ) , this->fun2 ( /* a */ ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " sym_difference(" << this->func1() 
                 << ","                << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SymDifference () ;                 // the default constructor is disabled 
      // ======================================================================
    };    
    // ========================================================================
    /** @class Includes
     *  simle functor to represent the "includes" for two vector-functors
     *  @see LoKi::Operations::Includes
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-06
     */
    template <class TYPE, class TYPE2> 
    class Includes : public LoKi::Functor<TYPE,bool>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      Includes 
      ( const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functor<TYPE,bool> () 
        , m_two  ( fun1 , fun2 )  
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Includes () {}
      /// MANDATORY: clone method ("virtual consturctor")
      virtual  Includes* clone() const { return new Includes ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functor<TYPE,bool>::result_type operator() 
        ( typename LoKi::Functor<TYPE,bool>::argument a ) const 
      {
        LoKi::Operations::Includes<TYPE2> _includes ;
        return _includes( m_two.fun1 ( a ) , m_two.fun2 ( a ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " includes(" << m_two.func1() 
                 << ","          << m_two.func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Includes () ;                      // the default constructor is disabled 
      // ======================================================================
    private:                                 
      // ======================================================================
      /// storage of two functors 
      LoKi::TwoFunctors <TYPE,std::vector<TYPE2> > m_two ;      // two functors 
      // ======================================================================
    };
    // ========================================================================
    template <class TYPE2> 
    class Includes<void,TYPE2> : public LoKi::Functor<void,bool>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      Includes 
      ( const LoKi::Functor <void,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <void,std::vector<TYPE2> >& fun2 ) 
        : LoKi::Functor     <void,bool> () 
        , m_two  ( fun1 , fun2 )  
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Includes () {}
      /// MANDATORY: clone method ("virtual consturctor")
      virtual  Includes* clone() const { return new Includes ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functor<void,bool>::result_type operator() 
        ( /* typename LoKi::Functor<void,bool>::argument a */ ) const 
      {
        LoKi::Operations::Includes<TYPE2> _includes ;
        return _includes( m_two.fun1 ( /* a */ ) , m_two.fun2 ( /* a */ ) ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " includes(" << m_two.func1() 
                 << ","          << m_two.func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Includes () ;                      // the default constructor is disabled 
      // ======================================================================
    private:                                 
      // ======================================================================
      /// storage of two functors 
      LoKi::TwoFunctors <void,std::vector<TYPE2> > m_two ;      // two functors 
      // ======================================================================
    };
    // ========================================================================
    /** @class Unique 
     *  simple function to remove duplicated elemens from the stream 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-05
     */
    template <class TYPE> 
    class Unique : public LoKi::Functor< std::vector<TYPE>, 
                                         std::vector<TYPE> >  
    {
    public :
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Unique() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Unique* clone() const { return new Unique ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> >::result_type 
      operator() 
        ( typename LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> >::argument a ) const 
      {
        LoKi::Operations::Unique<TYPE> _unique ;
        return _unique ( a ) ;
      }
      // ======================================================================
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                          end of namespace LoKi::Functors  
  // ==========================================================================
  /** simple "filter" function
   *
   *  @code 
   *  
   *   // some selection criteria (predicate) 
   *   const LoKi::BasicFunctors<TYPE2>::Function& cut = ...; 
   *
   *   // input data 
   *   std::vector<TYPE> in = ... ;
   *
   *   // get the filtered results:
   *   std::vector<TYPE> out = in >> filter<TYPE> ( cut ) ;
   *
   *  @endcode 
   *
   *  @see LoKi::Functors::Select
   *
   *  The concept belongs to the Gerhard "The Great" Raven.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28
   */      
  template <class TYPE, class TYPE2> 
  inline
  LoKi::Functors::Select<TYPE,TYPE2>
  filter ( const LoKi::Functor<TYPE2,bool>& cut ) 
  {
    return LoKi::Functors::Select<TYPE,TYPE2>( cut ) ;
  }
  // ==========================================================================
  /** simple "select/filter" function
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some cut:
   *   const LoKi::BasicFunctors<TYPE2>::Function& cut = ...; 
   *
   *   // get the filtered results:
   *   std::vector<TYPE> out = in >> select<TYPE> ( cun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28
   */      
  template <class TYPE, class TYPE2> 
  inline 
  LoKi::Functors::Select<TYPE,TYPE2>
  select ( const LoKi::Functor<TYPE2,bool>& cut ) 
  {
    return LoKi::Functors::Select<TYPE,TYPE2> ( cut ) ;
  }
  // ==========================================================================
  /** simple "yield/map" function 
   *
   *  @code 
   *
   *   const LoKi::Functor<LHCb::Track,double>& pt = ... ;
   *
   *   std::vector<LHCb::Track*> input = ... ;
   * 
   *   std::vector<double> results = input >> yields<LHCb::Track*>( pt )  ;
   *
   *  @endcode 
   * 
   *  The concept belongs to the Gerhard "The Great" Raven and the 
   *  name comes from Marcel Merk.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-02
   */
  template <class TYPE,class TYPE1,class TYPE2> 
  inline 
  LoKi::Functors::Yields<TYPE,TYPE1,TYPE2>
  yields ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::Yields<TYPE,TYPE1,TYPE2> ( fun ) ;  
  }
  // ==========================================================================
  /** simple "tee" function:
   *
   *  @code
   *
   *   // get some functor for plotting the TrPT 
   *   AIDA::IHistogram1D* histo = ...  
   *   FunctorFromFunctor<LHCb::Track,double> plotter = monitor ( TrPT , histo )  ;
   *
   *   // get some functor for selection of track with maximal pt 
   *   MaxElement<LHCb::Track*>  maxPt ( TrPT ) ;
   *
   *   // container of input tracks:
   *   std::vector<LHCb::Track*> input = ... ;
   * 
   *   // "tee"
   *   input >> tee ( compose ( maxPt , plotter ) ) ;   
   *  
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28
   */
  template <class TYPE,class TYPE2>
  inline 
  LoKi::Functors::Tee<TYPE,TYPE2>
  tee ( const LoKi::Functor<std::vector<TYPE>,TYPE2>& fun ) 
  {
    return LoKi::Functors::Tee<TYPE,TYPE2> ( fun ) ;
  }
  // ==========================================================================
  /** simple "min_value" function:
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some function
   *   const LoKi::BasicFunctors<TYPE>::Function& fun = ...; 
   *
   *   // get the minimal value of the function
   *   double m = in >> min_value<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28
   */
  template <class TYPE, class TYPE2, class TYPE1>
  inline 
  LoKi::Functors::Min<TYPE,TYPE2,TYPE1>
  min_value ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::Min<TYPE,TYPE2,TYPE1>( fun ) ;
  }
  // ==========================================================================
  template <class TYPE, class TYPE2, class TYPE1>
  inline 
  LoKi::Functors::Min<TYPE,TYPE2,TYPE1>
  min_value ( const LoKi::Functor<TYPE2,TYPE1>& fun , 
              const TYPE1&                      val )
  {
    return LoKi::Functors::Min<TYPE,TYPE2,TYPE1>( fun , val ) ;
  }
  // ==========================================================================
  /** simple "max_value" function:
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some function
   *   const LoKi::BasicFunctors<TYPE>::Function& fun = ...; 
   *
   *   // get the maxima;l value of the function
   *   double m = in >> max_value<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28
   */
  template <class TYPE, class TYPE2, class TYPE1>
  inline 
  LoKi::Functors::Max<TYPE,TYPE2,TYPE1>
  max_value ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::Max<TYPE,TYPE2,TYPE1>( fun ) ;
  }
  // ==========================================================================
  template <class TYPE, class TYPE2, class TYPE1>
  inline 
  LoKi::Functors::Max<TYPE,TYPE2,TYPE1>
  max_value ( const LoKi::Functor<TYPE2,TYPE1>& fun , 
              const TYPE1&                      val ) 
  {
    return LoKi::Functors::Max<TYPE,TYPE2,TYPE1>( fun , val ) ;
  }
  // ==========================================================================
  /** simple "min_abs_value" function:
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some function
   *   const LoKi::BasicFunctors<TYPE>::Function& fun = ...; 
   *
   *   // get the minimal value of the function
   *   double m = in >> min_abs_value<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28
   */
  template <class TYPE, class TYPE2, class TYPE1>
  inline 
  LoKi::Functors::AbsMin<TYPE,TYPE2,TYPE1>
  min_abs_value ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::AbsMin<TYPE,TYPE2,TYPE1>( fun ) ;
  }
  // ==========================================================================
  template <class TYPE, class TYPE2, class TYPE1>
  inline 
  LoKi::Functors::AbsMin<TYPE,TYPE2,TYPE1>
  min_abs_value ( const LoKi::Functor<TYPE2,TYPE1>& fun , 
                  const TYPE1&                      val ) 
  {
    return LoKi::Functors::AbsMin<TYPE,TYPE2,TYPE1>( fun , val ) ;
  }
  // ==========================================================================
  /** simple "max_abs_value" function:
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some function
   *   const LoKi::BasicFunctors<TYPE>::Function& fun = ...; 
   *
   *   // get the maximal value of the function
   *   double m = in >> max_abs_value<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28
   */
  template <class TYPE, class TYPE2, class TYPE1>
  inline 
  LoKi::Functors::AbsMax<TYPE,TYPE2,TYPE1>
  max_abs_value ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::AbsMax<TYPE,TYPE2,TYPE1>( fun ) ;
  }
  // ==========================================================================
  template <class TYPE, class TYPE2, class TYPE1>
  inline 
  LoKi::Functors::AbsMax<TYPE,TYPE2,TYPE1>
  max_abs_value ( const LoKi::Functor<TYPE2,TYPE1>& fun , 
                  const TYPE1&                      val ) 
  {
    return LoKi::Functors::AbsMax<TYPE,TYPE2,TYPE1>( fun , val ) ;
  }
  // ==========================================================================  
  /** simple "process" function 
   *
   *  @code
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some action
   *   const LoKi::Functor<TYPE2,TYPE3>& fun = ...; 
   *
   *   // get the processed results (indentical to the input)
   *   in >> process<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven. 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28   
   */      
  template <class TYPE,class TYPE1,class TYPE2>
  inline 
  LoKi::Functors::Process<TYPE,TYPE1,TYPE2>
  process ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::Process<TYPE,TYPE1,TYPE2>( fun ) ;
  }
  // ==========================================================================  
  /** simple "for_each/process" function 
   *
   *  @code
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some action
   *   const LoKi::Functor<TYPE2,TYPE3>& fun = ...; 
   *
   *   // get the processed results (indentical to the input)
   *   in >> for_each<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven. 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28   
   */      
  template <class TYPE,class TYPE1,class TYPE2>
  inline 
  LoKi::Functors::Process<TYPE,TYPE1,TYPE2>
  for_each ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::Process<TYPE,TYPE1,TYPE2>( fun ) ;
  }
  // ==========================================================================  
  /** simple 'max_element' function 
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some function
   *   const LoKi::BasicFunctors<TYPE>::Function& fun = ...; 
   *
   *   // get the maximal element
   *   TYPE m = in >> max_element<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven. 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28   
   */      
  template <class TYPE,class TYPE2,class TYPE1>
  inline 
  LoKi::Functors::MaxElement<TYPE,TYPE2,TYPE1>
  max_element ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::MaxElement<TYPE,TYPE2,TYPE1>( fun ) ;
  }
  // ==========================================================================  
  /** simple 'min_element' function 
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some function
   *   const LoKi::BasicFunctors<TYPE>::Function& fun = ...; 
   *
   *   // get the minimal element
   *   TYPE m = in >> min_element<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven. 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28   
   */      
  template <class TYPE,class TYPE2,class TYPE1>
  inline 
  LoKi::Functors::MinElement<TYPE,TYPE2,TYPE1>
  min_element ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::MinElement<TYPE,TYPE2,TYPE1>( fun ) ;
  }
  // ==========================================================================  
  /** simple 'max_abs_element' function 
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some function
   *   const LoKi::BasicFunctors<TYPE>::Function& fun = ...; 
   *
   *   // get the maximal element
   *   TYPE m = in >> max_abs_element<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven. 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28   
   */      
  template <class TYPE,class TYPE2,class TYPE1>
  inline 
  LoKi::Functors::AbsMaxElement<TYPE,TYPE2,TYPE1>
  max_abs_element ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::AbsMaxElement<TYPE,TYPE2,TYPE1>( fun ) ;
  }
  // ==========================================================================  
  /** simple 'min_abs_element' function 
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some function
   *   const LoKi::BasicFunctors<TYPE>::Function& fun = ...; 
   *
   *   // get the minimal element
   *   TYPE m = in >> min_abs_element<TYPE> ( fun ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven. 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28   
   */      
  template <class TYPE,class TYPE2,class TYPE1>
  inline 
  LoKi::Functors::AbsMinElement<TYPE,TYPE2,TYPE1>
  min_abs_element ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
  {
    return LoKi::Functors::AbsMinElement<TYPE,TYPE2,TYPE1>( fun ) ;
  }
  // ==========================================================================
  /**  simple "count" function
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some predicate
   *   const LoKi::BasicFunctors<TYPE>::Predicate& cut = ...; 
   *
   *   // count the number of elements
   *   in >> count<TYPE>( cut ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven. 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-10-17   
   */      
  template <class TYPE,class TYPE1,class TYPE2>
  inline 
  LoKi::Functors::Count<TYPE,TYPE1,TYPE2>
  count ( const LoKi::Functor<TYPE1,TYPE2>& cut ) 
  {
    return LoKi::Functors::Count<TYPE,TYPE1,TYPE2>( cut ) ;
  }
  // ==========================================================================
  /**  simple "has" function
   *
   *  @code 
   *  
   *   std::vector<TYPE> in = ... ;
   *
   *   // some predicate
   *   const LoKi::BasicFunctors<TYPE>::Predicate& cut = ...; 
   *
   *   // check the elements
   *   in >> has<TYPE>( cut ) ;
   *
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven. 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-10-17   
   */      
  template <class TYPE,class TYPE1,class TYPE2>
  inline 
  LoKi::Functors::Has<TYPE,TYPE1,TYPE2>
  has ( const LoKi::Functor<TYPE1,TYPE2>& cut ) 
  {
    return LoKi::Functors::Has<TYPE,TYPE1,TYPE2>( cut ) ;
  }
  // ==========================================================================
  /** simple function to produce the "union" for two streamers 
   *
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-06-05   
   */      
  template <class TYPE, class TYPE2>
  inline 
  LoKi::Functors::Union<TYPE,TYPE2>
  union_ ( const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun1 , 
           const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun2 ) 
  {
    return LoKi::Functors::Union<TYPE,TYPE2> ( fun1 , fun2 ) ;
  }
  // ==========================================================================
  template <class TYPE>
  inline 
  LoKi::Functors::Union< std::vector<TYPE>,TYPE>
  union_ ( const LoKi::Functor<TYPE,bool>&                            fun1 , 
           const LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> >& fun2 ) 
  {
    return LoKi::Functors::Union< std::vector<TYPE>,TYPE>
      ( LoKi::Functors::Select<TYPE,TYPE> ( fun1 ) , fun2 ) ;
  }
  // ==========================================================================
  template <class TYPE>
  inline 
  LoKi::Functors::Union< std::vector<TYPE>,TYPE>
  union_ ( const LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> >& fun1 ,
           const LoKi::Functor<TYPE,bool>&                            fun2 ) 
  {
    return LoKi::Functors::Union< std::vector<TYPE>,TYPE>
      ( fun1 , LoKi::Functors::Select<TYPE,TYPE> ( fun2 ) ) ;
  }
  // ==========================================================================
  template <class TYPE>
  inline 
  LoKi::Functors::Union< std::vector<TYPE>,TYPE>
  union_ ( const LoKi::Functor<TYPE,bool>& fun1 ,
           const LoKi::Functor<TYPE,bool>& fun2 ) 
  {
    return LoKi::Functors::Union< std::vector<TYPE>,TYPE>
      ( LoKi::Functors::Select<TYPE,TYPE> ( fun1 ) , 
        LoKi::Functors::Select<TYPE,TYPE> ( fun2 ) ) ;
  }
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================     
// the specific printpout
// ============================================================================     
template <class TYPE>
std::ostream& LoKi::Functors::Empty<TYPE>::fillStream
( std::ostream& s ) const { return s << "empty_" ; }
// ============================================================================     
// the specific printpout
// ============================================================================     
template <class TYPE>
std::ostream& LoKi::Functors::Size<TYPE>::fillStream
( std::ostream& s ) const { return s << "size_" ; }
// ============================================================================     
// the specific printpout
// ============================================================================     
template <class TYPE>
std::ostream& LoKi::Functors::Unique<TYPE>::fillStream
( std::ostream& s ) const { return s << "unique_" ; }
// ============================================================================     

// ============================================================================     
// The END 
// ============================================================================     
#endif // LOKI_FILTERS_H
// ============================================================================     
