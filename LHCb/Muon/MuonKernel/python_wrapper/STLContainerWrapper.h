// $Id: STLContainerWrapper.h,v 1.2 2005-02-21 09:41:55 atsareg Exp $

#ifndef L0MPY_STLCONTAINERWRAPPER_H
#define L0MPY_STLCONTAINERWRAPPER_H 1   

// Include files

#include <iostream>

#include <algorithm>
#include <map>
#include <vector>

#include "boost/python/errors.hpp"
#include <boost/python/detail/exception_handler.hpp>
#include "boost/python/list.hpp"
#include "boost/python/tuple.hpp"

using namespace boost::python;

/**
    Wrapper for std::vector.
    
    Based on container.h from Mike Rovner (2003-01-22).
    http://www.python.org/cgi-bin/moinmoin/boost_2epython_2fStlContainers
    
*/

template<class T> 
struct std_item 
{ 
    typedef typename T::value_type V; 

    static int count(T const& x, V const& v)
    {
        return std::count(x.begin(), x.end(), v);
    }
  
    static void del(T& x, int i) 
    { 
        int len = x.size();
        
        if( i<0 ) i+=len; 
        if( i>=0 && i<len ) 
        {
          typename T::iterator it = x.begin();
          std::advance(it, i);
          x.erase(it); 
        }
        else IndexError(); 
    }
  
    static V& get(T& x, int i) 
    { 
        int len = x.size();
        if( i<0 ) i+=len; 
        if( not (i>=0 && i<len) ) IndexError();
        return x[i]; 
    }
    
    static bool in(T& x, V& v) 
    { 
      typename T::iterator location = std::find(x.begin(), x.end(), v); 

      if( location != x.end() ) return true;
      else return false;

    }
    
    static int index(T& x, V& v) 
    { 
      typename T::iterator location = std::find(x.begin(), x.end(), v); 
      
      if( location != x.end() ) return std::distance(x.begin(), location);
      else return -1;
    } 
    
    static void insert( T& x, int i, V& v ) 
    { 
      typename T::iterator it = x.begin();
      std::advance(it, i);      
      x.insert(it, v);
    }

    static V pop( T& x, int i ) 
    {
        int len = x.size();
        if( i<0 ) i+=len; 
        if( not (i>=0 && i<len) ) IndexError();
    
        V v = x[i];
        typename T::iterator it = x.begin();
        std::advance(it, i);
        x.erase(it);
        
        return v;  
    }
    
    static void remove( T& x, V& v ) 
    { 
      typename T::iterator it = std::find(x.begin(), x.end(), v); 
      if( it != x.end() ) x.erase(it);
    }
    
    static void reverse(T& x)
    {
      std::reverse( x.begin(), x.end());
    }
    
    static void set(T& x, int i, V& v) 
    { 
        int len = x.size();
        if( i<0 ) i+=len; 
        if( i>=0 && i<len ) x[i]=v; 
        else IndexError(); 
    }

    static void sort(T& x) { std::sort(x.begin(), x.end()); }
    
    static void unique(T& x) 
    {
      typename T::iterator it;
      std::sort(x.begin(), x.end());
      it = std::unique(x.begin(), x.end());
      x.erase(it, x.end());
    }
    
    static void IndexError() { PyErr_SetString(PyExc_RuntimeError, "!!!error!!!"); } 
};


/**
    Wrapper for std::map.
    
    Based on container.h from Mike Rovner (2003-01-22).
    http://www.python.org/cgi-bin/moinmoin/boost_2epython_2fStlContainers
    
*/

template<class T> 
struct map_item 
{ 
    typedef typename T::key_type K; 
    typedef typename T::mapped_type V; 

    static T copy(T& x)
    {
      T y;
      for(typename T::iterator it=x.begin(); it!=x.end(); ++it)
        y[it->first] = it->second;
      return y;
    }
    
    static void del(T& x, K const& i) 
    { 
        if( x.find(i) != x.end() ) x.erase(i); 
        else KeyError(); 
    }
    
    static V& get(T& x, K const& i) 
    { 
        if( x.find(i) == x.end() ) KeyError();
        return x[i]; 
    }

    static V getByValue(T& x, K const& i) 
    { 
        if( x.find(i) == x.end() ) KeyError();
        return x[i]; 
    }
        
    static bool in(T& x, K const& i) 
    { 
        return x.find(i) != x.end(); 
    } 
    
    static list items(T& x) 
    { 
        list t; 
        for(typename T::const_iterator it=x.begin(); it!=x.end(); ++it) 
          t.append(make_tuple(it->first,it->second)); 
        return t; 
    } 
       
    static list keys(T& x) 
    { 
        list t; 
        for(typename T::iterator it=x.begin(); it!=x.end(); ++it) t.append(it->first); 
        return t; 
    }
    
    static void set(T& x, K const& i, V& v) 
    {
        x[i]=v; // use map autocreation feature 
    } 
    
    static void setByValue(T& x, K const& i, V v) 
    {
        x[i]=v; // use map autocreation feature 
    } 

    static list values(T& x) 
    { 
        list t; 
        for(typename T::const_iterator it=x.begin(); it!=x.end(); ++it) t.append(it->second); 
        return t; 
    } 

    static void KeyError() { PyErr_SetString(PyExc_RuntimeError, "!!!error!!!"); } 
};

#endif //L0MPY_STLCONTAINERWRAPPER_H
