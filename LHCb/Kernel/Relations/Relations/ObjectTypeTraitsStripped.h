// $Id: ObjectTypeTraitsStripped.h,v 1.4 2006-06-02 16:18:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_OBJECTTYPETRAITSSTRIPPED_H 
#define RELATIONS_OBJECTTYPETRAITSSTRIPPED_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Relations/ObjectTypeTraits.h"
#include "Relations/Pointer.h"
// ============================================================================

/** @file
 *  Partial specialization of general ObjectTypeTraits structure  
 *  It is not valid for MicroSoft compiler 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   09/05/2002
 */

namespace Relations
{
  /// forward declaration of general template structure 
  template <class Object> struct ObjectTypeTraits ;
  
  /** @struct ObjectTypeTraits<const OBJECT>
   *  partial specialization for "const" types
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT>      : public ObjectTypeTraits<OBJECT>
  {
    /// true type (never used)
    typedef const OBJECT                          TYPE       ;
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
  };

  /** @struct ObjectTypeTraits<OBJECT*>
   *  partial specialisation for pointers
   *  @see ObjectTypeTraits
   *  @warning not for MicroSoft compiler! 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<OBJECT*> : public ObjectTypeTraits<OBJECT>
  {
    /// true type (never used)
    typedef OBJECT*                                 TYPE       ;
    typedef Pointer<OBJECT>                        _TYPE       ;
    /// 'type'-provider, here it is own type
    typedef ObjectTypeTraits<TYPE>                  Traits     ;
    /// actual "stored" type
    typedef typename TypePersistent<_TYPE>::Result  Type       ;
    /// comparison (strict ordering criteria)
    typedef std::less<Type>                         Less        ;
    /// the unique class identification
    static  const CLID& id()   { return TypeId<_TYPE>::id() ; } ;
   /// true type (never used)
  };

  /** @struct ObjectTypeTraits<OBJECT&>
   *  partial specialisation for references
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<OBJECT&>           : public ObjectTypeTraits<OBJECT>
  {
    /// true type (never used)
    typedef OBJECT&                               TYPE       ;    
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
  };

  /** @struct ObjectTypeTraits<const OBJECT*>
   *  partial specialisation for pointers to const
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT*>     : public ObjectTypeTraits<OBJECT*>
  {
    /// true type (never used)
    typedef const OBJECT*                         TYPE       ;    
    /// "traits'-provider 
    typedef ObjectTypeTraits<const OBJECT*>       Traits     ;
  };

  /** @struct ObjectTypeTraits<const OBJECT&>
   *  partial specialisation for const references
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<const OBJECT&>     : public ObjectTypeTraits<OBJECT>
  {
    /// true type (never used)
    typedef const OBJECT&                         TYPE       ;    
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
  };

  /** @struct ObjectTypeTraits<SmartRef<OBJECT>>
   *  partial specialisation for smart references
   *  @see ObjectTypeTraits
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */   
  template <class OBJECT>
  struct ObjectTypeTraits<SmartRef<OBJECT> > : public ObjectTypeTraits<OBJECT>
  {
    /// true type (never used)
    typedef SmartRef<OBJECT>                      TYPE       ;
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
  };
  
}; // end of namespace Relations 



// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_OBJECTTYPETRAITSSTRIPPED_H
// ============================================================================
