// $Id: Apply_WIN32.h,v 1.1 2002-04-03 15:35:16 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_APPLY_WIN32_H 
#define RELATIONS_APPLY_WIN32_H 1
// Include files

/** @file Apply_WIN32.h
 *
 *  Implementation of Apply structure for WIN32 platform
 *
 *  @warning the content of teh file is platform-dependent! 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */
namespace Relations
{
  
  /** @struct Apply Apply.h Relations/Apply.h
   *  
   *  A helper stucture to handle the specific serialization.
   *  Generic definiiton is valid for 'SmartRef' objects
   *
   *  @warning WIN32 implementation! 
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>  
  struct Apply
  { 
    typedef OBJECT Type;
    
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

  
  template <>
  struct Apply<char>
  {
    typedef  char Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  template <>
  struct Apply<unsigned char>
  {
    typedef unsigned char Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  template <>
  struct Apply<short>
  {
    typedef  short Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  template <>
  struct Apply<unsigned short>
  {
    typedef unsigned short Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  
  template <>
  struct Apply<long>
  {
    typedef  long  Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };
  
  template <>
  struct Apply<unsigned long>
  {
    typedef unsigned long  Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };

  template <>
  struct Apply<float>
  {
    typedef  float  Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };
  
  template <>
  struct Apply<double>
  {
    typedef double  Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };
  
  template <>
  struct Apply<long double>
  {
    typedef long double  Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
    template<class OBJ>
    static const  Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type ; }
  };
  
  template <>
  struct Apply<std::string>
  {
    typedef std::string  Type ;
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type ; }
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
