// $Id: TypeStorable.h,v 1.1 2002-05-10 12:29:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_TYPESTORABLE_H 
#define RELATIONS_TYPESTORABLE_H 1
// Include files
#include "Relations/TypeDataObject.h"
#include "Relations/TypeContainedObject.h"

namespace Relations
{  
  /** @struct TypeStorable TypeStorable.h Relations/TypeStorable.h
   *  
   *  Primitive class to detect at compile type if 
   *  the type @p TYPE could be registered in the 
   *  Gaudi Transient Store
   *
   *  The implementation relies on the fact that 
   *  only objects which have @b public inheritance 
   *  from @p DataObject or @p ContainedObject could be registered in 
   *  Gaudi Transient Store 
   *  
   *  It is in the spirit of 
   *  <a href="http://www.boost.org">BOOST library</A>
   *  and the book of Andrey Alexandrescu 
   *  <a href="http://www.awl.com/cseng/titles/0-201-70431-5">
   *  Modern C++ Design: Generic Programming and Design Patterns Applied</a>
   *
   *  Examples:
   *
   *  @code
   *  const bool yes = TypeStorable<MCParticle>::value ;
   *  @endcode
   *  Here @p yes evaluates to @p true 
   *
   *  @see TypeDataObject
   *  @see TypeContainedObject
   *  @see DataObject
   *  @see ContainedObject
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   08/05/2002
   */
  template <class TYPE>
  struct TypeStorable
  {
    /// enum which holds the actual result of evaluation
    enum 
    { value = TypeContainedObject<TYPE>::value + TypeDataObject<TYPE>::value };
  };
  
}; // end of the namepace

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPESMART_H
// ============================================================================
