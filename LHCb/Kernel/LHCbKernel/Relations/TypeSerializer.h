// $Id: TypeSerializer.h,v 1.1 2002-04-24 21:16:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_TYPESERIALIZER_H 
#define RELATIONS_TYPESERIALIZER_H 1
// Include files
// forward declarations 
class StreamBuffer    ;    // from GaudiKernel Package

/** @file TypeSerializer.h
 *  
 *  definition and specializations of serliazations methods 
 *
 *  @author Vanya Belyaev Ivan Belyaev
 *  @date   24/03/2002
 */

namespace Relations
{

  /// forward declaration
  template <class OBJECT> struct ObjectTypeTraits;
  
  /** @struct TypeSerializer 
   * 
   *  A helper structure to control the serialization of objects
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */
  template<class OBJECT>
  struct TypeSerializer
  {
    /// object type 
    typedef OBJECT          TYPE  ;
    
    /** object serialization to Gaudi output stream (write)
     *  @see StreamBuffer 
     *  @param buffer reference to Gaudi output stream 
     *  @param object const reference to the object 
     *  @return       reference to Gaudi output stream 
     */
    static 
    StreamBuffer& serialize
    ( StreamBuffer& buffer , const TYPE& object )
    { return buffer << object ; }
    
    /** object serialization from Gaudi input stream  (read) 
     *  @see StreamBuffer 
     *  @param buffer reference to Gaudi input stream 
     *  @param object reference to the object 
     *  @return       reference to Gaudi input stream 
     */
    static 
    StreamBuffer& serialize
    ( StreamBuffer& buffer ,       TYPE& object )
    { return buffer >> object ; }
    
  };
  
}; // end of namespace Serializer


// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_TYPESERIALIZER_H
// ============================================================================
