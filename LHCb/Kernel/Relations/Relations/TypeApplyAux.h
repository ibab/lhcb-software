// $Id: TypeApplyAux.h,v 1.2 2004-12-08 17:46:22 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_TYPEAPPLYAUX_H 
#define RELATIONS_TYPEAPPLYAUX_H 1
// ============================================================================
// Include files
// ============================================================================

/** @file 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch 
 *  @date   09/05/2002
 */

namespace Relations
{
  /** @struct TypeApplyAux TypeApply.h Relations/TypeApply.h
   *  
   *  A helper stucture to handle the specific serialization
   *  @see ObjectTypeTraits
   *
   *  All this complex stuff needed only just to set 
   *  the environment for Smart References in a generic way
   *  without explicit reference to smart reference definiton
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   24/03/2002
   */
  namespace detail
  {
    template <class TYPE>   struct TypeApplyAux
    {  
      /// "apply" during writing from data object
      static const TYPE& apply( const TYPE&            typ , 
                                const DataObject*          ) 
      { return typ  ; }
      /// "apply" during writing from contained object 
      static const TYPE& apply( const TYPE&            typ , 
                                const ContainedObject*     ) 
      { return typ  ; } 
      /// "apply" during reading to   data       object 
      static       TYPE& apply(       TYPE&            typ ,       
                                      DataObject*          ) 
      { return typ  ; }
      /// "apply" during reading to   contained  object 
      static       TYPE& apply(       TYPE&            typ ,       
                                      ContainedObject*     ) 
      { return typ  ; } 
    };
    template <class TYPE>   struct TypeApplyAux<SmartRef<TYPE> >
    {  
      /// actual type used 
      typedef SmartRef<TYPE>          Type;
      /// "apply" during writing from data object
      static const Type&  apply( const Type&            typ , 
                                 const DataObject*      obj ) 
      { return typ( obj ) ; }
      /// "apply" during writing from contained object 
      static const Type& apply( const Type&            typ , 
                                const ContainedObject* obj ) 
      { return typ( obj ) ; }      
      /// "apply" during reading to   data       object 
      static       Type& apply(       Type&            typ ,
                                      DataObject*      obj )  
      { return typ( obj ) ; }      
      /// "apply" during reading to   contained  object 
      static       Type& apply(       Type&            typ ,       
                                      ContainedObject* obj ) 
      { return typ( obj ) ; } 
    };
    
  }; // end of namespace detail
  
}; // end of namespace Relations

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPEAPPLYAUX_H
// ============================================================================
