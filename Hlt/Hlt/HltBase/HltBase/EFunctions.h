// $Id: EFunctions.h,v 1.11 2008-02-19 19:19:31 graven Exp $
#ifndef HLTBASE_OPER_H 
#define HLTBASE_OPER_H 1

#include <vector>
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

namespace zen
{// Include file

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
    typedef zen::function<T> Self;
    typedef typename std::unary_function<const T,double>  Base;
    typedef typename Base::result_type   result_type   ;
    typedef typename Base::argument_type argument_type ;
  public:
    virtual ~function() {}
    virtual double operator() (const T& t) const = 0;
    virtual Self* clone() const = 0;
  };


  template <class T>
  class filter : public std::unary_function<const T,bool> 
  {
  public:
    typedef zen::filter<T> Self;
    typedef typename std::unary_function<const T,double>  Base;
    typedef typename Base::result_type   result_type   ;
    typedef typename Base::argument_type argument_type ;
  public:
    virtual ~filter() {}
    virtual bool operator() (const T& t) const  = 0;
    virtual Self* clone() const = 0;
  };

  typedef zen::filter< double > cut;

  template <class T1, class T2>
  class bifunction : public std::binary_function<const T1, const T2, double> 
  {
  public:
    typedef zen::bifunction<T1,T2> Self;
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
    typedef zen::bifilter<T1,T2> BiFilter;
    typedef typename std::binary_function<const T1, const T2, bool>  Base;
    typedef typename Base::result_type   result_type   ;
    // typedef typename Base::argument_type argument_type ;
    // typedef typename Base::argument_type argument_type ;
  public:
    virtual ~bifilter() {}
    virtual bool operator() (const T1& t, const T2& t2) const = 0;
    virtual BiFilter* clone() const = 0;
  };

  //------------ Binders --------------------------------- //

  template <class T>
  class abs_function : public zen::function<T> 
  {
  public:
    typedef zen::function<T> Function;
    explicit abs_function(const Function& fun) : _fun(fun.clone()) { }
    virtual ~abs_function() {delete _fun;}
    double operator() (const T& t) const 
      
    {return fabs((*_fun)(t));}
    Function* clone() const {return new abs_function<T>(*_fun);}
  public:
    Function* _fun;
  };
  
  template <class T>
  class binder_memberfunction : public zen::function<T> 
  {
  public:
    typedef zen::function<T> Function;
    typedef double (T::* ptr_menfun) () const;
    explicit binder_memberfunction(ptr_menfun pmf):m_pmf(pmf) {}
    double operator() (const T& t) const
    {return ((&t)->*m_pmf)();}
    Function* clone() const 
    {return new binder_memberfunction<T>(m_pmf);}
  public:
    ptr_menfun m_pmf;
  };


  template <class T1, class T2> 
  class binder_function : public zen::function<T1> 
  {
  public:
    typedef typename std::vector<T2*> Container;
    typedef typename std::vector<T2*>::iterator Iterator;
    typedef typename std::pair<double,Iterator> Pair;
    explicit binder_function(const zen::bifunction<T1,T2>& f,
                             Container& con, 
                             const zen::bifilter<double,double>& comp)
    {bifunction = f.clone(); container = &con; comparator = comp.clone();}
    virtual ~binder_function() {delete bifunction; delete comparator;}
    double operator() (const T1& t) const {
      if (container->size() == 0) return 0.; 
      Iterator it = container->begin(); 
      double d0 = (*bifunction)(t,**it); ++it;
      for (; it != container->end(); ++it)
      {double d = (*bifunction)(t,**it); if ((*comparator)(d,d0)) d0 = d;}
      return d0;
    }
    Pair pair(const T1& t) const {
      // TODO throw exception if empty container
      Iterator it = container->begin(); Iterator it0 = it;
      double d0 = (*bifunction)(t, **it); ++it;
      for (; it != container->end(); ++it) {
        double d = (*bifunction)(t,**it);
        if ((*comparator)(d,d0)) {d0 = d; it0 = it;}}
      return Pair(d0,it0);
    }
    zen::function<T1>* clone() const
    {return new binder_function<T1,T2>(*bifunction,*container,*comparator);}
  public:
    zen::bifunction<T1,T2>* bifunction;
    Container* container;
    zen::bifilter<double,double>* comparator;
  };  

  template <class T>
  class binder_cfunction : public zen::function<T> 
  {
  public:
    typedef double (*FUN) (const T&);
    binder_cfunction( FUN f):_fun(f) {}
    double operator() (const T& t) const {return (*_fun)(t);}
    zen::function<T>* clone() const 
    {return new zen::binder_cfunction<T>(_fun);}
    FUN _fun;
  };

  template <class T1, class T2>
  class binder_cbifunction : public zen::bifunction<T1,T2> 
  {
  public:
    typedef double (*FUN) (const T1&, const T2&);
    binder_cbifunction( FUN f):_fun(f) {}
    double operator() (const T1& t1, const T2& t2) const 
    {return (*_fun)(t1,t2);}
    zen::bifunction<T1,T2>* clone() const 
    {return new zen::binder_cbifunction<T1,T2>(_fun);}
    FUN _fun;
  };  

   /* Return the key of the best match
   *
   */
  template <class T1, class T2>
  class binder_by_key : public zen::function<T1> 
  {
  public:
    typedef zen::function<T1> Function;
    typedef zen::binder_function<T1,T2> Binder;
    typedef typename std::vector<T2*>::iterator Iterator;
    explicit binder_by_key(const Binder& bin)
    {m_binder = dynamic_cast<Binder*>(bin.clone());}
    virtual ~binder_by_key() {delete m_binder;}
    double operator() (const T1& t1) const {
      std::pair<double,Iterator> pair = m_binder->pair(t1);
      return (double) (*(pair.second))->key();
    }
    Function* clone() const 
    {return new zen::binder_by_key<T1,T2>(*m_binder);}
  public:
    Binder* m_binder;
  };

  /* Return some value from the best match
   *
   */
  template <class T1, class T2>
  class binder_by_value : public zen::function<T1> 
  {
  public:
    typedef zen::function<T1> Function;
    typedef zen::binder_function<T1,T2> Binder;
    typedef typename std::vector<T2*>::iterator Iterator;
    typedef double (T2::* ptr_menfun) () const;
    explicit binder_by_value(const Binder& bin, ptr_menfun pmf):m_pmf(pmf)
    {m_binder = dynamic_cast<Binder*>(bin.clone());}
    virtual ~binder_by_value() {delete m_binder;}
    double operator() (const T1& t1) const {
      std::pair<double,Iterator> pair = m_binder->pair(t1);
      return (double) ((*(pair.second))->*m_pmf)();
    }
    Function* clone() const 
    {return new binder_by_value<T1,T2>(*m_binder,m_pmf);}
  public:
    ptr_menfun m_pmf;
    Binder* m_binder;
  };

  
  //------------ Helper classes ---------------------------------

  class abs_min : public zen::bifilter<double,double> 
  {
  public:
    bool operator() (const double& d, const double& d0) const
    {return (fabs(d) < fabs(d0));}
    BiFilter* clone() const {return new abs_min();}
  };


  class abs_max : public zen::bifilter<double,double> 
  {
  public:
    bool operator() (const double& d, const double& d0) const
    {return (fabs(d) > fabs(d0));}
    BiFilter* clone() const {return new abs_max();}
  };

  class emin : public zen::bifilter<double,double> 
  {
  public:
    bool operator() (const double& d, const double& d0) const
    {return (d < d0);}
    BiFilter* clone() const {return new zen::emin();}
  };

  class emax : public zen::bifilter<double,double> 
  {
  public:
    bool operator() (const double& d, const double& d0) const
    {return (d > d0);}
    BiFilter* clone() const {return new zen::emax();}
  };

  template <class T1, class T2> 
  class binder_min_function : public binder_function<T1,T2> {
  public:
    typedef typename std::vector<T2*> Container;
    explicit binder_min_function(const zen::bifunction<T1,T2>& f,
                                 Container& cont):
      zen::binder_function<T1,T2>(f, cont, zen::emin() ) {};
  };

  template <class T1, class T2> 
  class binder_max_function : public binder_function<T1,T2> {
  public:
    typedef typename std::vector<T2*> Container;
    explicit binder_max_function(const zen::bifunction<T1,T2>& f,
                                 Container& cont):
      zen::binder_function<T1,T2>(f, cont, zen::emax() ) {};
  };

  // simple filters and functions

  class identity : public zen::function<double>
  {
  public:
    explicit identity(){}
    double operator() (const double& d) const {return d;}
    zen::function<double>* clone() const {return new identity();}
  };


  template <class T>
  class equal_to : public zen::filter<T> {
  public:
    typedef typename zen::filter<T>::Self Filter;
    explicit equal_to(const T& t){ t0 = t;}    
    bool operator() (const T& t) const {return (t == t0);}
    Filter* clone() const { return new equal_to(t0) ;}
    T t0;
  };

  
  template <class T>
  class less : public zen::filter<T>
  {
  public:
    typedef typename zen::filter<T>::Self Filter;
    explicit less(const T& t){ t0 = t;}    
    bool operator() (const T& t) const {return t < t0;}
    Filter* clone() const { return new less(t0) ;}
    T t0;
  };


  template <class T>
  class greater : public zen::filter<T>
  {
  public:
    typedef typename zen::filter<T>::Self Filter;
    explicit greater(const T& t){ t0 = t;}    
    bool operator() (const T& t) const {return t > t0;}
    Filter* clone() const { return new greater(t0) ;}
    T t0;
  };



  template <class T>
  class in_range : public zen::filter<T>
  {
  public:
    typedef typename zen::filter<T>::Self Filter;
    explicit in_range(const T& x0, const T& x1 ) {t0=x0; t1=x1;}    
    bool operator() (const T& t) const {return ((t>t0) && (t<t1));}
    Filter* clone() const { return new in_range(t0,t1) ;}
    T t0;
    T t1;
  };

  template <class T>
  class logical_not : public zen::filter<T>
  {
  public:
    typedef zen::filter<bool> Filter;
    explicit logical_not() {}
    bool operator() (const bool& t) const {return !t;}
    Filter* clone() const { return new logical_not<T>();}
  };
  
  template <class T1, class T2>
  class logical_and : public zen::bifilter<T1,T2>
  {
  public:
    typedef zen::bifilter<T1,T2> BiFilter;
    explicit logical_and() {}
    bool operator() (const T1& t1, const T2& t2) const 
    {return (t1 && t2) ;}
    BiFilter* clone() const { return new logical_and<T1,T2>();}
  };
  
  template <class T1, class T2>
  class logical_or : public zen::bifilter<T1,T2>
  {
  public:
    typedef zen::bifilter<T1,T2> BiFilter;
    explicit logical_or() {}
    bool operator() (const T1& t1, const T2& t2) const 
    {return (t1 || t2) ;}
    BiFilter* clone() const { return new logical_or<T1,T2>();}
  };

  // composites - single

  template <class T> 
  class filter_from_function : public zen::filter<T> {
  public:
    typedef zen::filter<T>      Filter;
    typedef zen::function<T>    Function;
    typedef zen::filter<double> Cut;
  public:
    explicit filter_from_function(const Function& f, const Cut& c)
    {function = f.clone(); cut = c.clone();}
    ~filter_from_function() {delete function; delete cut;}
    bool operator() (const T& t) const
    {return (*cut)( (*function)(t )) ;}
    Filter* clone() const 
    {return new zen::filter_from_function<T>(*function,*cut) ; }
  public:
    Function* function;
    Cut* cut;    
  };

  template <class T> 
  class filter_from_filter : public zen::filter<T> {
  public:
    typedef zen::filter<T>      Filter;
    typedef zen::filter<bool> Cut;
  public:
    explicit filter_from_filter(const Filter& f, const Cut& c)
    {filter = f.clone(); cut = c.clone();}
    ~filter_from_filter() {delete filter; delete cut;}
    bool operator() (const T& t) const
    {return (*cut)( (*filter)(t )) ;}
    Filter* clone() const 
    {return new zen::filter_from_filter<T>(*filter,*cut) ; }
  public:
    Filter* filter;
    Cut* cut;    
  };

  template <class T> 
  class filter_from_filters : public zen::filter<T> {
  public:
    typedef zen::filter<T>      Filter;
    typedef zen::bifilter<bool,bool> Cut;
  public:
    explicit filter_from_filters(const Filter& f, 
                                 const Filter& f2, const Cut& c)
    {filter = f.clone(); filter2 = f2.clone(); cut = c.clone();}
    ~filter_from_filters() {delete filter; delete filter2; delete cut;}
    bool operator() (const T& t) const
    {return (*cut)( (*filter)(t ), (*filter2) (t) ) ;}
    Filter* clone() const 
    {return new zen::filter_from_filters<T>(*filter,*filter2,*cut) ; }
  public:
    Filter* filter;
    Filter* filter2;
    Cut* cut;    
  };

  // composites - bi

  template <class T1, class T2> 
  class bifilter_from_bifunction : public zen::bifilter<T1,T2> {
  public:
    typedef zen::bifilter<T1,T2>      Filter;
    typedef zen::bifunction<T1,T2>    Function;
    typedef zen::filter<double> Cut;
  public:
    explicit bifilter_from_bifunction(const Function& f, const Cut& c)
    {function = f.clone(); cut = c.clone();}
    ~bifilter_from_bifunction() {delete function; delete cut;}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*function)(t1,t2)) ;}
    Filter* clone() const 
    {return new zen::bifilter_from_bifunction<T1,T2>(*function,*cut) ; }
  public:
    Function* function;
    Cut* cut;    
  };

  template <class T1, class T2> 
  class bifilter_from_bifilter : public zen::bifilter<T1,T2> {
  public:
    typedef zen::bifilter<T1,T2> Filter;
    typedef zen::filter<bool> Cut;
  public:
    explicit bifilter_from_bifilter(const Filter& f, const Cut& c)
    {filter = f.clone(); cut = c.clone();}
    ~bifilter_from_bifilter() {delete filter; delete cut;}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*filter)(t1,t2 )) ;}
    Filter* clone() const 
    {return new zen::bifilter_from_bifilter<T1,T2>(*filter,*cut) ; }
  public:
    Filter* filter;
    Cut* cut;    
  };

  template <class T1, class T2> 
  class bifilter_from_bifilters : public zen::bifilter<T1,T2> {
  public:
    typedef zen::bifilter<T1,T2>      Filter;
    typedef zen::bifilter<bool,bool> Cut;
  public:
    explicit bifilter_from_bifilters(const Filter& f, 
                                     const Filter& f2, const Cut& c)
    {filter = f.clone(); filter2 = f2.clone(); cut = c.clone();}
    ~bifilter_from_bifilters() {delete filter; delete filter2; delete cut;}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*filter)(t1,t2 ), (*filter2) (t1,t2) ) ;}
    Filter* clone() const 
    {return new zen::bifilter_from_bifilters<T1,T2>(*filter,*filter2,*cut) ; }
  public:
    Filter* filter;
    Filter* filter2;
    Cut* cut;    
  };
 
};

//------------ Operators -----------------------------------------------

template <class T>
zen::filter_from_function<T> operator< ( const zen::function<T>& funct,
                                           double value) 
{ return zen::filter_from_function<T>(funct, zen::less<double>( value ) ) ; }

template <class T>
zen::filter_from_function<T> operator> ( const zen::function<T>& funct,
                                          double value) 
{ return zen::filter_from_function<T>(funct, zen::greater<double>(value)) ; }

template <class T>
zen::filter_from_function<T> operator== ( const zen::function<T>& funct,
                                           double value) 
{ return zen::filter_from_function<T>(funct, zen::equal_to<double>(value)); }

template <class T>
zen::filter_from_filter<T> operator! (const zen::filter<T>& f)
{ return zen::filter_from_filter<T>(f, zen::logical_not<bool>() ) ; }

template <class T>
zen::filter_from_filters<T> operator&& (const zen::filter<T>& f1,
                                         const zen::filter<T>& f2)
{ return zen::filter_from_filters<T>(f1,f2,zen::logical_and<bool,bool>()) ; }

template <class T>
zen::filter_from_filters<T> operator|| (const zen::filter<T>& f1,
                                         const zen::filter<T>& f2)
{ return zen::filter_from_filters<T>(f1,f2,zen::logical_or<bool,bool>()) ; }

//---------------

template <class T1, class T2>
zen::bifilter_from_bifunction<T1,T2> operator< 
  ( const zen::bifunction<T1,T2>& funct, double value) 
{ return zen::bifilter_from_bifunction<T1,T2>(funct, zen::less<double>( value ) ) ; }

template <class T1, class T2>
zen::bifilter_from_bifunction<T1,T2> operator>
  ( const zen::bifunction<T1,T2>& funct, double value) 
{ return zen::bifilter_from_bifunction<T1,T2>(funct, zen::greater<double>( value ) ) ; }


template <class T1, class T2>
zen::bifilter_from_bifunction<T1,T2> operator== ( const zen::bifunction<T1,T2>& funct, double value) 
{ return zen::bifilter_from_bifunction<T1,T2>(funct, zen::equal_to<double>(value)); }

template <class T1, class T2>
zen::bifilter_from_bifilter<T1,T2> operator! (const zen::bifilter<T1,T2>& f)
{ return zen::bifilter_from_bifilter<T1,T2>(f, zen::logical_not<bool>() ) ; }

template <class T1, class T2>
zen::bifilter_from_bifilters<T1,T2> operator&& 
(const zen::bifilter<T1,T2>& f1, const zen::bifilter<T1,T2>& f2)
{ return zen::bifilter_from_bifilters<T1,T2>(f1,f2,zen::logical_and<bool,bool>()) ; }

template <class T1, class T2>
zen::bifilter_from_bifilters<T1,T2> operator|| 
(const zen::bifilter<T1,T2>& f1, const zen::bifilter<T1,T2>& f2)
{ return zen::bifilter_from_bifilters<T1,T2>(f1,f2,zen::logical_or<bool,bool>()) ; }


#endif
