// $Id: TypeSerializer.h,v 1.1.1.1 2004-07-21 07:57:27 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/05/10 12:29:43  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
//
// ============================================================================
#ifndef RELATIONS_TYPESERIALIZER_H 
#define RELATIONS_TYPESERIALIZER_H 1
// Include files
#include "Relations/PragmaWarnings.h"
#include "Relations/TypePersistent.h"
#include "Relations/TypeSerializers.h"
// forward declarations 
class StreamBuffer    ;    // from GaudiKernel Package

/** @file TypeSerializer.h
 *  
 *  definition and specializations of serliazations methods 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */
namespace Relations
{
  /** @struct TypeSerializer 
   * 
   *  A helper structure to control the serialization of objects
   *
   *  It is in the spirit of 
   *  <a href="http://www.boost.org">BOOST library</A>
   *  and the book of Andrey Alexandrescu 
   *  <a href="http://www.awl.com/cseng/titles/0-201-70431-5">
   *  Modern C++ Design: Generic Programming and Design Patterns Applied</a>
   * 
   *  Here @p Type evaluates to @p TYPE2 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */
  template<class TYPE>
  struct TypeSerializer
  {
    /// actual type of objects to be subjected serialization
    typedef typename TypePersistent<TYPE>::Result  Type ;
    /** object serialization to Gaudi output stream (write)
     *  @see StreamBuffer 
     *  @param buffer reference to Gaudi output stream 
     *  @param object const reference to the object 
     *  @return       reference to Gaudi output stream 
     */
    static 
    StreamBuffer& serialize ( StreamBuffer& buffer , const Type& object )
    { return ( buffer << object ) ; }
    
    /** object serialization from Gaudi input stream  (read) 
     *  @see StreamBuffer 
     *  @param buffer reference to Gaudi input stream 
     *  @param object reference to the object 
     *  @return       reference to Gaudi input stream 
     */
    static 
    StreamBuffer& serialize ( StreamBuffer& buffer ,       Type& object )
    { return ( buffer >> object ) ; }
  };
  
}; // end of namespace Serializer


// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_TYPESERIALIZER_H
// ============================================================================
