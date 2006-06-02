// $Id: Less.h,v 1.3 2006-06-02 16:18:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/12/08 17:46:22  ibelyaev
//  Win adaptation: VisualC++ 6.0 -> VisualC++ 7.1
// 
// ============================================================================
#ifndef RELATIONS_LESS_H 
#define RELATIONS_LESS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================


/** @file 
 *  
 *  Partial specialization of @p std::less structure for 
 *  @p SmartRef classes
 *  The overall result of these actions is that 
 *  comparison of @p SmartRef<TYPE> ojbects is 
 *  reduced to the comparison of raw pointers 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/05/2002
 */

namespace std 
{
  /** @struct less Less.h Relations/Less.h
   *  
   *  Partial specialiations of std::less.
   *  Needed to suppress numerous warning messages at Linux 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   08/05/2002
   */  
  template<class TYPE> 
  struct less< SmartRef<TYPE> >
    : std::binary_function<const SmartRef<TYPE>,const SmartRef<TYPE>,bool>
  {
    /** the only one essential method 
     *  comparison of smart references through 
     *  dereference to underlying raw pointers
     *  @param obj1 the first smart reference 
     *  @param obj2 the second smart reference 
     *  @return result of comparioson of raw pointers 
     */
    bool operator() ( const SmartRef<TYPE>& obj1 , 
                      const SmartRef<TYPE>& obj2 ) const
    { return std::less<const TYPE*>()( obj1 , obj2 ) ; };
  };
  
  // remove "const" qualifier
  template<class TYPE> 
  struct less<const SmartRef<TYPE> > 
    : public std::less<SmartRef<TYPE> > {};
  
}; // end of namespace std 



// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_LESS_H
// ============================================================================
