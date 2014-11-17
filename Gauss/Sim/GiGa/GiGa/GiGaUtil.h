// $Id: GiGaUtil.h,v 1.10 2006-09-19 22:13:22 gcorti Exp $ 
#ifndef GIGA_GIGAUTIL_H 
#define GIGA_GIGAUTIL_H 1

// Include files
// from STD & STL 
#include <iostream>
#include <string>
#include <functional>

// from Gaudi 
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"

// Forward declarations 
class IObjManager;
class ISvcLocator;
class IFactory;

/** @namespace  GiGaUtil GiGaUtil.h "GiGa/GiGaUtil.h"
 *  @brief  helper utilities for GiGa 
 *
 *  collection of simple helper utilities for GiGa package 
 * 
 *  @author Ivan Belyaev
 *  @date   23/07/2001
 */

namespace GiGaUtil 
{
  
  /** split a "Type/Name" string into "Type" and "Name"
   *  
   *  rules:
   * -  "Type/Name"  =  "Type" + "Name"
   * -  "Type"       =  "Type" + "Type"
   * -  "Type/"      =  "Type" + "Type"
   * 
   *  error conditions 
   *
   *  -  empty input string  
   *  -  more than 1 separator ('/') 
   *  -  empty type ("/Name")
   *
   *  @param TypeAndName   the string to be splitted 
   *  @param Type          returned "Type" 
   *  @param Name          returned "Name" 
   *  @return status code
   */  
  StatusCode SplitTypeAndName ( const std::string& TypeAndName ,
                                std::string      & Type        , 
                                std::string      & Name        );
  
  /** useful utility(function) to extract the object type name 
   *  @param obj pointer to object of type "TYPE"
   *  @return type name of the object
   */ 
  template <class TYPE>
  inline const std::string ObjTypeName( TYPE obj )
  { 
    return 
      obj ? 
      std::string( System::typeinfoName( typeid(*obj) ) ) : 
      std::string( "'UNKNOWN_type'" );
  }
  
  /** @class Delete 
   * 
   *  useful utility(templated class) to delete the object 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  class Delete
  { 
  public:
    
    /** delete the object 
     *  @param obj pointer to object to be deleted 
     *  @return NULL pointer 
     */
    template <class TYPE>
    inline TYPE* operator() ( TYPE* obj ) const 
    { 
      if( 0 != obj ) { delete obj ; obj = 0 ; }
      return obj ;
    }
  };
  
  /** @class Eraser
   * 
   *  useful utility(functor) to delete the object 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  template <class TYPE>
  class Eraser: public std::unary_function<TYPE*,TYPE*>
  { 
  public:
    
    /** delete the object 
     *  @param obj pointer to object to be deleted 
     *  @return NULL pointer 
     */
    inline TYPE* operator() ( TYPE* obj ) const 
    { 
      if( 0 != obj ) { delete obj ; obj = 0 ; }
      return obj ;
    };
  };
  
  /** useful utility(templated function)  
   *  to delete the object through the pointer  
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   *
   *  @param obj pointer to object to be deleted 
   *  @return NULL pointer 
   */ 
  template <class TYPE>
  inline TYPE* Delete_Ptr( TYPE* obj )
  { 
    if( 0 != obj ) { delete obj ; obj = 0 ; }
    return obj ;
  }

  /** @struct FastCast
   *  Helpful utility to perfrom a fast cast 
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  template <class FROM,class TO> 
  struct FastCast : std::unary_function<FROM*,TO*>
  {
  public :
    /** the only one essential method 
     *  @param from  object to be cast
     *  @return the result of dynamic cast
     */
    inline TO* operator() ( FROM* from ) const 
    {
      if ( 0 == from ) { return  (TO*) 0 ; }
#ifdef GIGA_FASTCAST
      return  static_cast<TO*>  ( from ) ;
#else
      return  dynamic_cast<TO*> ( from ) ;
#endif 
    };
  };
  
  /** @class InstanceCounter 
   *  Static class used to instrument constructors 
   *  and destructors to search for memory leaks
   *
   *  @author Markus Frank
   *  @date   2002-07-17
   */
  template <class T> class InstanceCounter    {
    long m_count;
  public:
    long increment ()      { return ++m_count; }
    long decrement ()      { return --m_count; }
    long count     ()const { return   m_count; }
    InstanceCounter() : m_count(0) {};
    virtual ~InstanceCounter()   
    {
      if( 0 != m_count ) 
        {
          std::cout << "Number of objects of type: "
                    << System::typeinfoName(typeid(T))
                    << " created, but not destroyed:" 
                    << m_count
                    << std::endl;
        }
    };
  };
  
} ///< end of namespace 

#endif ///< GIGA_GIGAUTIL_H

