// $Id: TypeApply.h,v 1.1.1.1 2004-07-21 07:57:26 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/05/10 12:29:43  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
//
// ============================================================================
#ifndef RELATIONS_TypeApply_H 
#define RELATIONS_TypeApply_H 1
// Include files
// relations 
#include "Relations/PragmaWarnings.h"
#include "Relations/TypePersistent.h"
#include "Relations/TypeStorable.h"
#include "Relations/TypeApplyAux.h"
// GaudiKernel
// forward declaration 
template <class TYPE> class SmartRef;
class DataObject      ;
class ContainedObject ;

/** @file TypeApply.h Relations/TypeApply.h
 *  
 *  definition of helper structure for "applying" of 
 *  DataObject/Contained object 
 *  information for serializations 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */

namespace Relations
{  
  
  /** @struct TypeApply TypeApply.h Relations/TypeApply.h
   *  
   *  A helper stucture to handle the specific serialization
   *  @see ObjectTypeTraits
   *
   *  All this complex stuff needed only just to set 
   *  the environment for Smart References in a generic way
   *  without explicit reference to smart reference definiton
   *
   *  Alternatively (and much simpler!) it could be implemented 
   *  through the partial template specialization for class 
   *  SmartRef, but unfortunately the partical specialzations 
   *  are not allowed by MicroSoft compiler.
   *  It is exactly the reason why we use several layers of 
   *  FULL specializations.
   * 
   *  @see SmartRef
   *  @see TypeApplyAux
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   24/03/2002
   */
  template <class TYPE>
  struct TypeApply
  {
    /// actual data type 
    typedef typename TypePersistent<TYPE>::Result      Type     ;
    /// actual "apply" operation
#ifdef WIN32 
    typedef typename 
    detail::TypeApplyAux<TypeStorable<TYPE>::value>::Apply<Type>  ApplyAux ;
#else 
    typedef typename detail::TypeApplyAux<Type>                   ApplyAux ;
#endif 
    /// "apply" during writing from data object    
    static const Type&   apply( const Type&            typ , 
                                const DataObject*      obj ) 
    { return   ApplyAux::apply( typ , obj ) ; }
    /// "apply" during writing from contained object    
    static const Type&   apply( const Type&            typ , 
                                const ContainedObject* obj ) 
    { return   ApplyAux::apply( typ , obj ) ; }
    /// "apply" during reading to   data       object    
    static       Type&   apply(       Type&            typ ,
                                      DataObject*      obj )  
    { return   ApplyAux::apply( typ , obj ) ; }
    /// "apply" during reading to   contained  object    
    static       Type&   apply(       Type&            typ ,       
                                      ContainedObject* obj ) 
    { return   ApplyAux::apply( typ , obj ) ; }
  private:
  };

}; // end of the namespace 


// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_TypeApply_H
// ============================================================================
