// $Id: Less.h,v 1.1 2002-03-18 19:32:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_LESS_H 
#define RELATIONS_LESS_H 1
// Include files
// forward declarations
template <class TYPE>
class SmartRef            ; // from GaudiKernel

/** @file Less.h Relations/Less.h
 *  
 *  comparison operator for smart reference objects 
 *  through ther dereference to raw pointers 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   28/01/2002
 */

/** The template operator for comparion of smart references
 *  through their dereference to raw C++ pointers 
 *   
 *  @param object1 the first  object 
 *  @param object2 the second object
 *  @return "true" if the first object "is less than" the second one
 *          overwise return "false"
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   28/01/2002
 */
template <class TYPE>
inline bool operator< (const SmartRef<TYPE>& object1 , 
                       const SmartRef<TYPE>& object2 ) 
{ return (const TYPE*) object1 < (const TYPE*) object2 ; };

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_LESS_H
// ============================================================================
