// $Id: ObjectTypeTraits.h,v 1.1 2002-03-18 19:32:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_ObjectTypeTraits_H
#define RELATIONS_ObjectTypeTraits_H 1

// forward declarations
template <class TYPE>
class     SmartRef        ; // from GaudiKernel package
class     DataObject      ; // from GaudiKernel package
class     ContainedObject ; // from GaudiKernel package

namespace Relations
{

  /** @struct ObjectTypeTraits
   *
   *  A helper structure to define the Object types
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */
  template <class OBJECT>
  struct ObjectTypeTraits
  {
    typedef OBJECT         TYPE      ;
    typedef SmartRef<TYPE> Type      ;
    static  int id() { return  OBJECT::classID() ;} ;
  };
  
  template <class OBJECT>
  struct ObjectTypeTraits<SmartRef<OBJECT> >
  {
    typedef OBJECT         TYPE      ;
    typedef SmartRef<TYPE> Type      ;
    static int id() { return  OBJECT::classID() ;} ;
  };
  
  template <>
  struct ObjectTypeTraits<char>
  {
    typedef        char    TYPE      ;
    typedef        TYPE    Type      ;
    static unsigned long   id() { return 1000000 + 1 ; } ;
  };
  
  template <>
  struct ObjectTypeTraits<unsigned char>
  {
    typedef unsigned char    TYPE      ;
    typedef          TYPE    Type      ;
    static unsigned long   id() { return 1000000 + 2 ; } ;
  };

  template <>
  struct ObjectTypeTraits<short>
  {
    typedef        short    TYPE      ;
    typedef        TYPE     Type      ;
    static unsigned long   id() { return 1000000 + 3 ; } ;
  };
  
  template <>
  struct ObjectTypeTraits<unsigned short>
  {
    typedef unsigned short   TYPE      ;
    typedef          TYPE    Type      ;
    static unsigned long   id() { return 1000000 + 4 ; } ;
  };

  template <>
  struct ObjectTypeTraits<long>
  {
    typedef        long     TYPE      ;
    typedef        TYPE     Type      ;
    static unsigned long   id() { return 1000000 + 5 ; } ;
  };
  
  template <>
  struct ObjectTypeTraits<unsigned long>
  {
    typedef unsigned long    TYPE      ;
    typedef          TYPE    Type      ;
    static int id() { return 1000000 + 6 ; } ;
  };

  template <>
  struct ObjectTypeTraits<float>
  {
    typedef        float    TYPE      ;
    typedef        TYPE     Type      ;
    static unsigned long   id() { return 1000000 + 7 ; } ;
  };
  
  template <>
  struct ObjectTypeTraits<double>
  {
    typedef          double  TYPE      ;
    typedef          TYPE    Type      ;
    static unsigned long   id() { return 1000000 + 8 ; } ;
  };
  
  template <>
  struct ObjectTypeTraits<long double>
  {
    typedef long     double  TYPE      ;
    typedef          TYPE    Type      ;
    static  unsigned long  id() { return 1000000 + 9 ; } ;
  };

  template <>
  struct ObjectTypeTraits<std::string>
  {
    typedef          std::string  TYPE      ;
    typedef          TYPE         Type      ;
    static unsigned long   id() { return 1000000 + 10 ; } ;
  };
  
};

// ============================================================================
//  The End
// ============================================================================
#endif // RELATIONS_ObjectTypeTraits_H
// ============================================================================
