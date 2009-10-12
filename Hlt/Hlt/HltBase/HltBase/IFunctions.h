// $Id: IFunctions.h,v 1.1 2009-10-12 18:18:31 graven Exp $
#ifndef HLTBASE_OPER_H 
#define HLTBASE_OPER_H 1

#include <functional>

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
    virtual ~filter() {}
    virtual bool operator() (const T& t) const  = 0;
    virtual Self* clone() const = 0;
  };

  template <class T1, class T2>
  class bifunction : public std::binary_function<const T1, const T2, double> 
  {
  public:
    typedef zen::bifunction<T1,T2> Self;
    typedef typename std::binary_function<const T1, const T2, double>  Base;
    typedef typename Base::result_type   result_type   ;
    typedef typename Base::first_argument_type first_argument_type ;
    typedef typename Base::second_argument_type second_argument_type ;
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
    typedef typename Base::first_argument_type first_argument_type ;
    typedef typename Base::second_argument_type second_argument_type ;
    virtual ~bifilter() {}
    virtual bool operator() (const T1& t, const T2& t2) const = 0;
    virtual BiFilter* clone() const = 0;
  };
}
#endif
