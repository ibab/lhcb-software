// $Id: TypeSelect.h,v 1.2 2004-12-08 17:46:22 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_TYPESELECT_H 
#define RELATIONS_TYPESELECT_H 1
// ============================================================================
// Include files
// ============================================================================

namespace Relations
{ 
  /** @struct TypeSelect TypeSelect.h Relations/TypeSelect.h
   *  
   *  Simple generic utility for compile-time selection of 
   *  the types depending on value of boolean constant
   *
   *  It is in the spirit of 
   *  <a href="http://www.boost.org">BOOST library</A>
   *  and the book of Andrey Alexandrescu 
   *  <a href="http://www.awl.com/cseng/titles/0-201-70431-5">
   *  Modern C++ Design: Generic Programming and Design Patterns Applied</a>
   * 
   *  Examples:
   *  @code
   *  typedef TypeSelect<true,TYPE1,TYPE2>::Result Type ;
   *  @endcode
   *  Here @p Type evaluates to  @p TYPE1
   * 
   *  @code
   *  typedef TypeSelect<false,TYPE1,TYPE2>::Result Type ;
   *  @endcode
   *  Here @p Type evaluates to @p TYPE2 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   08/05/2002
   */
  template <bool flag, class FIRST, class SECOND>
  struct TypeSelect
  {
    // generic template "selects"  the second type 
    typedef SECOND   Result;
  };
  template <           class FIRST, class SECOND>
  struct TypeSelect<true,FIRST,SECOND>
  {
    // partical specialization "selects" the first type 
    typedef FIRST    Result;
  };
  
}; // end of namespace Relations 

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPESELECT_H
// ============================================================================
