// $Id: Apply.h,v 1.2 2002-04-03 15:35:16 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_APPLY_H 
#define RELATIONS_APPLY_H 1
// Include files

/** @file Apply.h Relations/Apply.h
 *  
 *  definition of helper structure for "applying" of 
 *  DataObject/Contained object 
 *  information for serializations 
 *
 *  @author Vanya Belyaev Ivan Belyaev
 *  @date   24/03/2002
 */

#ifdef WIN32 
#include "Relations/Apply_WIN32.h"  // use another implementation for WIN32!
#else
namespace Relations
{  
  /** @struct Apply Apply.h Relations/Apply.h
   *  
   *  A helper stucture to handle the specific serialization
   *
   *  @author Vanya Belyaev Ivan Belyaev
   *  @date   24/03/2002
   */
  template <class OBJECT>
  struct Apply
  { 
    typedef OBJECT      Type  ;
    typedef Apply<Type> APPLY ;

    /** mandatory method for serialization of SmartRef structure (read)
     *  @see SmartRef
     *  @param  type      reference to the object
     *  @param  object    pointer to DataObject/ContainedObject
     *  @return reference to the object
     */
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ* /* object */ )
    { return type          ; }
    
    /** mandatory method for serialization of SmartRef structure (write)
     *  @see SmartRef
     *  @param  type      const reference to the object
     *  @param  object    pointer to const DataObject/ContainedObject
     *  @return const reference to the object
     */
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ* /* object */ )
    { return type          ; }
  };

  
  template <class OBJECT>
  struct Apply<const OBJECT>
  { 
    typedef OBJECT      Type  ;
    typedef Apply<Type> APPLY ;
    
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return APPLY::apply( type , object ); };
    
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return APPLY::apply( type , object ); };
  };
  
  template <class OBJECT>
  struct Apply<OBJECT*>
  { 
    typedef OBJECT      Type  ;
    typedef Apply<Type> APPLY ;
    
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return APPLY::apply( type , object ); };
    
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return APPLY::apply( type , object ); };
  };

  template <class OBJECT>
  struct Apply<OBJECT&>
  { 
    typedef OBJECT      Type  ;
    typedef Apply<Type> APPLY ;
    
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return APPLY::apply( type , object ); };
    
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return APPLY::apply( type , object ); };
  };

  template <class OBJECT>
  struct Apply<const OBJECT*>
  { 
    typedef OBJECT      Type  ;
    typedef Apply<Type> APPLY ;
    
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return APPLY::apply( type , object ); };
    
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return APPLY::apply( type , object ); };
  };
  
  template <class OBJECT>
  struct Apply<const OBJECT&>
  { 
    typedef OBJECT      Type  ;
    typedef Apply<Type> APPLY ;
    
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return APPLY::apply( type , object ); };
    
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return APPLY::apply( type , object ); };
  };

  template <class OBJECT>
  struct Apply<SmartRef<OBJECT> >
  { 
    typedef SmartRef<OBJECT> Type  ;
    typedef Apply<Type>      APPLY ;
    
    template<class OBJ>
    static        Type& apply(       Type& type  ,       OBJ*    object    )
    { return  type( object ) ; };
    
    template<class OBJ>
    static  const Type& apply( const Type& type  , const OBJ*    object    )
    { return  type( object ) ; };
  };
  
};
#endif 

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_APPLY_H
// ============================================================================
