// $Id: Filters.h,v 1.2 2007-12-03 12:03:22 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_FILTERS_H 
#define LOKI_FILTERS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD &STL
// ============================================================================
#include <algorithm>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functor.h"
#include "LoKi/Functions.h"
#include "LoKi/select.h"
#include "LoKi/Algs.h"
#include "LoKi/StdMath.h"
// ============================================================================
namespace LoKi 
{
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
      /// the base class 
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > uBase    ;
    public:
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
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Select* clone() const { return new Select ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typename uBase::result_type out ;
        out.reserve ( a.size() ) ;
        LoKi::apply_filter 
          ( a.begin() , a.end() , m_predicate.fun() , std::back_inserter ( out ) ) ;
        return out ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "select(" << m_predicate << ")" ; }
    private:
      // the default construct is private:
      Select () ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,bool> m_predicate ; // the functor itself 
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
      /// the underlying basic functor 
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE1> > uBase    ;
    public:
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
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Yields* clone() const { return new Yields ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const 
      {
        typename uBase::result_type out ;
        out.reserve ( a.size () ) ;
        LoKi::apply 
          ( a.begin() , a.end() , m_functor.fun() , std::back_inserter ( out ) ) ;
        return out ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "yields(" << m_functor << ")"; };
    private:
      // the default construct is private:
      Yields() ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; // the functor itself 
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
      /// the underlying basic functor 
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > uBase    ;
    public:
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
        LoKi::apply ( a.begin() , a.end() , m_functor.fun() ) ;
        return a ;        
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "process(" << m_functor << ")"; };
    private:
      // the default construct is private:
      Process() ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; // the functor itself 
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
      /// the underlying basic functor 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE2>              uFunctor ;
      typedef LoKi::Functor<std::vector<TYPE>,std::vector<TYPE> > uBase    ;
    public:
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
        LoKi::apply ( m_functor.fun() , a ) ;
        return out ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "tee(" << m_functor << ")"; };
    private:
      // the default construct is private:
      Tee () ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<std::vector<TYPE>,TYPE2> m_functor ; ///< the functor 
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
    protected:
      // the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE1> uBase    ; ///< the base 
      // the base 
      typedef LoKi::Functor<TYPE2,TYPE1> uFunctor    ; ///< the functor
    public:
      /// constructor 
      Max ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      Max ( const Max& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Max() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Max* clone() const { return new Max ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.fun() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin() , a.end() , app , std::greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return res.second ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "max(" << m_functor << ")"; } ;
    private:
      // the default construct is private:
      Max () ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; ///< the functor
    } ;
    // ========================================================================
    /** @class AbsMax 
     *  simple meta-functor to find the maximum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class AbsMax : public LoKi::Functor<std::vector<TYPE>,TYPE1> 
    {
    protected:
      // the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE1> uBase    ; ///< the base 
      // the base 
      typedef LoKi::Functor<TYPE2,TYPE1> uFunctor    ; ///< the functor
    public:
      /// constructor 
      AbsMax ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      AbsMax ( const AbsMax& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~AbsMax() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsMax* clone() const { return new AbsMax ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.fun() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin() , a.end() , app , 
                                 LoKi::StdMath::abs_greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return res.second ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "max_abs(" << m_functor << ")"; } ;
    private:
      // the default construct is private:
      AbsMax () ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; ///< the functor
    } ;
    // ========================================================================
    /** @class Min
     *  simple meta-functor to find the minimum 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class Min : public LoKi::Functor<std::vector<TYPE>,TYPE1> 
    {
    protected:
      // the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE1> uBase    ; ///< the base 
      // the base 
      typedef LoKi::Functor<TYPE2,TYPE1> uFunctor    ; ///< the functor
    public:
      /// constructor 
      Min ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      Min ( const Min& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Min() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Min* clone() const { return new Min ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.fun() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin() , a.end() , app , std::less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return res.second ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "min_value(" << m_functor << ")"; } ;
    private:
      // the default construct is private:
      Min () ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; ///< the functor
    } ;
    // ========================================================================
    /** @class AbsMin
     *  simple meta-functor to find the minimum 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class AbsMin : public LoKi::Functor<std::vector<TYPE>,TYPE1> 
    {
    protected:
      // the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE1> uBase    ; ///< the base 
      // the base 
      typedef LoKi::Functor<TYPE2,TYPE1> uFunctor    ; ///< the functor
    public:
      /// constructor 
      AbsMin ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      AbsMin ( const AbsMin& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~AbsMin() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsMin* clone() const { return new AbsMin ( *this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        typedef typename std::vector<TYPE>::const_iterator     _I ;
        LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.fun() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin() , a.end() , app , LoKi::StdMath::abs_less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return res.second ;
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "min_abs_value(" << m_functor << ")"; } ;
    private:
      // the default construct is private:
      AbsMin () ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; ///< the functor
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
      // the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE>  uBase    ; ///< the base 
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; ///< the functor 
    public:
      /// constructor 
      MaxElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      MaxElement ( const MaxElement& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
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
        const LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.fun() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin() , a.end() , app , std::greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "max_element(" << m_functor << ")"; } ;
    private:
      // the default construct is private:
      MaxElement() ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; ///< the functor
    } ;
    // ========================================================================
    /** @class MinElement
     *  simple meta-functor to find the minimal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class MinElement : public LoKi::Functor<std::vector<TYPE>,TYPE> 
    {
    public:
      // the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE>  uBase    ; ///< the base 
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; ///< the functor 
    public:
      /// constructor 
      MinElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      MinElement ( const MinElement& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
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
        const LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.fun() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum ( a.begin() , a.end() , app , std::less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "min_element(" << m_functor << ")"; } ;
    private:
      // the default construct is private:
      MinElement() ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; ///< the functor
    } ;
    // ========================================================================
    /** @class AbsMaxElement 
     *  simple meta-functor to find the maximal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class AbsMaxElement : public LoKi::Functor<std::vector<TYPE>,TYPE> 
    {
    public:
      // the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE>  uBase    ; ///< the base 
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; ///< the functor 
    public:
      /// constructor 
      AbsMaxElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      AbsMaxElement ( const AbsMaxElement& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
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
        const LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.fun() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin() , a.end() , app , LoKi::StdMath::abs_greater<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "max_abs_element(" << m_functor << ")"; } ;
    private:
      // the default construct is private:
      AbsMaxElement() ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; ///< the functor
    } ;
    // ========================================================================
    /** @class AbsMinElement
     *  simple meta-functor to find the minimal element 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-26
     */
    template <class TYPE,class TYPE2=TYPE,class TYPE1=double>
    class AbsMinElement : public LoKi::Functor<std::vector<TYPE>,TYPE> 
    {
    public:
      // the base 
      typedef LoKi::Functor<std::vector<TYPE>,TYPE>  uBase    ; ///< the base 
      typedef LoKi::Functor<TYPE2,TYPE1>              uFunctor ; ///< the functor 
    public:
      /// constructor 
      AbsMinElement ( const LoKi::Functor<TYPE2,TYPE1>& fun ) 
        : uBase     (     ) 
        , m_functor ( fun ) 
      {}
      /// copy constructor 
      AbsMinElement ( const AbsMinElement& right  ) 
        : LoKi::AuxFunBase ( right ) 
        , uBase            ( right ) 
        , m_functor        ( right.m_functor ) 
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
        const LoKi::Apply<TYPE2,TYPE1> app ( &m_functor.fun() ) ;
        std::pair<_I,typename uFunctor::result_type> res = 
          LoKi::Algs::extremum 
          ( a.begin() , a.end() , app , LoKi::StdMath::abs_less<TYPE1>() ) ;
        Assert ( a.end() != res.first , "Empty input container!" ) ;
        return *(res.first);
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return  s << "min_abs_element(" << m_functor << ")"; } ;
    private:
      // the default construct is private:
      AbsMinElement() ; ///< no default contructor
    private:
      // the basic functor itself 
      LoKi::FunctorFromFunctor<TYPE2,TYPE1> m_functor ; ///< the functor
    } ;
    // ========================================================================
  } // end of namespace LoKi::Filters 
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
} // end of namespace LoKi 
// ============================================================================     
// The END 
// ============================================================================     
#endif // LOKI_FILTERS_H
// ============================================================================     
