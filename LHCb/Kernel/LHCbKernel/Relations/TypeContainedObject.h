// $Id: TypeContainedObject.h,v 1.1 2002-05-10 12:29:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_TYPECONTAINEDOBJECT_H 
#define RELATIONS_TYPECONTAINEDOBJECT_H 1
// Include files
#include "Relations/TypeConversion.h"
// GaudiKernel 
#include "GaudiKernel/ContainedObject.h"
class ContainedObject;

namespace Relations
{
  
  /** @struct TypeContainedObject TypeContainedObject.h 
   *  
   *
   *  Primitive class to detect at compile type if 
   *  @p TYPE cound be converted to @p ContainedObject
   *
   *  It is in the spirit of 
   *  <a href="http://www.boost.org">BOOST library</A>
   *  and the book of Andrey Alexandrescu 
   *  <a href="http://www.awl.com/cseng/titles/0-20107043105">
   *  Modern C++ Design: Generic Programming and Design Patterns Applied</a>
   *
   *  Examples:
   *
   *  @code 
   *  const bool yes = TypeContainedObject<MCParticle>::value ;
   *  @endcode
   *  Here @p yes evaluates to @p true 
   *
   *  @see TypeConversion
   *  @see ContainedObject
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   08/05/2002
   */
  template <class TYPE>
  struct TypeContainedObject
  {
    /// enum which holds the actual result of evaluation
    enum { value = TypeConversion<const TYPE*,const ContainedObject*>::exists };
  };
  
}; // end of the namespace 


// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPEDATAOBJECT_H
// ============================================================================
