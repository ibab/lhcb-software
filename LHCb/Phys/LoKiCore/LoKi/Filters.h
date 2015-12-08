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
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LHCb
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functor.h"
#include "LoKi/Functions.h"
#include "LoKi/Algs.h"
#include "LoKi/Primitives.h"
#include "LoKi/Dump.h"
#include "LoKi/FirstN.h"
#include "LoKi/Timers.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
#include "boost/type_traits/is_pointer.hpp"
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
 *  @date 2007-10-30
 */
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
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , uBase       (     ) 
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
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , uBase       (     ) 
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
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , uBase     (     ) 
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
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , uBase     (     ) 
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
      Max ( const LoKi::Functor<TYPE2,TYPE1>& fun , 
            const TYPE1 val = -std::numeric_limits<TYPE1>::max() )
        : LoKi::AuxFunBase ( std::tie ( fun , val ) ) 
        , uBase     (     ) 
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
      ( const LoKi::Functor<TYPE2,TYPE1>& fun      , 
        const TYPE1                       val = -1 )
        : LoKi::AuxFunBase ( std::tie ( fun , val ) ) 
        , LoKi::Functors::Max<TYPE,TYPE2,TYPE1>  ( fun , val ) 
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
        if ( -1 != this->val() ) { s << "," << this->val() ; }
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
      Min ( const LoKi::Functor<TYPE2,TYPE1>& fun , 
            const TYPE1 val = std::numeric_limits<TYPE1>::max() )
        : LoKi::AuxFunBase ( std::tie ( fun , val ) ) 
        , LoKi::Functors::AbsMax<TYPE,TYPE2,TYPE1> ( fun , val )  
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
        : LoKi::AuxFunBase ( std::tie ( fun , val ) ) 
        , LoKi::Functors::Min<TYPE,TYPE2,TYPE1> ( fun , val ) 
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
    class MaxElement : public LoKi::BasicFunctors<TYPE>::Pipe 
    {
    public:
      // ======================================================================
      /// the base 
      typedef typename LoKi::BasicFunctors<TYPE>::Pipe uBase    ; //   the base
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>               uFunctor ; //the functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      MaxElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , uBase     (     ) 
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
        if ( a.empty() ){  return a ; }
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.func () ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            std::greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return typename uBase::result_type( 1 , *(res.first) ) ;
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
      typedef typename LoKi::BasicFunctors<TYPE>::Pipe uBase    ; //   the base
      typedef LoKi::Functor<TYPE2,TYPE1>             uFunctor ;     //  functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      MinElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , LoKi::Functors::MaxElement<TYPE,TYPE2,TYPE1> ( fun ) 
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
        if ( a.empty() ) { return a ; }
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin () , 
                                 a.end   () , 
                                 app        , 
                                 std::less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return typename uBase::result_type( 1 , *(res.first) ) ;
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
      typedef typename LoKi::BasicFunctors<TYPE>::Pipe uBase    ; //   the base
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>             uFunctor ; //  the functor
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      AbsMaxElement ( const LoKi::Functor<TYPE2,TYPE1>& fun )
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , LoKi::Functors::MinElement<TYPE,TYPE2,TYPE1> ( fun ) 
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
        if ( a.empty() ) { return a ; }
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func () ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            LHCb::Math::abs_greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return typename uBase::result_type( 1 , *(res.first) ) ;
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
      typedef typename LoKi::BasicFunctors<TYPE>::Pipe uBase    ; //   the base
      /// the functor
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; // the functor 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      AbsMinElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : LoKi::AuxFunBase ( std::tie ( fun ) ) 
        , LoKi::Functors::AbsMaxElement<TYPE,TYPE2,TYPE1> ( fun ) 
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
        if ( a.empty() ) { return a ; }
        //
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        const LoKi::Apply<TYPE2,TYPE1> app ( &this->func() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin () , 
            a.end   () , 
            app        , 
            LHCb::Math::abs_less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return typename uBase::result_type( 1 , *(res.first) ) ;
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
        : LoKi::AuxFunBase ( std::tie ( cut ) ) 
        , LoKi::Functor<std::vector<TYPE>,double> () 
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
    /** @class Sum 
     *  sum of the function over the stream 
     *  @see LoKi::Algs::accumulate 
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-02-27
     */
    template <class TYPE>
    class Sum : public LoKi::Functor<std::vector<TYPE>,double> 
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,double>  uBase    ; // the base 
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from the function
       *  @param fun the function 
       *  @param init the initial value for summation 
       */
      Sum ( const LoKi::Functor<TYPE,double>& fun      , 
            const double                      init = 0 ) 
        : LoKi::AuxFunBase ( std::tie ( fun , init ) ) 
        , LoKi::Functor<std::vector<TYPE>,double> () 
        , m_fun      ( fun  ) 
        , m_cut      ( LoKi::Constant<TYPE,bool>( true ) )
        , m_trivCut  ( true ) 
        , m_init     ( init ) 
      {}
      /** constructor from the function
       *  @param fun the function 
       *  @param init the initial value for summation 
       */
      Sum ( const LoKi::Functor<TYPE,double>& fun      , 
            const LoKi::Functor<TYPE,bool>&   cut      , 
            const double                      init = 0 ) 
        : LoKi::AuxFunBase ( std::tie ( fun , cut , init ) ) 
        , LoKi::Functor<std::vector<TYPE>,double> () 
        , m_fun      ( fun   ) 
        , m_cut      ( cut   )
        , m_trivCut  ( false ) 
        , m_init     ( init  ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Sum () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Sum* clone() const { return new Sum ( *this ) ; }
      /// MANDATORY: the only one essential method:
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      { return this -> _accumulate_ ( a , std::plus<double>() ) ; }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return this -> _print_      ( s , "sum" , 0 ) ; }
      // ======================================================================
    protected : 
      // ======================================================================
      /// 
      template <class OPERATION>
      typename uBase::result_type _accumulate_ 
      ( typename uBase::argument a , OPERATION operation ) const
      {
        const LoKi::Apply<TYPE,double> appFun ( &m_fun.func() ) ;
        const LoKi::Apply<TYPE,bool>   appCut ( &m_cut.func() ) ;
        //
        return m_trivCut ? 
          LoKi::Algs::accumulate
          ( a.begin ()   , 
            a.end   ()   , 
            appFun       , 
            this->m_init , 
            operation    ) :
          LoKi::Algs::accumulate
          ( a.begin ()   , 
            a.end   ()   , 
            appFun       , 
            appCut       , 
            this->m_init , 
            operation    ) ;
      }
      // ======================================================================
      /// OPTIONAL: the basic printout method 
      std::ostream& _print_ ( std::ostream&      s , 
                              const std::string& n ,
                              const double       d ) const 
      { 
        s << n << "(" << this->m_fun ;
        if ( !this->m_trivCut  ) { s << "." << this -> m_cut  ; }
        if ( d != this->m_init ) { s << "," << this -> m_init ; }
        return s << ")" ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Sum () ;                              // default constructor is disabled 
      // ======================================================================      
    protected:
      // ======================================================================
      /// the function 
      LoKi::FunctorFromFunctor<TYPE,double> m_fun     ; // the function 
      LoKi::FunctorFromFunctor<TYPE,bool>   m_cut     ; // the cut 
      /// trivial cut ?
      bool                                  m_trivCut ; // trivial cut ?
      double                                m_init    ; // init-value 
      // ======================================================================
    };
    // ========================================================================
    /** @class Product
     *  product of the function over the stream 
     *  @see LoKi::Algs::accumulate 
     *  @see LoKi::Functors::Sum
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-02-27
     */
    template <class TYPE>
    class Product : public LoKi::Functors::Sum<TYPE>
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,double>  uBase    ; // the base 
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the function 
      Product ( const LoKi::Functor<TYPE,double>& fun      , 
                const double                      init = 1 ) 
        : LoKi::AuxFunBase ( std::tie ( fun , init ) ) 
        , LoKi::Functors::Sum <TYPE> ( fun , init ) 
      {}
      /// constructor from the function and  predicate 
      Product ( const LoKi::Functor<TYPE,double>& fun      , 
                const LoKi::Functor<TYPE,bool>&   cut      , 
                const double                      init = 1 ) 
        : LoKi::AuxFunBase ( std::tie ( fun , cut , init ) ) 
        , LoKi::Functors::Sum <TYPE> ( fun , cut , init ) 
      {}      
      /// MANDATORY: virtual destructor 
      virtual ~Product () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Product* clone() const { return new Product ( *this ) ; }
      /// MANDATORY: the only one essential method:
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      { return this -> _accumulate_ ( a, std::multiplies<double>() ) ; }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return this -> _print_      ( s , "product" , 1 ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Product () ;                           // default constructor is disabled 
      // ======================================================================      
    };
    // ========================================================================
    /** @class Fetch
     *  Fetch & evaluate the certain element of the stream 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2011-03-27
     */
    template <class TYPE,class TYPE2>
    class Fetch : public LoKi::Functor<std::vector<TYPE>,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor from functor, index and bad-value 
      Fetch  ( const LoKi::Functor<TYPE,TYPE2>& fun   , 
               const unsigned int               index ,
               const TYPE2                      bad   ) 
        : LoKi::AuxFunBase ( std::tie ( fun , index , bad ) ) 
        , LoKi::Functor<std::vector<TYPE>,TYPE2> () 
        , m_fun   ( fun   ) 
        , m_index ( index )
        , m_bad   ( bad   ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Fetch() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Fetch* clone() const { return new Fetch ( *this ) ; }
      /// MANDATORY: 
      /// MANDATORY: the only one essential method:
      virtual typename LoKi::Functor<std::vector<TYPE>,TYPE2>::result_type operator()
        ( typename LoKi::Functor<std::vector<TYPE>,TYPE2>::argument a ) const
      {
        if ( a.size() < m_index + 1 ) 
        {
          this->Error ( "Input vector is too short, return 'bad'" ) ;
          return this->m_bad ;
        }
        // evaluate the functor:
        return this->m_fun.fun ( a[ this->m_index] ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Fetch () ;                         // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the functor itself 
      typename LoKi::Assignable<LoKi::Functor<TYPE,TYPE2> >::Type m_fun ;
      /// the index 
      unsigned int m_index ;  // the index 
      /// bad value 
      TYPE2        m_bad   ;  // the bad value 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Has
     *  check the presence of good elemements in stream 
     */
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
        : LoKi::AuxFunBase ( std::tie ( cut ) ) 
        , LoKi::Functor<std::vector<TYPE>,bool> () 
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
     *  Simple functor which checks the emptiness of the container 
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functor     <TYPE,std::vector<TYPE2> > () 
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functor     <void,std::vector<TYPE2> > () 
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
    /** @class NoEmptyUnion
     *  simple functor to represent the "union" for two NON-empty vector-functors
     *  Result is empty if one of inputs is empty 
     *  @see LoKi::Operations::NoEmptyUnion 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-06
     */
    template <class TYPE, class TYPE2> 
    class NoEmptyUnion : public Union<TYPE,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      NoEmptyUnion 
      ( const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <TYPE,std::vector<TYPE2> >& fun2 ) 
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functors::Union<TYPE,TYPE2> ( fun1 , fun2 ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~NoEmptyUnion() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  NoEmptyUnion* clone() const { return new NoEmptyUnion ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functor<TYPE,std::vector<TYPE2> >::result_type operator() 
      ( typename LoKi::Functor<TYPE,std::vector<TYPE2> >::argument a ) const 
      {
        LoKi::Operations::NoEmptyUnion<TYPE2> _union ;
        //
        typename LoKi::Functor<TYPE,std::vector<TYPE2> >::result_type r1 = this->fun1 ( a ) ;
        if ( r1.empty() ) { return r1 ; }  // RETURN EMPTY 
        typename LoKi::Functor<TYPE,std::vector<TYPE2> >::result_type r2 = this->fun2 ( a ) ;
        if ( r2.empty() ) { return r2 ; }  // RETURN EMPTY        
        return _union ( r1 , r2 ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " no_empty_union( " << this->func1() 
                 << ","                 << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      NoEmptyUnion () ;                         // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    template <class TYPE2> 
    class NoEmptyUnion<void,TYPE2> : public Union<void,TYPE2>
    {
    public:
      // ======================================================================
      /// constructor from two streamers 
      NoEmptyUnion 
      ( const LoKi::Functor <void,std::vector<TYPE2> >& fun1 , 
        const LoKi::Functor <void,std::vector<TYPE2> >& fun2 ) 
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functors::Union<void,TYPE2> ( fun1 , fun2 ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~NoEmptyUnion() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  NoEmptyUnion* clone() const { return new NoEmptyUnion ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual 
      typename LoKi::Functor<void,std::vector<TYPE2> >::result_type operator() 
      ( /* typename LoKi::Functor<void,std::vector<TYPE2> >::argument a */ ) const 
      {
        //
        LoKi::Operations::NoEmptyUnion<TYPE2> _union ;
        //
        typename LoKi::Functor<void,std::vector<TYPE2> >::result_type r1 = this->fun1( /* a */ ) ;
        if ( r1.empty() ) { return r1 ; }  // RETURN EMPTY 
        typename LoKi::Functor<void,std::vector<TYPE2> >::result_type r2 = this->fun2( /* a */ ) ;
        if ( r2.empty() ) { return r2 ; }  // RETURN EMPTY        
        //
        return _union ( r1 , r2 ) ;
      }
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << " union(" << this->func1() 
                 << ","       << this->func2() << ") " ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      NoEmptyUnion () ;                  // the default constructor is disabled 
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functors::Union <TYPE,TYPE2> ( fun1 , fun2 ) 
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
        typename LoKi::Functor <TYPE,std::vector<TYPE2> >::result_type
          r1 = this -> fun1 ( a ) ;
        if ( r1.empty() ) { return r1 ; } // RETURN 
        //
        LoKi::Operations::Difference<TYPE2> _diff ;
        return _diff ( r1  , this->fun2 ( a )  ) ;
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functors::Union <void,TYPE2> ( fun1 , fun2 ) 
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
        typename LoKi::Functor <void,std::vector<TYPE2> >::result_type
          r1 = this -> fun1 ( /* a */ ) ;
        if ( r1.empty() ) { return r1 ; } // RETURN
        //
        LoKi::Operations::Difference<TYPE2> _diff ;
        return _diff ( r1 , this->fun2 ( /* a */ ) ) ;
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functors::Difference <TYPE,TYPE2> ( fun1 , fun2 ) 
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functors::Difference  <void,TYPE2> ( fun1 , fun2 ) 
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functors::Intersection <TYPE,TYPE2> ( fun1 , fun2 ) 
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functors::Intersection <void,TYPE2> ( fun1 , fun2 ) 
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functor<TYPE,bool> () 
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
        : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
        , LoKi::Functor     <void,bool> () 
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
    /** @class Gate
     *  Helper class to control the flow of objects.
     *  Objects flows only if the gate is open.
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-17
     */
    template <class TYPE>
    class Gate : public LoKi::Functor<std::vector<TYPE>  ,
                                      std::vector<TYPE>  >
    {
    private: 
      // ======================================================================
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> >      Pipe_ ;
      typedef typename Pipe_::argument                              argument ;
      typedef typename Pipe_::result_type                        result_type ;
      // ======================================================================
    public:
      // =====================================================================
      /// the constructor from the stopper 
      Gate ( const LoKi::Functor<void,bool>& gate ) 
        : LoKi::AuxFunBase ( std::tie ( gate ) ) 
        , LoKi::Functor<std::vector<TYPE> , std::vector<TYPE> >() 
        , m_gate ( gate )
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Gate() {}
      /// MANDATORY: clone method("virtual constructor")
      virtual  Gate* clone() const { return new Gate ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      {
        // run stopper only for non-empty containers
        // and stop the data flow if needed 
        if ( !a.empty() && !m_gate() ) { result_type() ; }
        return a ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "gate(" << m_gate << ")"; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Gate () ;                         // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual gate
      LoKi::FunctorFromFunctor<void,bool> m_gate ;                  // the gate
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Cause
     *  Helper class for ``conditional source''
     *  @thanks Roel AAIJ
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-18
     */
    template <class TYPE>
    class Cause : public LoKi::Functor<void,std::vector<TYPE> >
    {
    private: 
      // ======================================================================
      typedef LoKi::Functor<void,std::vector<TYPE> >                 Source_ ;
      typedef typename Source_::result_type                      result_type ;
      // ======================================================================
    public:
      // =====================================================================
      /// the constructor from the start & source 
      Cause
      ( const LoKi::Functor< void , bool              >& start  , 
        const LoKi::Functor< void , std::vector<TYPE> >& source ) 
        : LoKi::AuxFunBase ( std::tie ( source , start ) ) 
        , LoKi::Functor< void , std::vector<TYPE> >() 
        , m_start  ( start  ) 
        , m_source ( source ) 
      {}
      /// the constructor from the start & source 
      Cause
      ( const LoKi::Functor< void , std::vector<TYPE> >& source , 
        const LoKi::Functor< void , bool              >& start  ) 
        : LoKi::AuxFunBase ( std::tie ( source , start ) ) 
        , LoKi::Functor< void , std::vector<TYPE> >() 
        , m_start  ( start  ) 
        , m_source ( source ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Cause() {}
      /// MANDATORY: clone method("virtual constructor")
      virtual  Cause* clone() const { return new Cause ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( /* argument a */ ) const 
      {
        // invoke the source if enabled 
        if ( m_start ( /* a */ ) ) { return m_source ( /* a */ ) ; }
        // return empty container 
        return result_type() ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << " cause( "<< m_start << "," << m_source << ") "; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Cause () ;                         // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual starter 
      LoKi::FunctorFromFunctor<void,bool>               m_start ; // the start
      /// the actual source 
      LoKi::FunctorFromFunctor<void,std::vector<TYPE> > m_source; // the source
      // ======================================================================
    } ;
    // ========================================================================
    /** @class FirstN_
     *  get the first N-elements from the stream 
     *  @thanks Roel AAIJ
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-18
     */
    template <class TYPE>
    class FirstN_ : public LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> >
    {
    public:
      // =====================================================================
      /// the constructor
      FirstN_ ( const unsigned int N )
        : LoKi::AuxFunBase ( std::tie ( N ) ) 
        , LoKi::Functor< std::vector<TYPE> , std::vector<TYPE> > ()
        , m_N ( N ) 
      {}
      /// the constructor
      FirstN_ ( const LoKi::FirstN& N  )
        : LoKi::AuxFunBase ( std::tie ( N ) ) 
        , LoKi::Functor< std::vector<TYPE> , std::vector<TYPE> > ()
        , m_N ( N ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~FirstN_ () {}
      /// MANDATORY: clone method("virtual constructor")
      virtual  FirstN_ * clone() const { return new FirstN_ ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> >::result_type 
      operator() ( typename LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> >::argument a ) const 
      {
        //
        if ( a.size() <= m_N.n() ) { return a ; }                 // RETURN 
        //
        return typename LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> >::result_type 
          ( a.begin() , a.begin() + m_N.n() ) ;  
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      FirstN_ () ;                        // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// number of eleemtns to be selected :
      LoKi::FirstN m_N ;                 // number of elements to be selected 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Reverse_
     *  reverse order of elements in the stream 
     *  @thanks Roel AAIJ
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-18
     */
    template <class TYPE>
    class Reverse_ : public LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> >
    {
    public:
      // =========================================================================
      /// the constructor 
      Reverse_ () : LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > () {}
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Reverse_ () {}
      /// MANDATORY: clone method("virtual constructor")
      virtual  Reverse_* clone() const { return new Reverse_ ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> >::result_type 
      operator() ( typename LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> >::argument a ) const 
      {
        typename LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> >::result_type r = a ;
        std::reverse ( r.begin() , r.end() ) ;
        return r ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Sort_
     *  get the first N-element of sorted input sequence
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-02-27
     */
    template < class TYPE               , 
               class TYPE2     = double ,
               bool  ASCENDING = true   >  
    class Sort_ : public LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > 
    {
    protected:
      // ======================================================================
      /// the base class 
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > uBase    ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from the function
       *  @param fun the function 
       *  @param init the initial value for summation 
       */
      Sort_ ( const LoKi::Functor<TYPE,TYPE2>& fun        ,
              const int                        N   = -1   )
        : LoKi::AuxFunBase ( std::tie ( fun , N  ) )  
        , LoKi::Functor< std::vector<TYPE>,std::vector<TYPE> > () 
        , m_cmp  ( fun )
        , m_sort ( N , ASCENDING ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Sort_ () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Sort_* clone() const { return new Sort_ ( *this ) ; }
      /// MANDATORY: the only one essential method:
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        // nothing to select => nothing to sort 
        if ( 0 == this->N() ) { return typename uBase::result_type() ; }
        //
        typename uBase::result_type r = a ;
        if ( this->all() || (unsigned) this->N() >= a.size() ) 
        {
          std::stable_sort ( r.begin () , r.end   () , this->m_cmp ) ;
          return r ;
        }
        //
        std::partial_sort  ( r.begin ()                 , 
                             r.begin () + this -> N()   , 
                             r.end   () , this -> m_cmp ) ;
        return typename uBase::result_type ( r.begin () , 
                                             r.begin () + this -> N () );
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { 
        s << " sort ( " << this->m_cmp.func1() ;
        //
        if      ( ASCENDING && this->m_sort.all() ) { return s << ") " ; }
        else if ( ASCENDING ) 
        { return  s << ", " << this -> N() << ") "        ; }
        //
        return    s << ", " << this -> N() << ", False) " ;
      }
      // ======================================================================
    public:
      // ======================================================================
      int  N   ( ) const { return this->m_sort.N   () ; }
      bool all ( ) const { return this->m_sort.all () ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Sort_ () ;                             // default constructor is disabled 
      // ======================================================================      
    protected:
      // ======================================================================
      /// the comparison criteria 
      typename LoKi::Cmp<TYPE,TYPE2,ASCENDING>::Type  m_cmp ; // the comparison criteria 
      /// sorting options 
      LoKi::Sort m_sort ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Dump_
     *  Helper class to dump the content of the flow 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-17
     */
    template <class TYPE>
    class Dump_ : public LoKi::Functor<std::vector<TYPE> ,
                                       std::vector<TYPE> >
    {
    private: 
      // ======================================================================
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> >      Pipe_ ;
      typedef typename Pipe_::argument                              argument ;
      typedef typename Pipe_::result_type                        result_type ;
      // ======================================================================
    public:
      // =====================================================================
      /// the constructor 
      Dump_ ( const std::string& open   = ""        ,
              const std::string& close  = "\n"      , 
              std::ostream&      stream = std::cout ) 
        : LoKi::AuxFunBase ( std::tie ( open , close ) )  
        , LoKi::Functor<std::vector<TYPE> , std::vector<TYPE> >() 
        , m_stream ( stream )
        , m_dump   ( open , close ) 
      {}
      /// the constructor 
      Dump_ ( std::ostream&      stream        ,
              const std::string& open   = ""   ,
              const std::string& close  = "\n" )  
        : LoKi::AuxFunBase ( std::tie ( open , close ) )  
        , LoKi::Functor<std::vector<TYPE> , std::vector<TYPE> >() 
        , m_stream ( stream )
        , m_dump   ( open , close ) 
      {}
      /// the constructor from the stopper 
      Dump_ ( std::ostream&      stream   ,
              const LoKi::Dump&  dump     )  
        : LoKi::AuxFunBase ( std::tie ( dump ) )  
        , LoKi::Functor<std::vector<TYPE> , std::vector<TYPE> >() 
        , m_stream ( stream )
        , m_dump   ( dump   ) 
      {}
      /// the constructor from the stopper 
      Dump_ ( const LoKi::Dump&  dump               , 
              std::ostream&      stream = std::cout ) 
        : LoKi::AuxFunBase ( std::tie ( dump ) )  
        , LoKi::Functor<std::vector<TYPE> , std::vector<TYPE> >() 
        , m_stream ( stream )
        , m_dump   ( dump   ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Dump_ () {}
      /// MANDATORY: clone method("virtual constructor")
      virtual  Dump_* clone() const { return new Dump_ ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "dump"; }
      // ======================================================================
    private:
      // ======================================================================
      /// the stream 
      std::ostream& m_stream ;                                   // the stream      
      /// open/close 
      LoKi::Dump    m_dump   ;                                   // open/close
      // ======================================================================
    } ;
    // ========================================================================
    // MANDATORY: the only one essential method 
    // ========================================================================
    template <class TYPE>
    inline 
    typename Dump_<TYPE>::result_type 
    Dump_<TYPE>::operator() ( typename Dump_<TYPE>::argument a ) const 
    {
      m_stream << m_dump.open  () ;
      //
      if ( a.size() <= m_dump.nMax() ) 
      {
        Gaudi::Utils::toStream 
          ( a.begin() , a.end() , m_stream , "[ " , " ]" , " ,\n " ) ;
      }
      else 
      {
        Gaudi::Utils::toStream 
          ( a.begin() , a.begin() + m_dump.nMax() , 
            m_stream , "[ " , " , ... ]" , " ,\n " ) ;
      }
      //
      m_stream << m_dump.close () ;
      return a ;
    }
    // ========================================================================
    // MANDATORY: the only one essential method 
    // ========================================================================
    template <>
    Dump_<double>::result_type 
    Dump_<double>::operator() ( Dump_<double>::argument a ) const ;
    // ========================================================================
    /** @class FakeSource
     *  simple "fake" source, useful for variuous debugging, fake streams, etc
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2015-03-12
     */
    template <class TYPE>
    class FakeSource : public LoKi::Functor<void,std::vector<TYPE> >
    {
    private: 
      // ======================================================================
      typedef LoKi::Functor<void,std::vector<TYPE> >                  Source_ ;
      typedef typename Source_::result_type                       result_type ;
      // ======================================================================
    public:
      // =====================================================================
      /// the constructor 
      FakeSource ( const unsigned short n = 0      )
        : LoKi::AuxFunBase ( std::tie ( n ) )  
        , LoKi::Functor<void, std::vector<TYPE> >() 
        , m_n     ( n ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~FakeSource () {}
      /// MANDATORY: clone method("virtual constructor")
      virtual  FakeSource* clone() const { return new FakeSource ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( /* argument a */ ) const 
      { return std::vector<TYPE> ( m_n )  ; }  
      // ======================================================================
    private:
      // ======================================================================
      /// data 
      unsigned short m_n   ;                                     // the length 
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
  template <class TYPE, class TYPE2>
  inline 
  LoKi::Functors::NoEmptyUnion<TYPE,TYPE2>
  no_empty_union ( const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun1 , 
                   const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun2 ) 
  {
    return LoKi::Functors::NoEmptyUnion<TYPE,TYPE2> ( fun1 , fun2 ) ;
  }
  // ==========================================================================
  template <class TYPE, class TYPE2>
  inline 
  LoKi::Functors::NoEmptyUnion<TYPE,TYPE2>
  no_empty_union ( const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun1 , 
                   const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun2 ,
                   const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun3 ) 
  {
    return no_empty_union ( no_empty_union ( fun1 , fun2 ) , fun3 ) ;
  }
  // ==========================================================================
  template <class TYPE, class TYPE2>
  inline 
  LoKi::Functors::NoEmptyUnion<TYPE,TYPE2>
  no_empty_union ( const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun1 , 
                   const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun2 ,
                   const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun4 ,  
                   const LoKi::Functor<TYPE,std::vector<TYPE2> >& fun3 ) 
  {
    return
      no_empty_union ( no_empty_union ( no_empty_union ( fun1 , fun2 ) , fun3 ) , fun4 ) ;
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
  /** simple "gate" function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-11-17
   */
  template <class TYPE>
  LoKi::Functors::Gate<TYPE>
  gate ( const LoKi::Functor<void,bool>& gate ) 
  {
    return LoKi::Functors::Gate<TYPE>( gate );
  }
  // ==========================================================================
  /** conditional source 
   *  @param start the start condition
   *  @param source the actual source 
   *  @thanks Roel AAIJ
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-11-18
   */
  template <class TYPE>
  LoKi::Functors::Cause<TYPE>
  cause ( const LoKi::Functor<void,bool>&               start  , 
          const LoKi::Functor<void,std::vector<TYPE> >& source )
  {
    return LoKi::Functors::Cause<TYPE>( start , source );
  }  
  // ==========================================================================
  /** conditional source 
   *  @param source the actual source 
   *  @param start the start condition
   *  @thanks Roel AAIJ
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-11-18
   */
  template <class TYPE>
  LoKi::Functors::Cause<TYPE>
  cause ( const LoKi::Functor<void,std::vector<TYPE> >& source ,
          const LoKi::Functor<void,bool>&               start  ) 
  {
    return LoKi::Functors::Cause<TYPE>( start , source );
  }  
  // ==========================================================================
  /** sum over the stream 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   */
  template <class TYPE> 
  LoKi::Functors::Sum<TYPE>
  sum   ( const LoKi::Functor<TYPE,double>& fun      , 
          const double                      init = 0 ) 
  {
    return LoKi::Functors::Sum<TYPE>( fun , init ) ;
  }
  // ==========================================================================
  /** sum over good elements of the stream 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   */
  template <class TYPE> 
  LoKi::Functors::Sum<TYPE>
  sum   ( const LoKi::Functor<TYPE,double>& fun      , 
          const LoKi::Functor<TYPE,bool>&   cut      , 
          const double                      init = 0 ) 
  {
    return LoKi::Functors::Sum<TYPE>( fun , cut , init ) ;
  }
  // ==========================================================================
  /** product over the stream 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   */
  template <class TYPE> 
  LoKi::Functors::Product<TYPE>
  product ( const LoKi::Functor<TYPE,double>& fun      , 
            const double                      init = 1 ) 
  {
    return LoKi::Functors::Product<TYPE>( fun , init ) ;
  }
  // ==========================================================================
  /** product over good elements of the stream 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   */
  template <class TYPE> 
  LoKi::Functors::Product<TYPE>
  product ( const LoKi::Functor<TYPE,double>& fun      , 
            const LoKi::Functor<TYPE,bool>&   cut      , 
            const double                      init = 1 ) 
  {
    return LoKi::Functors::Product<TYPE>( fun , cut , init ) ;
  }
  // ==========================================================================
  /** fetch certain element from the vector and evaluate  the function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   */
  template <class TYPE, class TYPE2> 
  LoKi::Functors::Fetch<TYPE,TYPE2>
  fetch ( const LoKi::Functor<TYPE,TYPE2>& fun      , 
          const unsigned int               index    , 
          const TYPE2                      bad      ) 
  {
    return LoKi::Functors::Fetch<TYPE,TYPE2> ( fun , index , bad ) ;
  }
  // ==========================================================================
  /** sort the input sequence 
   *  @see LoKi::Functors::Sort_
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-10-30
   */
  template <class TYPE, class TYPE2>
  LoKi::Functors::Sort_<TYPE,TYPE2> 
  sort ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::Functors::Sort_<TYPE,TYPE2>( fun ) ; }
  // ==========================================================================
  /** sort the sequence and get the first N-elements 
   *  @see LoKi::Functors::Sort_
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-10-30
   */
  template <class TYPE, class TYPE2>
  LoKi::Functors::Sort_<TYPE,TYPE2> 
  sort ( const LoKi::Functor<TYPE,TYPE2>& fun , 
         const int                        N   ) 
  { return LoKi::Functors::Sort_<TYPE,TYPE2>( fun , N ) ; }
  // ==========================================================================
  /** sort the sequence and get the first N-elements 
   *  @see LoKi::Functors::Sort_
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-10-30
   */
  template <class TYPE, class TYPE2>
  LoKi::Functors::Sort_<TYPE,TYPE2> 
  sort ( const int                        N   ,
         const LoKi::Functor<TYPE,TYPE2>& fun )
  { return LoKi::Functors::Sort_<TYPE,TYPE2>( fun , N ) ; }
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================     
// the specific printpout
// ============================================================================     
template <class TYPE>
std::ostream& 
LoKi::Functors::Empty<TYPE>::fillStream
( std::ostream& s ) const { return s << "empty_" ; }
// ============================================================================     
// the specific printpout
// ============================================================================     
template <class TYPE>
std::ostream& 
LoKi::Functors::Size<TYPE>::fillStream
( std::ostream& s ) const { return s << "size_" ; }
// ============================================================================     
// the specific printpout
// ============================================================================     
template <class TYPE>
std::ostream& 
LoKi::Functors::Unique<TYPE>::fillStream
( std::ostream& s ) const { return s << "unique_" ; }
// ============================================================================
// the specific printpout
// ============================================================================     
template <class TYPE>
std::ostream& 
LoKi::Functors::FirstN_<TYPE>::fillStream
( std::ostream& s ) const { return s << m_N ; }
// ============================================================================
// the specific printpout
// ============================================================================     
template <class TYPE>
std::ostream& 
LoKi::Functors::Reverse_<TYPE>::fillStream
( std::ostream& s ) const { return s << "reverse" ; }
// ============================================================================


// ============================================================================     
// The END 
// ============================================================================     
#endif // LOKI_FILTERS_H
// ============================================================================
