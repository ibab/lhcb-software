// $Id: Less.h,v 1.3 2002-05-08 12:51:20 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_LESS_H 
#define RELATIONS_LESS_H 1
// Include files
// STD & STL 
#include <functional>
#ifndef WIN32

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
   *  comparison of smart references through their pointers
   *  @param obj1 the first smart reference 
   *  @param obj1 the first smart reference 
   *  @return result of comparioson of raw pointers 
   */
  bool operator() ( const SmartRef<TYPE>& obj1 , 
                    const SmartRef<TYPE>& obj2 ) const
  {
    return ( const TYPE*) obj1 < (const TYPE*) obj2 ;    
  };
};

/// the same for "const version"
template<class TYPE> 
struct std::less<const SmartRef<TYPE> >
  : public std::less<SmartRef<TYPE> > {};

#endif 

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_LESS_H
// ============================================================================
