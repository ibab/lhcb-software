// $Id: TypeId.h,v 1.1 2002-05-10 12:29:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_TYPEID_H 
#define RELATIONS_TYPEID_H 1
// Include files
// relations 
#include "Relations/TypeIdAux.h"
#include "Relations/TypeStorable.h"
// GaudiKernel
#include "GaudiKernel/ClassID.h" 
#include "GaudiKernel/System.h" 

namespace Relations
{
  /** @struct TypeId TypeId.h Relations/TypeId.h
   *  
   *  Helper class to define the unique type identification for 
   *  objects of type @p TYPE
   *  
   *  The unique type identifier is extracted from 
   *  \p TYPE::classID() methods for *storable* objects 
   *  and it is evaluated from typename for *non-storable* objects
   * 
   *  It is in the spirit of 
   *  <a href="http://www.boost.org">BOOST library</A>
   *  and the book of Andrey Alexandrescu 
   *  <a href="http://www.awl.com/cseng/titles/0-201-70431-5">
   *  Modern C++ Design: Generic Programming and Design Patterns Applied</a>
   *
   *  Examples: 
   *  @code
   *  const long id1 = TypeId<int>::id() ;     
   *  const long id2 = TypeId<MCParticle>::id() ;
   *  @endcode
   *
   *  @see TypeIdAux 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   08/05/2002
   */
  
  template<class TYPE>
  struct TypeId 
  {  
    /** retrieve the unique identifier for type @p TYPE  (static)
     *  @return unique identifier for type @p TYPE 
     */
    static const CLID& id() 
#ifdef WIN32 
    { return detail::TypeIdAux<TypeStorable<TYPE>::value>::ID<TYPE>::id() ; }
#else
    { return detail::TypeIdAux<TypeStorable<TYPE>::value,TYPE>::id()      ; }
#endif 
  };
  
}; // end of the namespace Relations 

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPEID_H
// ============================================================================
