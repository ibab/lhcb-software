// $Id: TypeConversion.h,v 1.2 2002-04-25 08:49:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/25 08:44:05  ibelyaev
//  bug fix for Win2K
// 
// ============================================================================
#ifndef RELATIONS_TYPECONVERSION_H 
#define RELATIONS_TYPECONVERSION_H 1
// Include files
#include "Relations/PragmaWarnings.h"

namespace Relations
{  
  /** @class TypeConversion TypeConversion.h Relations/TypeConversion.h
   *
   *  Helper class which allows to determined at compile time 
   *  if class "F" could be converted to class "T".
   *  Also it detects the public inheritance 
   *  
   *  Implementation is imported form 
   *  Andrei Alexandrescu's book 
   *  "Modern C++ Design: Generic programming and design pattern applied"
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   25/04/2002
   */
  template<class F, class T>
  class TypeConversion
  {
  private:
    typedef char Small ;
    class   Big   { char m_c[2];} ;
    static  Small Test  ( T   )   ;
    static  Big   Test  ( ... )   ;
    static  F     MakeF ()        ;
  public:
    enum { exists = sizeof( Test( MakeF() ) ) == sizeof( Small ) };
    /// need at least one puiblic method to please the compiler 
    static bool convertable() { return exists ; }
  };
  
}; // end of namespace

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_TYPECONVERSION_H
// ============================================================================
