// $Id: TypeSerializers.h,v 1.1.1.1 2004-07-21 07:57:27 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/05/10 12:29:43  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
// 
// ============================================================================
#ifndef RELATIONS_TYPESERIALIZERS_H 
#define RELATIONS_TYPESERIALIZERS_H 1
// Include files
// forward declarations from GaudiKernel
class StreamBuffer ;
template <class TYPE> 
class SmartRef ;

/** @file 
 * 
 *  Specializations serialization opertators to/from 
 *  Gaudi Streams. Duw to some strange nehaviour of 
 *  MicroSoft compiler the "global" operators are not 
 *  visible in the templated context. Thus one need to have 
 *  "explicite" redirection.
 *  These "local" definitions are declared in the 
 *  namespace Relations,  thus it do not interfere 
 *  with any other global definitions
 *
 *  @see StreamBuffer 
 *  @see SmartRef 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   10/05/2002
 */
namespace Relations
{
  
#ifdef WIN32 
  /** object ( @p SmartRef ) serialization to Gaudi 
   *  output stream (write)
   *  @see StreamBuffer 
   *  @see SmartRef 
   *  @param buffer reference to Gaudi output stream 
   *  @param object const reference to the object 
   *  @return       reference to Gaudi output stream 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   10/05/2002
   */
  template <class TYPE> 
  StreamBuffer& 
  operator<<( StreamBuffer& buffer , const SmartRef<TYPE>& object ) 
  { return object.writeRef( buffer ) ; }
  
  /** object ( @p SmartRef ) serialization from Gaudi 
   *  input stream (read)
   *  @see StreamBuffer 
   *  @see SmartRef 
   *  @param buffer reference to Gaudi output stream 
   *  @param object const reference to the object 
   *  @return       reference to Gaudi output stream 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   10/05/2002
   */
  template <class TYPE> 
  StreamBuffer& 
  operator>>( StreamBuffer& buffer ,       SmartRef<TYPE>&       object  ) 
  { return object.readRef( buffer ) ; }
#endif
  
}; // end of namespace 

// ============================================================================
// The END
// ============================================================================
#endif // RELATIONS_TYPESERIALIZERS_H
// ============================================================================
