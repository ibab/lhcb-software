// $Id: TypeDataObject.h,v 1.1 2002-05-10 12:29:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_TYPEDATAOBJECT_H 
#define RELATIONS_TYPEDATAOBJECT_H 1
// Include files
// Relations 
#include "Relations/TypeConversion.h"
// GaudiKernel 
#include "GaudiKernel/DataObject.h"
class DataObject;

namespace Relations
{
  
  /** @struct TypeDataObject TypeDataObject.h Relations/TypeDataObject.h
   *  
   *  Primitive class to detect at compile type if 
   *  @p TYPE cound be converted to @p DataObject
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
   *  const bool yes = TypeDataObject<ObjectVector<MCParticle> >::value ;
   *  @endcode
   *  Here @p yes evaluates to @p true 
   *
   *  @see TypeConversion
   *  @see DataObject
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   08/05/2002
   */
  template <class TYPE>
  struct TypeDataObject
  {
    /// enum which holds the actual result of evaluation
    enum { value = TypeConversion<const TYPE*,const DataObject*>::exists };
  };
  
}; // end of the namespace

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPEDATAOBJECT_H
// ============================================================================
