// $Id: EFunctions.h,v 1.5 2007-02-14 17:23:51 cattanem Exp $
#ifndef HLTBASE_OPER_H 
#define HLTBASE_OPER_H 1

#include <functional>
#include <algorithm>
#include <ostream>
#include <cmath>

/** Namespace for Hlt template base classes
 *  
 *  @author Jose Angel Hernando Morata
 *          based on LoKi ideas by I. Belyaev
 *  @date   2006-07-18
 */

namespace Estd
{

  /** Template base class to define functions and filters and operations
   *  between them
   *  
   *  @author Jose Angel Hernando Morata
   *          based on LoKi ideas by I. Belyaev
   *  @date   2006-07-18
   */
  
  template <class T>
  class function : public std::unary_function<const T,double> 
  {
  public:
    typedef Estd::function<T> Self;
    typedef typename std::unary_function<const T,double>  Base;
    typedef typename Base::result_type   result_type   ;
    typedef typename Base::argument_type argument_type ;
  public:
    virtual ~function() {}
    virtual double operator() (const T& t) const = 0;
    virtual Self* clone() const  = 0;
  };


  template <class T>
  class filter : public std::unary_function<const T,double> 
  {
  public:
    typedef Estd::filter<T> Self;
    typedef typename std::unary_function<const T,double>  Base;
    typedef typename Base::result_type   result_type   ;
    typedef typename Base::argument_type argument_type ;
  public:
    virtual ~filter() {}
    virtual bool operator() (const T& t) const = 0;
    virtual Self* clone() const = 0;
  };

  typedef Estd::filter< double > cut;

  template <class T1, class T2>
  class bifunction : public std::binary_function<const T1, const T2, double> 
  {
  public:
    typedef Estd::bifunction<T1,T2> Self;
    typedef typename std::binary_function<const T1, const T2, double>  Base;
    typedef typename Base::result_type   result_type   ;
    // typedef typename Base::argument_type argument_type ;
    // typedef typename Base::argument_type argument_type ;
  public:
    virtual ~bifunction() {}
    virtual double operator() (const T1& t, const T2& t2) const = 0;
    virtual Self* clone() const = 0;
  };

  template <class T1, class T2>
  class bifilter : public std::binary_function<const T1, const T2, bool> 
  {
  public:
    typedef Estd::bifilter<T1,T2> BiFilter;
    typedef typename std::binary_function<const T1, const T2, bool>  Base;
    typedef typename Base::result_type   result_type   ;
    // typedef typename Base::argument_type argument_type ;
    // typedef typename Base::argument_type argument_type ;
  public:
    virtual ~bifilter() {}
    virtual bool operator() (const T1& t, const T2& t2) const = 0;
    virtual BiFilter* clone() const = 0;
  };

  //------------ Binders

  template <class T1, class T2, class CON> 
  class binder_function : Estd::function<T1> 
  {
  public:
    typedef typename CON::iterator iterator;
    explicit binder_function(const Estd::bifunction<T1,T2>& f,
                             CON& con, 
                             const Estd::bifilter<double,double>& comp)
    {bifunction = f.clone(); container = &con; comparator = comp.clone();}
    virtual ~binder_function() {delete bifunction; delete comparator;}
    double operator() (const T1& t) const {
      if (container->size() == 0) return 0.;
      iterator it = container->begin();
      double d0 = (*bifunction)(t, **it); ++it;
      for ( ; it != container->end(); ++it) 
      {double d = (*bifunction)(t,**it); if ((*comparator)(d,d0)) d0 = d;}
      return d0;
    }
    Estd::function<T1>* clone() const
    {return new binder_function<T1,T2,CON>(*bifunction,*container,*comparator);}
    Estd::bifunction<T1,T2>* bifunction;
    CON* container;
    Estd::bifilter<double,double>* comparator;    
  };

  template <class T>
  class binder_cfunction : public Estd::function<T> 
  {
  public:
    typedef double (*FUN) (const T&);
    binder_cfunction( FUN f):_fun(f) {}
    double operator() (const T& t) const {return (*_fun)(t);}
    Estd::function<T>* clone() const 
    {return new Estd::binder_cfunction<T>(_fun);}
    FUN _fun;
  };

  template <class T1, class T2>
  class binder_cbifunction : public Estd::bifunction<T1,T2> 
  {
  public:
    typedef double (*FUN) (const T1&, const T2&);
    binder_cbifunction( FUN f):_fun(f) {}
    double operator() (const T1& t1, const T2& t2) const 
    {return (*_fun)(t1,t2);}
    Estd::bifunction<T1,T2>* clone() const 
    {return new Estd::binder_cbifunction<T1,T2>(_fun);}
    FUN _fun;
  };  

  // template <class BASE, class ARG, class RES>
//   class binder : public BASE {
//   public:
//     typedef BASE Base;
//     explicit binder(const Base& b) { base = b.clone();}
//     RES operator() (const ARG& a) const
//     {return bind( (*base)(a) );}
//     virtual RES bind(const RES& res) const {return res;};
//     Base* clone() const {return new binder(*base);}
//     Base* base;
//   };

//   template <class BASE, class ARG1, class ARG2, class RES>
//   class bibinder : public BASE {
//   public:
//     typedef BASE Base;
//     explicit bibinder(const Base& b) { base = b.clone();}
//     RES operator() (const ARG1& a1, const ARG2& a2) const
//     {return bind( (*base)(a1,a2) );}
//     virtual RES bind(const RES& res) const {return res;};
//     Base* base;
//   };

//   template <class T>
//   class binder_function : Estd::binder <Estd::function<T>,T,double> {
//   public:
//     typedef Estd::function<T> Function;
//     typedef typename Estd::binder<Function,T,double>::Base Base;
//     explicit binder_function(const Function& f):binder<Function,T,double>(f)
//     {}
//   };

 //  template <class T>
//   class binder_filter : Estd::binder <Estd::filter<T>,T,bool> {
//   public:
//     typedef Estd::filter<T> Filter;
//     typedef Filter Base;
//     explicit binder_filter(const Filter& f):binder<Filter,T,bool>(f)
//     {}
//   };

//   template <class T1, class T2>
//   class binder_bifunction : Estd::bibinder <Estd::bifunction<T1,T2>,T1,T2,double> {
//   public:
//     typedef Estd::bifunction<T1,T2> BiFunction;
//     explicit binder_bifunction(const BiFunction& f):bibinder<BiFunction,T1,T2,double>(f) {}
//   };
  
//   template <class T1, class T2>
//   class binder_bifilter : Estd::bibinder <Estd::bifilter<T1,T2>,T1,T2,bool> {
//   public:
//     typedef Estd::bifilter<T1,T2> BiFilter;
//     explicit binder_bifilter(const BiFilter& f):bibinder<BiFilter,T1,T2,bool>(f) {}
//   };
  

  //------------ Helper classes

  class identity : public Estd::function<double>
  {
  public:
    explicit identity(){}
    double operator() (const double& d) const {return d;}
    Estd::function<double>* clone() const {return new identity();}
  };

  class abs_min : public Estd::bifilter<double,double> 
  {
  public:
    bool operator() (const double& d, const double& d0) const
    {return (fabs(d) < fabs(d0));}
    BiFilter* clone() const {return new abs_min();}
  };

  template <class T>
  class equal_to : public Estd::filter<T> {
  public:
    typedef typename Estd::filter<T>::Self Filter;
    explicit equal_to(const T& t){ t0 = t;}    
    bool operator() (const T& t) const {return (t == t0);}
    Filter* clone() const { return new equal_to(t0) ;}
    T t0;
  };


  template <class T>
  class less : public Estd::filter<T>
  {
  public:
    typedef typename Estd::filter<T>::Self Filter;
    explicit less(const T& t){ t0 = t;}    
    bool operator() (const T& t) const {return t < t0;}
    Filter* clone() const { return new less(t0) ;}
    T t0;
  };


  template <class T>
  class greater : public Estd::filter<T>
  {
  public:
    typedef typename Estd::filter<T>::Self Filter;
    explicit greater(const T& t){ t0 = t;}    
    bool operator() (const T& t) const {return t > t0;}
    Filter* clone() const { return new greater(t0) ;}
    T t0;
  };



  template <class T>
  class in_range : public Estd::filter<T>
  {
  public:
    typedef typename Estd::filter<T>::Self Filter;
    explicit in_range(const T& x0, const T& x1 ) {t0=x0; t1=x1;}    
    bool operator() (const T& t) const {return ((t>t0) && (t<t1));}
    Filter* clone() const { return new in_range(t0,t1) ;}
    T t0;
    T t1;
  };

  template <class T>
  class logical_not : public Estd::filter<T>
  {
  public:
    typedef Estd::filter<bool> Filter;
    explicit logical_not() {}
    bool operator() (const bool& t) const {return !t;}
    Filter* clone() const { return new logical_not<T>();}
  };
  
  template <class T1, class T2>
  class logical_and : public Estd::bifilter<T1,T2>
  {
  public:
    typedef Estd::bifilter<T1,T2> BiFilter;
    explicit logical_and() {}
    bool operator() (const T1& t1, const T2& t2) const 
    {return (t1 && t2) ;}
    BiFilter* clone() const { return new logical_and<T1,T2>();}
  };
  
  template <class T1, class T2>
  class logical_or : public Estd::bifilter<T1,T2>
  {
  public:
    typedef Estd::bifilter<T1,T2> BiFilter;
    explicit logical_or() {}
    bool operator() (const T1& t1, const T2& t2) const 
    {return (t1 || t2) ;}
    BiFilter* clone() const { return new logical_and<T1,T2>();}
  };


  // composites - single

  template <class T> 
  class filter_from_function : public Estd::filter<T> {
  public:
    typedef Estd::filter<T>      Filter;
    typedef Estd::function<T>    Function;
    typedef Estd::filter<double> Cut;
  public:
    explicit filter_from_function(const Function& f, const Cut& c)
    {function = f.clone(); cut = c.clone();}
    ~filter_from_function() {delete function; delete cut;}
    bool operator() (const T& t) const
    {return (*cut)( (*function)(t )) ;}
    Filter* clone() const 
    {return new Estd::filter_from_function<T>(*function,*cut) ; }
  public:
    Function* function;
    Cut* cut;    
  };

  template <class T> 
  class filter_from_filter : public Estd::filter<T> {
  public:
    typedef Estd::filter<T>      Filter;
    typedef Estd::filter<bool> Cut;
  public:
    explicit filter_from_filter(const Filter& f, const Cut& c)
    {filter = f.clone(); cut = c.clone();}
    ~filter_from_filter() {delete filter; delete cut;}
    bool operator() (const T& t) const
    {return (*cut)( (*filter)(t )) ;}
    Filter* clone() const 
    {return new Estd::filter_from_filter<T>(*filter,*cut) ; }
  public:
    Filter* filter;
    Cut* cut;    
  };

  template <class T> 
  class filter_from_filters : public Estd::filter<T> {
  public:
    typedef Estd::filter<T>      Filter;
    typedef Estd::bifilter<bool,bool> Cut;
  public:
    explicit filter_from_filters(const Filter& f, 
                                 const Filter& f2, const Cut& c)
    {filter = f.clone(); filter2 = f2.clone(); cut = c.clone();}
    ~filter_from_filters() {delete filter; delete filter2; delete cut;}
    bool operator() (const T& t) const
    {return (*cut)( (*filter)(t ), (*filter2) (t) ) ;}
    Filter* clone() const 
    {return new Estd::filter_from_filters<T>(*filter,*filter2,*cut) ; }
  public:
    Filter* filter;
    Filter* filter2;
    Cut* cut;    
  };

  // composites - bi

  template <class T1, class T2> 
  class bifilter_from_bifunction : public Estd::bifilter<T1,T2> {
  public:
    typedef Estd::bifilter<T1,T2>      Filter;
    typedef Estd::bifunction<T1,T2>    Function;
    typedef Estd::filter<double> Cut;
  public:
    explicit bifilter_from_bifunction(const Function& f, const Cut& c)
    {function = f.clone(); cut = c.clone();}
    ~bifilter_from_bifunction() {delete function; delete cut;}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*function)(t1,t2)) ;}
    Filter* clone() const 
    {return new Estd::bifilter_from_bifunction<T1,T2>(*function,*cut) ; }
  public:
    Function* function;
    Cut* cut;    
  };

  template <class T1, class T2> 
  class bifilter_from_bifilter : public Estd::bifilter<T1,T2> {
  public:
    typedef Estd::bifilter<T1,T2> Filter;
    typedef Estd::filter<bool> Cut;
  public:
    explicit bifilter_from_bifilter(const Filter& f, const Cut& c)
    {filter = f.clone(); cut = c.clone();}
    ~bifilter_from_bifilter() {delete filter; delete cut;}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*filter)(t1,t2 )) ;}
    Filter* clone() const 
    {return new Estd::bifilter_from_bifilter<T1,T2>(*filter,*cut) ; }
  public:
    Filter* filter;
    Cut* cut;    
  };

  template <class T1, class T2> 
  class bifilter_from_bifilters : public Estd::bifilter<T1,T2> {
  public:
    typedef Estd::bifilter<T1,T2>      Filter;
    typedef Estd::bifilter<bool,bool> Cut;
  public:
    explicit bifilter_from_bifilters(const Filter& f, 
                                     const Filter& f2, const Cut& c)
    {filter = f.clone(); filter2 = f2.clone(); cut = c.clone();}
    ~bifilter_from_bifilters() {delete filter; delete filter2; delete cut;}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*filter)(t1,t2 ), (*filter2) (t1,t2) ) ;}
    Filter* clone() const 
    {return new Estd::bifilter_from_bifilters<T1,T2>(*filter,*filter2,*cut) ; }
  public:
    Filter* filter;
    Filter* filter2;
    Cut* cut;    
  };
 
};

//------------ Operators

template <class T>
Estd::filter_from_function<T> operator< ( const Estd::function<T>& funct,
                                           double value) 
{ return Estd::filter_from_function<T>(funct, Estd::less<double>( value ) ) ; }

template <class T>
Estd::filter_from_function<T> operator> ( const Estd::function<T>& funct,
                                          double value) 
{ return Estd::filter_from_function<T>(funct, Estd::greater<double>(value)) ; }

template <class T>
Estd::filter_from_function<T> operator== ( const Estd::function<T>& funct,
                                           double value) 
{ return Estd::filter_from_function<T>(funct, Estd::equal_to<double>(value)); }

template <class T>
Estd::filter_from_filter<T> operator! (const Estd::filter<T>& f)
{ return Estd::filter_from_filter<T>(f, Estd::logical_not<bool>() ) ; }

template <class T>
Estd::filter_from_filters<T> operator&& (const Estd::filter<T>& f1,
                                         const Estd::filter<T>& f2)
{ return Estd::filter_from_filters<T>(f1,f2,Estd::logical_and<bool,bool>()) ; }

template <class T>
Estd::filter_from_filters<T> operator|| (const Estd::filter<T>& f1,
                                         const Estd::filter<T>& f2)
{ return Estd::filter_from_filters<T>(f1,f2,Estd::logical_or<bool,bool>()) ; }

//---------------

template <class T1, class T2>
Estd::bifilter_from_bifunction<T1,T2> operator< 
  ( const Estd::bifunction<T1,T2>& funct, double value) 
{ return Estd::bifilter_from_bifunction<T1,T2>(funct, Estd::less<double>( value ) ) ; }

template <class T1, class T2>
Estd::bifilter_from_bifunction<T1,T2> operator>
  ( const Estd::bifunction<T1,T2>& funct, double value) 
{ return Estd::bifilter_from_bifunction<T1,T2>(funct, Estd::greater<double>( value ) ) ; }


template <class T1, class T2>
Estd::bifilter_from_bifunction<T1,T2> operator== ( const Estd::bifunction<T1,T2>& funct, double value) 
{ return Estd::bifilter_from_bifunction<T1,T2>(funct, Estd::equal_to<double>(value)); }

template <class T1, class T2>
Estd::bifilter_from_bifilter<T1,T2> operator! (const Estd::bifilter<T1,T2>& f)
{ return Estd::bifilter_from_bifilter<T1,T2>(f, Estd::logical_not<bool>() ) ; }

template <class T1, class T2>
Estd::bifilter_from_bifilters<T1,T2> operator&& 
(const Estd::bifilter<T1,T2>& f1, const Estd::bifilter<T1,T2>& f2)
{ return Estd::bifilter_from_bifilters<T1,T2>(f1,f2,Estd::logical_and<bool,bool>()) ; }

template <class T1, class T2>
Estd::bifilter_from_bifilters<T1,T2> operator|| 
(const Estd::bifilter<T1,T2>& f1, const Estd::bifilter<T1,T2>& f2)
{ return Estd::bifilter_from_bifilters<T1,T2>(f1,f2,Estd::logical_or<bool,bool>()) ; }


#endif
