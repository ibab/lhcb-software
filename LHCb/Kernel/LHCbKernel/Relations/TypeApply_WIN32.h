// $Id: TypeApply_WIN32.h,v 1.1 2002-04-24 21:16:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/03 15:35:16  ibelyaev
// essential update and redesing of all 'Relations' stuff
// 
// ============================================================================
#ifndef RELATIONS_TypeAppLY_WIN32_H 
#define RELATIONS_TypeAppLY_WIN32_H 1
// Include files

/** @file TypeApply_WIN32.h
 *
 *  Implementation of TypeApply structure for WIN32 platform
 *
 *  @warning the content of the file is platform-dependent! 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */
namespace Relations
{
  
  /** @struct TypeApply TypeApply_WIN32.h Relations/TypeApply_Win32.h
   *  
   *  A helper stucture to handle the specific serialization.
   *  Generic definition is valid for 'SmartRef' objects
   *
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>  
  struct TypeApply
  { 
    /// true type (not used) 
    typedef OBJECT          TYPE  
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  @see SmartRef
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* object )
    { return type( object ); }    
    /** mandatory method for serialization of SmartRef structure (write)
     *  @see SmartRef
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ* object )
    { return type( object ); }
  };

  /** @struct TypeApply<bool>
   *  
   *  full specialization for type 'bool'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<bool>
  {
    /// true type (not used)
    typedef bool            TYPE  ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  /** @struct TypeApply<char>
   *  
   *  full specialization for type 'char'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<char>
  {
    /// true type (not used)
    typedef char            TYPE  ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  /** @struct TypeApply<unsigned char>
   *  
   *  full specialization for type 'unsigned char'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<unsigned char>
  {
    /// true type (not used)
    typedef unsigned char   TYPE  ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  /** @struct TypeApply<short>
   *  
   *  full specialization for type 'short'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<short>
  {
    /// true type (not used)
    typedef short           TYPE 
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  /** @struct TypeApply<unsigned short>
   *  
   *  full specialization for type 'unsigned short'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<unsigned short>
  {
    /// true type (not used)
    typedef unsigned short  TYPE  ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  
  /** @struct TypeApply<long>
   *  
   *  full specialization for type 'long'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<long>
  {
    /// true type (not used)
    typedef long            TYPE  ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };
  
  /** @struct TypeApply<unsigned long>
   *  
   *  full specialization for type 'unsigned long'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<unsigned long>
  {
    /// true type (not used)
    typedef unsigned long   TYPE ;
    /// object type 
    typedef TYPE            Type ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  /** @struct TypeApply<float>
   *  
   *  full specialization for type 'float'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<float>
  {
    /// true type (not used)
    typedef float           TYPE ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };
  
  /** @struct TypeApply<double>
   *  
   *  full specialization for type 'double'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<double>
  {
    /// true type (not used)
    typedef double          TYPE ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };
  
  /** @struct TypeApply<long double>
   *  
   *  full specialization for type 'long double'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<long double>
  {
    /// true type (not used)
    typedef long double     TYPE ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };
  
  /** @struct TypeApply<std::string>
   *  
   *  full specialization for type 'std::string'
   *
   *  @see TypeApply
   *  @see ObjectTypeTraits
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <>
  struct TypeApply<std::string>
  {
    /// true type (not used)
    typedef std::string     TYPE  ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  empty method here!
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  empty method here!
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_APPLY_WIN32_H
// ============================================================================
