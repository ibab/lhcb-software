// $Id: Apply.h,v 1.1 2002-03-18 19:32:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_APPLY_H 
#define RELATIONS_APPLY_H 1
// Include files
#include "Relations/ObjectTypeTraits.h"

/** @file Apply.h Relations/Apply.h
 *  
 *  Auxillary function to set the environemnt 
 *  for smart references. 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   28/01/2002
 */

// forward declarations
template <class TYPE>
class     SmartRef        ; // from GaudiKernel package
template <class TYPE>
class     SmartRefVector  ; // from GaudiKernel package
class     DataObject      ; // from GaudiKernel package
class     ContainedObject ; // from GaudiKernel package


namespace Relations
{
  
  /** @function apply Apply.h Relations/Apply.h
   *  
   *  Auxillary function to set the environemnt for smart references. 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   07/02/2002
   */
  template<class TYPE, class OBJECT> 
  inline                TYPE&                 
  apply
  (                TYPE & type , OBJECT*        ) { return type            ; }
  
  template<class TYPE, class OBJECT>
  inline const          TYPE& 
  apply
  ( const          TYPE & type , OBJECT*        ) { return type            ; }
  
  template<class TYPE, class OBJECT>
  inline       SmartRef<TYPE>& 
  apply
  (       SmartRef<TYPE>& type , OBJECT* object ) { return type ( object ) ; }

  template<class TYPE, class OBJECT>
  inline const SmartRef<TYPE>& 
  apply
  ( const SmartRef<TYPE>& type , OBJECT* object ) { return type ( object ) ; }

}; ///< end of namespace Raltions 

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_APPLY_H
// ============================================================================
