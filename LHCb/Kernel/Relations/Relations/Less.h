// $Id: Less.h,v 1.1.1.1 2004-07-21 07:57:25 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_LESS_H 
#define RELATIONS_LESS_H 1
// Include files
// STD & STL 
#include <functional>

/** @file 
 *  
 *  Partial specialization of @p std::less structure for 
 *  @p SmartRef classes for no-MicroSoft compilers and 
 *  defininion of templated "operator<" for MicroSoft compiler 
 *  The overall result of these actions is that 
 *  comparison of @p SmartRef<TYPE> ojbects is 
 *  reduced to the comparison of raw pointers 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/05/2002
 */

#ifndef WIN32 

namespace std {
/** @struct less Less.h Relations/Less.h
 *  
 *  Partial specialiations of std::less.
 *  Needed to suppress numerous warning messages at Linux 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/05/2002
 */
template<class TYPE> 
struct std::less<SmartRef<TYPE> >
  : public std::binary_function<const SmartRef<TYPE>,const SmartRef<TYPE>,bool>
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
struct std::less<const SmartRef<TYPE> > : public std::less<SmartRef<TYPE> > {};
}

#else

/** For MicroSoft compiler we could not use the partial 
 *  specialization of templated std::less, but we could
 *  use the templated operator!
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/05/2002
 */
template <class TYPE> bool 
operator<( const SmartRef<TYPE>& obj1 , 
           const SmartRef<TYPE>& obj2 ) 
{ return std::less<const TYPE*>()( obj1 , obj2 ) ; };

#endif 

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_LESS_H
// ============================================================================
