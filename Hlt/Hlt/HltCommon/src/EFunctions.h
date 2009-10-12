#ifndef EFUNCTIONS_H
#define EFUNCTIONS_H
#include "HltBase/IFunctions.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include "boost/type_traits/remove_pointer.hpp"
/** Namespace for Hlt template base classes
 *  
 *  @author Jose Angel Hernando Morata
 *          based on LoKi ideas by I. Belyaev
 *  @date   2006-07-18
 */

namespace zen
{// Include file


  //------------ Binders --------------------------------- //

  template <class T>
  class abs_function : public zen::function<T> 
  {
  public:
    typedef zen::function<T> Function;
    explicit abs_function(const Function& fun) : m_fun(fun.clone()) { }
    abs_function(const abs_function<T>& rhs) : zen::function<T>(), m_fun(rhs.m_fun->clone()) { }
    virtual ~abs_function() {}
    double operator() (const T& t) const {return fabs((*m_fun)(t));}
    abs_function<T>* clone() const {return new abs_function<T>(*m_fun);}
  private:
    std::auto_ptr<Function> m_fun;
  };
  
  template <class T1, class Container> 
  class binder_function : public zen::function<T1> 
  {
  public:
    typedef typename boost::remove_pointer<typename Container::value_type>::type T2; 
    typedef typename Container::iterator iterator;
    typedef typename std::pair<double,iterator> Pair;
    explicit binder_function(const zen::bifunction<T1,T2>& f,
                             Container& con, 
                             const zen::bifilter<double,double>& comp)
        : bifunction( f.clone() )
        , container( &con )
        , comparator( comp.clone() )
    {}
    binder_function(const binder_function<T1,Container>& rhs)
        : bifunction( rhs.bifunction->clone() )
        , container( rhs.container )
        , comparator( rhs.comparator->clone() )
    {}
    virtual ~binder_function() {}
    double operator() (const T1& t) const {
      if (container->empty()) return 0.; //@TODO: is this really what we want??
      iterator it = container->begin(); 
      double d0 = (*bifunction)(t,**it); ++it;
      for (; it != container->end(); ++it) {
           double d = (*bifunction)(t,**it); 
           if ((*comparator)(d,d0)) d0 = d;
      }
      return d0;
    }
    Pair pair(const T1& t) const {
      // TODO throw exception if empty container
      iterator it = container->begin(); iterator it0 = it;
      double d0 = (*bifunction)(t, **it); ++it;
      for (; it != container->end(); ++it) {
        double d = (*bifunction)(t,**it);
        if ((*comparator)(d,d0)) {d0 = d; it0 = it;}
      }
      return Pair(d0,it0);
    }
    binder_function<T1,Container>* clone() const
    {return new binder_function(*bifunction,*container,*comparator);}
  private:
    std::auto_ptr<zen::bifunction<T1,T2> > bifunction;
    Container* container;
    zen::bifilter<double,double>* comparator;
  };  

   /* Return the key of the best match
   *
   */
  template <class T1, class Container>
  class binder_by_key : public zen::function<T1> 
  {
  public:
    typedef typename boost::remove_pointer<typename Container::value_type>::type T2; 
    typedef zen::binder_function<T1,Container> Binder;
    typedef typename Container::iterator iterator;
    explicit binder_by_key(const Binder& bin) : m_binder( bin.clone() ) {}
    binder_by_key(const binder_by_key<T1,Container>& rhs) : m_binder( rhs.m_binder->clone() ) {}
    virtual ~binder_by_key() {}
    double operator() (const T1& t1) const {
      std::pair<double,iterator> pair = m_binder->pair(t1);
      return (double) (*(pair.second))->key(); //FIXME: returning int as double...
    }
    zen::binder_by_key<T1,Container>* clone() const 
    {return new zen::binder_by_key<T1,Container>(*m_binder);}
  private:
    std::auto_ptr<Binder> m_binder;
  };

  /* Return some value from the best match
   *
   */

  template <class T1, class Container>
  class binder_by_value : public zen::function<T1> 
  {
    template <typename T> struct _helper { typedef double (T::* ptr_memfun) () const; };
  public:
    typedef typename boost::remove_pointer<typename Container::value_type>::type T2;
    typedef zen::binder_function<T1,Container> Binder;
    typedef typename Container::iterator iterator;
    typedef typename _helper<T2>::ptr_memfun ptr_memfun;
    explicit binder_by_value(const Binder& bin, ptr_memfun pmf): m_binder(bin.clone()),m_pmf(pmf) {}
    binder_by_value(const binder_by_value<T1,Container>& rhs) : m_binder(rhs.m_binder->clone()), m_pmf(rhs.m_pmf) {}
    virtual ~binder_by_value() {}
    double operator() (const T1& t1) const {
      std::pair<double,iterator> pair = m_binder->pair(t1);
      return (double) ((*(pair.second))->*m_pmf)();
    }
    binder_by_value<T1,Container>* clone() const 
    {return new binder_by_value<T1,Container>(*m_binder,m_pmf);}
  private:
    ptr_memfun m_pmf;
    std::auto_ptr<Binder> m_binder;
  };


  //------------ Helper classes ---------------------------------
  class abs_min : public zen::bifilter<double,double> 
  {
  public:
    bool operator() (const double& d, const double& d0) const
    {return (fabs(d) < fabs(d0));}
    abs_min* clone() const {return new abs_min();}
  };


  class abs_max : public zen::bifilter<double,double> 
  {
  public:
    bool operator() (const double& d, const double& d0) const
    {return (fabs(d) > fabs(d0));}
    abs_max* clone() const {return new abs_max();}
  };



  // simple filters and functions

  template <class T>
  class equal_to : public zen::filter<T> {
  public:
    explicit equal_to(const T& t) : m_t(t) { }    
    bool operator() (const T& t) const {return (t == m_t);}
    equal_to* clone() const { return new equal_to(m_t) ;}
  private:
    T m_t;
  };

  
  template <class T>
  class less : public zen::filter<T>
  {
  public:
    explicit less(const T& t) : m_t(t) {}
    bool operator() (const T& t) const {return t < m_t;}
    less* clone() const { return new less(m_t) ;}
  private:
    T m_t;
  };


  template <class T>
  class greater : public zen::filter<T>
  {
  public:
    explicit greater(const T& t) : m_t(t) { }
    bool operator() (const T& t) const {return t > m_t;}
    greater* clone() const { return new greater(m_t) ;}
  private:
    T m_t;
  };



  template <class T>
  class in_range : public zen::filter<T>
  {
  public:
    explicit in_range(const T& x0, const T& x1 ) : m_t0(x0),m_t1(x1) {}    
    bool operator() (const T& t) const {return ((t>m_t0) && (t<m_t1));}
    in_range* clone() const { return new in_range(m_t0,m_t1) ;}
  private:
    T m_t0;
    T m_t1;
  };

  template <class T>
  class logical_not : public zen::filter<T>
  {
  public:
    explicit logical_not() {}
    bool operator() (const bool& t) const {return !t;}
    logical_not<T>* clone() const { return new logical_not<T>();}
  };
  
  template <class T1, class T2>
  class logical_and : public zen::bifilter<T1,T2>
  {
  public:
    explicit logical_and() {}
    bool operator() (const T1& t1, const T2& t2) const 
    {return (t1 && t2) ;}
    logical_and<T1,T2>* clone() const { return new logical_and<T1,T2>();}
  };
  
  template <class T1, class T2>
  class logical_or : public zen::bifilter<T1,T2>
  {
  public:
    explicit logical_or() {}
    bool operator() (const T1& t1, const T2& t2) const 
    {return (t1 || t2) ;}
    logical_or<T1,T2>* clone() const { return new logical_or<T1,T2>();}
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
        : m_function( f.clone() )
        , m_cut( c.clone() ) 
        {}
    filter_from_function(const filter_from_function<T>& f)
        : zen::filter<T>()
        , m_function( f.m_function->clone() )
        , m_cut( f.m_cut->clone() ) 
        {}
    ~filter_from_function() {}
    bool operator() (const T& t) const
    {return (*m_cut)( (*m_function)(t )) ;}
    zen::filter_from_function<T>* clone() const 
    {return new zen::filter_from_function<T>(*m_function,*m_cut) ; }
  private:
    std::auto_ptr<Function> m_function;
    std::auto_ptr<Cut> m_cut;    
  };

  template <class T> 
  class filter_from_filter : public zen::filter<T> {
  public:
    typedef zen::filter<T>      Filter;
    typedef zen::filter<bool> Cut;
  public:
    explicit filter_from_filter(const Filter& f, const Cut& c)
        : Filter()
        , m_filter( f.clone() )
        , m_cut( c.clone() ) 
    {}
    filter_from_filter(const filter_from_filter<T>& f)
        : Filter()
        , m_filter( f.m_filter->clone() )
        , m_cut( f.m_cut->clone() ) 
    {}
    ~filter_from_filter() {}
    bool operator() (const T& t) const
    {return (*m_cut)( (*m_filter)(t )) ;}
    zen::filter_from_filter<T>* clone() const 
    {return new zen::filter_from_filter<T>(*m_filter,*m_cut) ; }
  private:
    std::auto_ptr<Filter> m_filter;
    std::auto_ptr<Cut> m_cut;    
  };

  template <class T> 
  class filter_from_filters : public zen::filter<T> {
  public:
    typedef zen::filter<T>      Filter;
    typedef zen::bifilter<bool,bool> Cut;
  public:
    explicit filter_from_filters(const Filter& f, 
                                 const Filter& f2, const Cut& c)
        : Filter()
        , filter(f.clone())
        , filter2(f2.clone())
        , cut(c.clone())
    {}
    filter_from_filters(const filter_from_filters& f)
        : Filter()
        , filter(f.filter->clone())
        , filter2(f.filter2->clone())
        , cut(f.cut->clone())
    {}
    ~filter_from_filters() {}
    bool operator() (const T& t) const
    {return (*cut)( (*filter)(t ), (*filter2) (t) ) ;}
    zen::filter_from_filters<T>* clone() const 
    {return new zen::filter_from_filters<T>(*filter,*filter2,*cut) ; }
  private:
    std::auto_ptr<Filter> filter;
    std::auto_ptr<Filter> filter2;
    std::auto_ptr<Cut> cut;    
  };

  // composites - bi

  template <class T1, class T2> 
  class bifilter_from_bifunction : public zen::bifilter<T1,T2> {
  public:
    typedef zen::bifunction<T1,T2>    Function;
    typedef zen::filter<double> Cut;
  public:
    explicit bifilter_from_bifunction(const Function& f, const Cut& c)
        : function(f.clone())
        , cut( c.clone() )
    {}
    ~bifilter_from_bifunction() {}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*function)(t1,t2)) ;}
    zen::bifilter_from_bifunction<T1,T2>* clone() const 
    {return new zen::bifilter_from_bifunction<T1,T2>(*function,*cut) ; }
  private:
    std::auto_ptr<Function> function;
    std::auto_ptr<Cut> cut;    
  };

  template <class T1, class T2> 
  class bifilter_from_bifilter : public zen::bifilter<T1,T2> {
  public:
    typedef zen::bifilter<T1,T2> Filter;
    typedef zen::filter<bool> Cut;
  public:
    explicit bifilter_from_bifilter(const Filter& f, const Cut& c)
        : filter(f.clone())
        , cut(c.clone())
    {}
    bifilter_from_bifilter(const bifilter_from_bifilter<T1,T2>& f)
        : filter(f.filter->clone())
        , cut(f.cut->clone())
    {}
    ~bifilter_from_bifilter() {}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*filter)(t1,t2 )) ;}
    zen::bifilter_from_bifilter<T1,T2>* clone() const 
    {return new zen::bifilter_from_bifilter<T1,T2>(*filter,*cut) ; }
  private:
    std::auto_ptr<Filter> filter;
    std::auto_ptr<Cut> cut;    
  };

  template <class T1, class T2> 
  class bifilter_from_bifilters : public zen::bifilter<T1,T2> {
  public:
    typedef zen::bifilter<T1,T2>      Filter;
    typedef zen::bifilter<bool,bool> Cut;
  public:
    explicit bifilter_from_bifilters(const Filter& f, 
                                     const Filter& f2, const Cut& c)
        : filter(f.clone())
        , filter2(f2.clone())
        , cut(c.clone())
    {}
    bifilter_from_bifilters(const bifilter_from_bifilters& f)
        : filter(f.filter->clone())
        , filter2(f.filter2->clone())
        , cut(f.cut->clone())
    {}
    ~bifilter_from_bifilters() {}
    bool operator() (const T1& t1, const T2& t2) const
    {return (*cut)( (*filter)(t1,t2 ), (*filter2) (t1,t2) ) ;}
    zen::bifilter_from_bifilters<T1,T2>* clone() const 
    {return new zen::bifilter_from_bifilters<T1,T2>(*filter,*filter2,*cut) ; }
  private:
    std::auto_ptr<Filter> filter;
    std::auto_ptr<Filter> filter2;
    std::auto_ptr<Cut> cut;    
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
zen::bifilter_from_bifunction<T1,T2> operator< ( const zen::bifunction<T1,T2>& funct, double value) 
{ return zen::bifilter_from_bifunction<T1,T2>(funct, zen::less<double>( value ) ) ; }

template <class T1, class T2>
zen::bifilter_from_bifunction<T1,T2> operator> ( const zen::bifunction<T1,T2>& funct, double value) 
{ return zen::bifilter_from_bifunction<T1,T2>(funct, zen::greater<double>( value ) ) ; }


template <class T1, class T2>
zen::bifilter_from_bifunction<T1,T2> operator== ( const zen::bifunction<T1,T2>& funct, double value) 
{ return zen::bifilter_from_bifunction<T1,T2>(funct, zen::equal_to<double>(value)); }

template <class T1, class T2>
zen::bifilter_from_bifilter<T1,T2> operator! (const zen::bifilter<T1,T2>& f)
{ return zen::bifilter_from_bifilter<T1,T2>(f, zen::logical_not<bool>() ) ; }

template <class T1, class T2>
zen::bifilter_from_bifilters<T1,T2> operator&& (const zen::bifilter<T1,T2>& f1, const zen::bifilter<T1,T2>& f2)
{ return zen::bifilter_from_bifilters<T1,T2>(f1,f2,zen::logical_and<bool,bool>()) ; }

template <class T1, class T2>
zen::bifilter_from_bifilters<T1,T2> operator|| (const zen::bifilter<T1,T2>& f1, const zen::bifilter<T1,T2>& f2)
{ return zen::bifilter_from_bifilters<T1,T2>(f1,f2,zen::logical_or<bool,bool>()) ; }


#endif
