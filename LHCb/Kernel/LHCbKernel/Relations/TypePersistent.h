// $Id: TypePersistent.h,v 1.1 2002-05-10 12:29:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_TYPEPERSISTENT_H 
#define RELATIONS_TYPEPERSISTENT_H 1
// Include files
// Relations 
#include "Relations/TypeStorable.h"
#include "Relations/TypeSelect.h"
// 
template <class TYPE> class SmartRef;

#include <map>
#include <set>
#include <deque>
#include <map>
#include <map>

namespace Relations 
{
  
  /** @struct TypePersistent TypePersistent.h Relations/TypePersistent.h
   *  
   *  Helper class to define the actual representation for
   *  objects of type @p TYPE, it is evaluated to 
   *  @p SmartRef<TYPE> for "sorable" types and to @p TYPE for other types 
   * 
   *  It is in the spirit of 
   *  <a href="http://www.boost.org">BOOST library</A>
   *  and the book of Andrey Alexandrescu 
   *  <a href="http://www.awl.com/cseng/titles/0-201-70431-5">
   *  Modern C++ Design: Generic Programming and Design Patterns Applied</a>
   *
   *  Examples:
   *  @code 
   *  typedef typename TypePersistent<MCParticle>::Result Type ;
   *  @endcode
   *  Here @p Type evaluates to @p SmartRef<MCParticle> 
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   08/05/2002
   */  
  template<class TYPE>
  struct TypePersistent
  {
    /** the actual typedef 
     *  it evaluates to @p SmartRef<TYPE> for "stoable types" 
     *  and to @p TYPE otherwise 
     */
    typedef typename 
    TypeSelect<TypeStorable<TYPE>::value,SmartRef<TYPE>,TYPE>::Result Result ;
  };
  
}; // end of the namespace

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPEPERSISTENT_H
// ============================================================================
