// $Id: TypeConversion.h,v 1.2 2006-03-29 14:38:23 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2004/07/21 07:57:27  cattanem
// first import of Kernel/Relations
//
// Revision 1.3  2002/05/10 12:29:43  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
//
// ============================================================================
#ifndef RELATIONS_TYPECONVERSION_H 
#define RELATIONS_TYPECONVERSION_H 1
// Include files
// Boost!
#include "boost/type_traits/conversion_traits.hpp" 

namespace Relations
{
  /** @struct TypeConversion TypeConversion.h Relations/TypeConversion.h
   *
   *  Helper class which allows to determined at compile time 
   *  if class @p FROM could be converted to class @p TO.
   *  Also it detects the public inheritance 
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
   *  const bool exist = TypeConversion<MCParticle*,ContainedObject*>::exists ;
   *  @endcode
   *  Here @p exist evaluates to @p true 
   * 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   25/04/2002
   */
  template <class FROM, class TO> struct TypeConversion
  {
    /// enum containes the actual result of converison test  
    enum
    { 
      exists = boost::is_convertible<FROM,TO>::value , // direct usage of Boost
      value  = boost::is_convertible<FROM,TO>::value   // direct usage of Boost 
    };
    /// need at least one public 'method' to suppress compiler warnings 
    static const bool foo() { return exists ; } ;
  };
  
}; // end of the namespace

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPECONVERSION_H
// ============================================================================
