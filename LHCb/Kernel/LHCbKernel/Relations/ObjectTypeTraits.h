// $Id: ObjectTypeTraits.h,v 1.9 2002-05-08 12:51:20 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/04/27 09:48:03  ibelyaev
//  reorder includes and forward declarations
//
// ============================================================================
#ifndef RELATIONS_ObjectTypeTraits_H
#define RELATIONS_ObjectTypeTraits_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL 
#include <functional>
#include <algorithm>
// Relations 
#include "Relations/TypeSerializer.h"
#include "Relations/TypeApply.h"
#include "Relations/RelationUtils.h"
#include "Relations/Less.h"
// forward declarations
template <class TYPE> class SmartRef ; // from GaudiKernel package

namespace Relations
{  
  /// forward declaration
  template <class OBJECT> struct TypeSerializer ;  
  /// forward declaration
  template <class OBJECT> struct TypeApply      ;
  
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
    /// true type (never used)
    typedef OBJECT                                TYPE       ;
    /// "traits'-provider, own type  
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
    /// actual "stored" type 
    typedef SmartRef<TYPE>                        Type       ;
#ifndef WIN32
    /// serializer
    typedef TypeSerializer<Type>                  Serializer ;
#else 
    /// serializer
    typedef RefSerializer<Type>                   Serializer ;
#endif 
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
#ifdef WIN32
    typedef std::less<const OBJECT*>              Less       ;
#else
    typedef std::less<Type>                       Less       ;
#endif  
    /// the unique class identification
    static  int id() { return  OBJECT::classID() ;}          ;
  };
  
#ifndef WIN32
  
  /** @struct ObjectTypeTraits<const OBJECT>
   *  partial specialisation for "const" types
   *  @see ObjectTypeTraits
   *  @warning not for MicroSoft compiler! 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT>
  {
    /// true type (never used)
    typedef const OBJECT                          TYPE       ;
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
    /// actual "stored" type 
    typedef typename Traits::Type                 Type       ;
    /// serializer 
    typedef typename Traits::Serializer           Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef typename Traits::Less                 Less       ;
    /// the unique class identification
    static  int id() { return  Traits::id() ;}               ;
  };
  
  /** @struct ObjectTypeTraits<OBJECT*>
   *  partial specialisation for pointers
   *  @see ObjectTypeTraits
   *  @warning not for MicroSoft compiler! 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<OBJECT*>
  {
    /// true type (never used)
    typedef OBJECT*                               TYPE       ;
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
    /// actual "stored" type 
    typedef typename Traits::Type                 Type       ;
    /// serializer 
    typedef typename Traits::Serializer           Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef Traits::Less                          Less       ;
    /// the unique class identification
    static  int id() { return  Traits::id() ;}               ;
  };
  
  /** @struct ObjectTypeTraits<OBJECT&>
   *  partial specialisation for references
   *  @see ObjectTypeTraits
   *  @warning not for MicroSoft compiler! 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<OBJECT&>
  {
    /// true type (never used)
    typedef OBJECT&                               TYPE       ;    
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
    /// actual "stored" type 
    typedef typename Traits::Type                 Type       ;
    /// serializer 
    typedef typename Traits::Serializer           Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef typename Traits::Less                 Less       ;
    /// the unique class identification
    static  int id() { return  Traits::id() ;}               ;
  };
  
  /** @struct ObjectTypeTraits<const OBJECT*>
   *  partial specialisation for pointers to const
   *  @see ObjectTypeTraits
   *  @warning not for MicroSoft compiler! 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT*>
  {
    /// true type (never used)
    typedef const OBJECT*                         TYPE       ;    
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
    /// actual "stored" type 
    typedef typename Traits::Type                 Type       ;
    /// serializer 
    typedef typename Traits::Serializer           Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef typename Traits::Less                 Less       ;
    /// the unique class identification
    static  int id() { return  Traits::id() ;}               ;
  };
  
  /** @struct ObjectTypeTraits<const OBJECT&>
   *  partial specialisation for const references
   *  @see ObjectTypeTraits
   *  @warning not for MicroSoft compiler! 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT&>
  {
    /// true type (never used)
    typedef const OBJECT&                         TYPE       ;    
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
    /// actual "stored" type 
    typedef typename Traits::Type                 Type       ;
    /// serializer 
    typedef typename Traits::Serializer           Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef typename Traits::Less                 Less       ;
    /// the unique class identification
    static  int id() { return  Traits::id() ;} ;
  };
  
  /** @struct ObjectTypeTraits<SmartRef<OBJECT>>
   *  partial specialisation for smart references
   *  @see ObjectTypeTraits
   *  @warning not for MicroSoft compiler! 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<SmartRef<OBJECT> >
  {
    /// true type (never used)
    typedef SmartRef<OBJECT>                      TYPE       ;
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
    /// actual "stored" type 
    typedef SmartRef<OBJECT>                      Type       ;
#ifndef WIN32
    /// serializer
    typedef TypeSerializer<Type>                  Serializer ;
#else 
    /// serializer
    typedef RefSerializer<Type>                   Serializer ;
#endif 
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<const OBJECT*>              Less       ;
    /// the unique class identification
    static int id() { return  Traits::id() ;}                ;
  };

#endif
  
  /** @struct ObjectTypeTraits<bool>
   *  full specialisation for type 'bool'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<bool>
  {
    /// true type (never used)
    typedef        bool                           TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<bool>                Traits     ;
    /// actual "stored" type 
    typedef        TYPE                           Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static int id() 
    { 
      static int s_id = clid( "bool" );
      return     s_id ;
    } ;
  };

  /** @struct ObjectTypeTraits<char>
   *  full specialisation for type 'char'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<char>
  {
    /// true type (never used)
    typedef        char                           TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<char>                Traits     ;
    /// actual "stored" type 
    typedef        TYPE                           Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static int id() 
    { 
      static int s_id = clid( "char" );
      return     s_id ;
    } ;    
  };

  /** @struct ObjectTypeTraits<unsigned char>
   *  full specialisation for type 'unsigned char'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<unsigned char>
  {
    /// true type (never used)
    typedef unsigned char                         TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<unsigned char>       Traits     ;
    /// actual "stored" type 
    typedef          TYPE                         Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static  int id()
    { 
      static int s_id = clid( "unsigned char" );
      return     s_id ;
    } ;
  };
  
  /** @struct ObjectTypeTraits<short>
   *  full specialisation for type 'short'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<short>
  {
    /// true type (never used)
    typedef        short                          TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<short>               Traits     ;
    /// actual "stored" type 
    typedef        TYPE                           Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static int id()
    { 
      static int s_id = clid( "short" );
      return     s_id ;
    } ;
  };
  
  /** @struct ObjectTypeTraits<unsigned short>
   *  full specialisation for type 'unsigned short'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<unsigned short>
  {
    /// true type (never used)
    typedef unsigned short                        TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<unsigned short>      Traits     ;
    /// actual "stored" type 
    typedef          TYPE                         Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static  int id()
    { 
      static int s_id = clid( "unsigned short" );
      return     s_id ;
    } ;
  };

  /** @struct ObjectTypeTraits<int>
   *  full specialisation for type 'int'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<int>
  {
    /// true type (never used)
    typedef int                                   TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<int>                 Traits     ;
    /// actual "stored" type 
    typedef          TYPE                         Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static  int id()
    { 
      static int s_id = clid( "int" );
      return     s_id ;
    } ;
  };

  /** @struct ObjectTypeTraits<unsigned int>
   *  full specialisation for type 'unsigned int'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<unsigned int>
  {
    /// true type (never used)
    typedef unsigned int                          TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<unsigned int>        Traits     ;
    /// actual "stored" type 
    typedef          TYPE                         Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static  int id()
    { 
      static int s_id = clid( "unsigned int" );
      return     s_id ;
    } ;
  };

  /** @struct ObjectTypeTraits<long>
   *  full specialisation for type 'long'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<long>
  {
    /// true type (never used)
    typedef long                                  TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<long>                Traits     ;
    /// actual "stored" type 
    typedef        TYPE                           Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static  int id() 
    { 
      static int s_id = clid( "long" );
      return     s_id ;
    } ;
  };

  /** @struct ObjectTypeTraits<unsigned long>
   *  full specialisation for type 'unsigned long'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<unsigned long>
  {
    /// true type (never used)
    typedef unsigned long                         TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<unsigned long>       Traits     ;
    /// actual "stored" type 
    typedef          TYPE                         Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static  int id() 
    { 
      static int s_id = clid( "unsigned long" );
      return     s_id ;
    } ;
  };

  /** @struct ObjectTypeTraits<float>
   *  full specialisation for type 'float'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<float>
  {
    /// true type (never used)
    typedef        float                          TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<float>               Traits     ;
    /// actual "stored" type 
    typedef        TYPE                           Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static  int id() 
    { 
      static int s_id = clid( "float" );
      return     s_id ;
    } ;
  };

  /** @struct ObjectTypeTraits<double>
   *  full specialisation for type 'double'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<double>
  {
    /// true type (never used)
    typedef          double                       TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<double>              Traits     ;
    /// actual "stored" type 
    typedef          TYPE                         Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;    
    /// the unique class identification
    static  int id() 
    { 
      static int s_id = clid( "double" );
      return     s_id ;
    } ;

  };
  
  /** @struct ObjectTypeTraits<long double>
   *  full specialisation for type 'long double'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<long double>
  {
    /// true type (never used)
    typedef long double                           TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<long double>         Traits     ;
    /// actual "stored" type 
    typedef          TYPE                         Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    typedef std::less<Type>                       Less       ;
    /// the unique class identification
    static  int id() 
    { 
      static int s_id = clid( "long double" );
      return     s_id ;
    } ;
  };
  
  /** @struct ObjectTypeTraits<std::string>
   *  full specialisation for type 'std::string'
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <>
  struct ObjectTypeTraits<std::string>
  {
    /// true type (never used)
    typedef          std::string                  TYPE       ;
    /// "traits'-provider == own type 
    typedef ObjectTypeTraits<std::string>         Traits     ;
    /// actual "stored" type 
    typedef          TYPE                         Type       ;
    /// serlializer 
    typedef TypeSerializer<Type>                  Serializer ;
    /// "apply"
    typedef TypeApply<Type>                       Apply      ;
    /// comparison
    struct Less : public std::binary_function<Type,Type,bool>
    {
      inline bool operator() 
        ( const Type& s1 , const Type& s2 ) const
      {
        return 
          std::lexicographical_compare( s1.begin() , s1.end() , 
                                        s2.begin() , s2.end() );
      };
    };    
    /// the unique class identification
    static  int id() 
    { 
      static int s_id = clid( "std::string" );
      return     s_id ;
    } ;
  };

}; // end of namespace Relations

// ============================================================================
//  The End
// ============================================================================
#endif // RELATIONS_ObjectTypeTraits_H
// ============================================================================
