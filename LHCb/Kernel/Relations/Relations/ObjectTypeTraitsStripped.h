// $Id: ObjectTypeTraitsStripped.h,v 1.1.1.1 2004-07-21 07:57:26 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/05/10 12:29:42  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
// 
// ============================================================================
#ifndef RELATIONS_OBJECTTYPETRAITSSTRIPPED_H 
#define RELATIONS_OBJECTTYPETRAITSSTRIPPED_H 1
// Include files
#include "Relations/ObjectTypeTraits.h"

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
  
#ifndef WIN32
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
  struct ObjectTypeTraits<OBJECT*>           : public ObjectTypeTraits<OBJECT>
  {
    /// true type (never used)
    typedef OBJECT*                               TYPE       ;
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
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
  struct ObjectTypeTraits<const OBJECT*>     : public ObjectTypeTraits<OBJECT>
  {
    /// true type (never used)
    typedef const OBJECT*                         TYPE       ;    
    /// "traits'-provider 
    typedef ObjectTypeTraits<OBJECT>              Traits     ;
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
#endif 
  
}; // end of namespace Relations 



// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_OBJECTTYPETRAITSSTRIPPED_H
// ============================================================================
