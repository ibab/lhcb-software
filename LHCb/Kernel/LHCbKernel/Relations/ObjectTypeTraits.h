// $Id: ObjectTypeTraits.h,v 1.2 2002-04-03 15:35:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_ObjectTypeTraits_H
#define RELATIONS_ObjectTypeTraits_H 1
// Include files
// STD & STL 
#include <functional>
#include <algorithm>
// Relations 
#include "Relations/Serializer.h"
#include "Relations/Apply.h"
// forward declarations
template <class TYPE>
class     SmartRef        ; // from GaudiKernel package
class     DataObject      ; // from GaudiKernel package
class     ContainedObject ; // from GaudiKernel package
class     StreamBuffer    ; // from GaudiKernel package

namespace Relations
{

  /** @struct ObjectTypeTraits
   *
   *  A helper structure to define the Object types
   *
   *  @warning due to well-know bug of Visual C++ compiler
   *           the implementation is platform-dependent!
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */
  template <class OBJECT>
  struct ObjectTypeTraits
  {
    typedef ObjectTypeTraits<OBJECT> Traits     ;
    typedef OBJECT                   TYPE       ;
    typedef SmartRef<TYPE>           Type       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;
    typedef std::less<const OBJECT*> Less       ;
    
    /** the unique class identification
     *  @return unique class identifier
     */
    static  int id() { return  OBJECT::classID() ;} ;
  };
  
#ifndef WIN32

  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT>
  {
    typedef ObjectTypeTraits<OBJECT> Traits     ;
    typedef Traits::TYPE             TYPE       ;
    typedef Traits::Type             Type       ;
    typedef Traits::Less             Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;
    
    static  int id() { return  ObjectTypeTraits<OBJECT>::id() ;} ;
  };

  template <class OBJECT>
  struct ObjectTypeTraits<OBJECT*>
  {
    typedef ObjectTypeTraits<OBJECT> Traits     ;
    typedef Traits::TYPE             TYPE       ;
    typedef Traits::Type             Type       ;
    typedef Traits::Less             Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;
    
    static  int id() { return  Traits::id() ;} ;
  };

  template <class OBJECT>
  struct ObjectTypeTraits<OBJECT&>
  {
    typedef ObjectTypeTraits<OBJECT> Traits     ;
    typedef Traits::TYPE             TYPE       ;
    typedef Traits::Type             Type       ;
    typedef Traits::Less             Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;

    static  int id() { return  Traits::id() ;} ;
  };

  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT*>
  {
    typedef ObjectTypeTraits<OBJECT> Traits     ;
    typedef Traits::TYPE             TYPE       ;
    typedef Traits::Type             Type       ;
    typedef Traits::Less             Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;

    static  int id() { return  Traits::id() ;} ;
  };

  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT&>
  {
    typedef ObjectTypeTraits<OBJECT> Traits     ;
    typedef Traits::TYPE             TYPE       ;
    typedef Traits::Type             Type       ;
    typedef Traits::Less             Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;

    static  int id() { return  Traits::id() ;} ;
  };

  template <class OBJECT>
  struct ObjectTypeTraits<SmartRef<OBJECT> >
  {
    typedef ObjectTypeTraits<OBJECT> Traits     ;
    typedef SmartRef<OBJECT>         TYPE       ;
    typedef SmartRef<OBJECT>         Type       ;
    typedef std::less<const OBJECT*> Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;
    
    static int id() { return  Traits::id() ;} ;
  };

#endif

  template <>
  struct ObjectTypeTraits<bool>
  {
    typedef ObjectTypeTraits<bool>   Traits     ;
    typedef        bool              TYPE       ;
    typedef        TYPE              Type       ;
    typedef std::less<Type>          Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;

    static int id() { return 1000000 + 0 ; } ;
  };


  template <>
  struct ObjectTypeTraits<char>
  {
    typedef ObjectTypeTraits<char>   Traits     ;
    typedef        char              TYPE       ;
    typedef        TYPE              Type       ;
    typedef std::less<Type>          Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;

    static int id() { return 1000000 + 1 ; } ;
  };

  template <>
  struct ObjectTypeTraits<unsigned char>
  {
    typedef ObjectTypeTraits<unsigned char> Traits     ;
    typedef unsigned char                   TYPE       ;
    typedef          TYPE                   Type       ;
    typedef std::less<Type>                 Less       ;
    typedef Serializer<Type>                SERIALIZER ;
    typedef Apply<Type>                     APPLY      ;

    static  int id() { return 1000000 + 2 ; } ;
  };

  template <>
  struct ObjectTypeTraits<short>
  {
    typedef ObjectTypeTraits<short>  Traits     ;
    typedef        short             TYPE       ;
    typedef        TYPE              Type       ;
    typedef std::less<Type>          Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;

    static int id() { return 1000000 + 3 ; } ;
  };

  template <>
  struct ObjectTypeTraits<unsigned short>
  {
    typedef ObjectTypeTraits<unsigned short> Traits     ;
    typedef unsigned short                   TYPE       ;
    typedef          TYPE                    Type       ;
    typedef std::less<Type>                  Less       ;
    typedef Serializer<Type>                 SERIALIZER ;
    typedef Apply<Type>                      APPLY      ;

    static  int id() { return 1000000 + 4 ; } ;
  };

  template <>
  struct ObjectTypeTraits<int>
  {
    typedef ObjectTypeTraits<int>           Traits     ;
    typedef          int                     TYPE       ;
    typedef          TYPE                    Type       ;
    typedef std::less<Type>                  Less       ;
    typedef Serializer<Type>                 SERIALIZER ;
    typedef Apply<Type>                      APPLY      ;

    static  int id() { return 1000000 + 5 ; } ;
  };

  template <>
  struct ObjectTypeTraits<unsigned int>
  {
    typedef ObjectTypeTraits<unsigned int>   Traits     ;
    typedef unsigned int                     TYPE       ;
    typedef          TYPE                    Type       ;
    typedef std::less<Type>                  Less       ;
    typedef Serializer<Type>                 SERIALIZER ;
    typedef Apply<Type>                      APPLY      ;

    static  int id() { return 1000000 + 6 ; } ;
  };

  template <>
  struct ObjectTypeTraits<long>
  {
    typedef ObjectTypeTraits<long>   Traits     ;
    typedef        long              TYPE       ;
    typedef        TYPE              Type       ;
    typedef std::less<Type>          Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;

    static  int id() { return 1000000 + 7 ; } ;
  };

  template <>
  struct ObjectTypeTraits<unsigned long>
  {
    typedef ObjectTypeTraits<unsigned long> Traits     ;
    typedef unsigned long                   TYPE       ;
    typedef          TYPE                   Type       ;
    typedef std::less<Type>                 Less       ;
    typedef Serializer<Type>                SERIALIZER ;
    typedef Apply<Type>                     APPLY      ;

    static  int id() { return 1000000 + 8 ; } ;
  };

  template <>
  struct ObjectTypeTraits<float>
  {
    typedef ObjectTypeTraits<float>  Traits     ;
    typedef        float             TYPE       ;
    typedef        TYPE              Type       ;
    typedef std::less<Type>          Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;

    static  int id() { return 1000000 + 9 ; } ;
  };

  template <>
  struct ObjectTypeTraits<double>
  {
    typedef ObjectTypeTraits<double> Traits     ;
    typedef          double          TYPE       ;
    typedef          TYPE            Type       ;
    typedef std::less<Type>          Less       ;
    typedef Serializer<Type>         SERIALIZER ;
    typedef Apply<Type>              APPLY      ;
    
    static  int id() { return 1000000 + 10 ; } ;

  };
  
  template <>
  struct ObjectTypeTraits<long double>
  {
    typedef ObjectTypeTraits<long double> Traits     ;
    typedef long     double               TYPE       ;
    typedef          TYPE                 Type       ;
    typedef std::less<Type>               Less       ;
    typedef Serializer<Type>              SERIALIZER ;
    typedef Apply<Type>                   APPLY      ;
    
    static  int id() { return 1000000 + 11 ; } ;
  };

  template <>
  struct ObjectTypeTraits<std::string>
  {
    typedef ObjectTypeTraits<std::string> Traits     ;
    typedef          std::string          TYPE       ;
    typedef          TYPE                 Type       ;
    typedef Serializer<Type>              SERIALIZER ;
    typedef Apply<Type>                   APPLY      ;
    
    struct Less
      : public std::binary_function<Type,Type,bool>
    {
      inline bool operator() ( const Type& s1 , const Type& s2 ) const
      {
        return std::lexicographical_compare( s1.begin() , s1.end() , 
                                             s2.begin() , s2.end() );
      };
    };
    
    static  int id() { return 1000000 + 12 ; } ;
  };

}; // end of namespace Relations

// ============================================================================
//  The End
// ============================================================================
#endif // RELATIONS_ObjectTypeTraits_H
// ============================================================================
