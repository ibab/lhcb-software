// $Id: TypeSelectAux.h,v 1.1 2002-05-10 12:29:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_TYPESELECTAUX_H 
#define RELATIONS_TYPESELECTAUX_H 1

/** @file 
 *  @brief Compiler-dependent implementation of TypeSelectAux 
 *
 *  Unfortunately the implementation is platform dependent 
 *  Since partial template specialiazation is not implemented 
 *  for MicroCoft C++ compiler one need to "mimic" it using the full 
 *  specialization and embedded templated classes. The trick is 
 *  imported from <a href="http://www.boost.org">BOOST library</A>
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   09/05/2002
 */

namespace Relations
{
  namespace detail
  {
    /** @struct TypeSelectAux TypeSelectAux.h
     *  Simple generic utility for compile-time selection of 
     *  the types depending on value of boolean constant
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   09/05/2002
     */
#ifdef WIN32 
    template <bool flag>  struct TypeSelectAux 
    {
      template <class FIRST, class SECOND>
      struct traits { typedef SECOND Result ; };
    };
    
    template <>           struct TypeSelectAux<true> 
    {
      template <class FIRST, class SECOND>
      struct traits { typedef FIRST Result ; };
    };
#endif
  }; // end of namespace detail
}; // end of namespace Relations

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPESELECTAUX_H
// ============================================================================
