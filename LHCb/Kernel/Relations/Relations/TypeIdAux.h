// $Id: TypeIdAux.h,v 1.1.1.1 2004-07-21 07:57:27 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/05/10 12:29:43  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
// 
// ============================================================================
#ifndef RELATIONS_TYPEIDAUX_H 
#define RELATIONS_TYPEIDAUX_H 1
// Include files
// Relations 
#include "Relations/RelationUtils.h"
// GaudiKernel
#include "GaudiKernel/System.h"

/** @file 
 *  @brief Compiler-dependent implementation of TypeIdAux
 *
 *  Unfortunately the implementation is platform dependent 
 *  Since partial template specialiazation is not implemented 
 *  for MicroCoft C++ compiler one need to "mimic" it using the full 
 *  specialization and embedded templated classes. The trick is 
 *  imported from <a href="http://www.boost.org">BOOST library</A>
 *
 *  @see TypeIdAux 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/05/2002
 */
namespace Relations
{
  namespace detail 
  {
    /** @struct TypeIdAux TypeId.h Relations/TypeId.h
     *  
     *  Helper class to define the unique type identifier for 
     *  objects of type @p TYPE
     *  
     *  The unique type identifier is extracted from 
     *  \p TYPE::classID() methods for *storable* objects 
     *  and it is evaluated from typename for *non-storable* objects
     * 
     *  It is in the spirit of 
     *  <a href="http://www.boost.org">BOOST library</A>
     *  and the book of Andrey Alexandrescu 
     *  <a href="http://www.awl.com/cseng/titles/0-201-70431-5">
     *  Modern C++ Design: Generic Programming and Design Patterns Applied</a>
     *
     *  Unfortunately the implementation is platform dependent 
     *  Since partial template specialiazation is not implemented 
     *  for MicroCoft C++ compiler one need to "mimic" it using the full 
     *  specialization and embedded templated classes. The trick is 
     *  imported from <a href="http://www.boost.org">BOOST library</A>
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   08/05/2002
     */
#ifdef WIN32
    template<bool flag>     struct TypeIdAux 
    {
      template <class TYPE> struct ID
      {
        static const CLID& id() 
        {
          static const CLID s_id = 
            clid( System::typeinfoName( typeid( TYPE ) ) );
          return            s_id ;
        };  
      };
    };
    template<>              struct TypeIdAux<true> 
    {
      template <class TYPE> struct ID
      {
        static const CLID& id() 
        {
          static const CLID s_id = TYPE::classID() ;
          return            s_id ;
        };  
      };
    };    
#else
    template<bool flag, class TYPE>     struct TypeIdAux 
    {
      static const CLID& id() 
      {
        static const CLID s_id = 
          clid( System::typeinfoName( typeid( TYPE ) ) );
        return            s_id ;
      };  
    };
    template<class TYPE>                struct TypeIdAux<true,TYPE>
    { static const CLID& id() { return            TYPE::classID(); }; };
#endif
    
  }; // end of namespace detail
  
}; // end of namespace Relations 

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPEIDAUX_H
// ============================================================================
