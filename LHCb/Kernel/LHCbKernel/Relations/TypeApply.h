// $Id: TypeApply.h,v 1.3 2002-04-27 09:48:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/04/25 08:44:05  ibelyaev
//  bug fix for Win2K
//
// Revision 1.1  2002/04/24 21:16:40  ibelyaev
//  fix one more problem for Win2K
//
// ============================================================================
#ifndef RELATIONS_TypeApply_H 
#define RELATIONS_TypeApply_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// GaudiKernel

/** @file TypeApply.h Relations/TypeApply.h
 *  
 *  definition of helper structure for "applying" of 
 *  DataObject/Contained object 
 *  information for serializations 
 *
 *  @author Vanya Belyaev Ivan Belyaev
 *  @date   24/03/2002
 */

#ifdef WIN32 
#include "Relations/TypeApply_WIN32.h"  // use another implementation for WIN32!
#else
/// forwarde declaration of SmartRef class 
template <class TYPE> class SmartRef;
namespace Relations
{  
  /** @struct TypeApply TypeApply.h Relations/TypeApply.h
   *  
   *  A helper stucture to handle the specific serialization
   *  @see ObjectTypeTraits
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>  
  struct TypeApply
  { 
    /// true type (not used)
    typedef OBJECT          TYPE  ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  @attention it is empty! 
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type          ; }
    /** mandatory method for serialization of SmartRef structure (write)
     *  @attention it is empty! 
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type          ; }
  };
  
  /** @struct TypeApply<const OBJECT>
   *  
   *  partial specialization for const objects
   *  @see ObjectTypeTraits
   *  @see TypeApply
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>
  struct TypeApply<const OBJECT>
  { 
    /// true type (not used)
    typedef const OBJECT    TYPE  ;
    /// object type 
    typedef TYPE            Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */   
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return Apply::apply( type , object ); };    
    /** mandatory method for serialization of SmartRef structure (write)
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return Apply::apply( type , object ); };
  };
  
  /** @struct TypeApply<OBJECT*>
   *  
   *  partial specialization for pointers
   *  @see ObjectTypeTraits
   *  @see TypeApply
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>
  struct TypeApply<OBJECT*>
  { 
    /// true type (not used)
    typedef OBJECT*         TYPE  ;
    /// object type 
    typedef OBJECT          Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return Apply::apply( type , object ); };    
    /** mandatory method for serialization of SmartRef structure (write)
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return Apply::apply( type , object ); };
  };

  /** @struct TypeApply<OBJECT&>
   *  
   *  partial specialization for references
   *  @see ObjectTypeTraits
   *  @see TypeApply
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>
  struct TypeApply<OBJECT&>
  { 
    /// true type (not used)
    typedef OBJECT&         TYPE  ;
    /// object type 
    typedef OBJECT          Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return Apply::apply( type , object ); };    
    /** mandatory method for serialization of SmartRef structure (write)
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return Apply::apply( type , object ); };
  };
  
  /** @struct TypeApply<const OBJECT*>
   *  
   *  partial specialization for pointers to const objects
   *  @see ObjectTypeTraits
   *  @see TypeApply
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>
  struct TypeApply<const OBJECT*>
  { 
    /// true type (not used)
    typedef const OBJECT*   TYPE  ;
    /// object type 
    typedef OBJECT          Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    
    /** mandatory method for serialization of SmartRef structure (read)
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return Apply::apply( type , object ); };
    
    /** mandatory method for serialization of SmartRef structure (write)
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return Apply::apply( type , object ); };
  };
  
  /** @struct TypeApply<const OBJECT&>
   *  
   *  partial specialization for references to const objects
   *  @see ObjectTypeTraits
   *  @see TypeApply
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>
  struct TypeApply<const OBJECT&>
  { 
    /// true type (not used)
    typedef const OBJECT&   TYPE  ;
    /// object type 
    typedef OBJECT          Type  ;
    /// own type 
    typedef TypeApply<Type> Apply ;
    /** mandatory method for serialization of SmartRef structure (read)
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return Apply::apply( type , object ); };
    
    /** mandatory method for serialization of SmartRef structure (write)
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return Apply::apply( type , object ); };
  };
  

  /** @struct TypeApply<SmartRef<OBJECT>>
   *  
   *  partial specialization for smart references 
   *  @see ObjectTypeTraits
   *  @see TypeApply
   *  @attention the only one non-trivial specialization! 
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>
  struct TypeApply< SmartRef<OBJECT> >
  { 
    /// true type (not used)
    typedef SmartRef<OBJECT> TYPE  ;
    /// object type 
    typedef SmartRef<OBJECT> Type  ;
    /// own type 
    typedef TypeApply<Type>  Apply ;    
    /** mandatory method for serialization of SmartRef structure (read)
     *  @see SmartRef
     *  @attention the only one non-trivial 'apply'-operation
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return  type( object ) ; };
    /** mandatory method for serialization of SmartRef structure (write)
     *  @see SmartRef
     *  @attention the only one non-trivial 'apply'-operation
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return  type( object ) ; };
  };
  
};

#endif 

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_TypeApply_H
// ============================================================================
