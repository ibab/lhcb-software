// $Id: ObjectTypeTraits.h,v 1.10 2002-05-10 12:29:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_ObjectTypeTraits_H
#define RELATIONS_ObjectTypeTraits_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL 
#include "GaudiKernel/SmartRefBase.h"
#include <functional>
#include <algorithm>
// Relations 
#include "Relations/TypeConversion.h"
#include "Relations/TypeSerializer.h"
#include "Relations/TypePersistent.h"
#include "Relations/TypeApply.h"
#include "Relations/TypeId.h"
#include "Relations/Less.h"
#include "Relations/ObjectTypeTraitsStripped.h"
// forward declarations
template <class TYPE> class SmartRef ; // from GaudiKernel package

namespace Relations
{  
  /// forward declaration
  template <class OBJECT> struct TypePersistent ;  
  /// forward declaration
  template <class OBJECT> struct TypeSerializer ;  
  /// forward declaration
  template <class OBJECT> struct TypeApply      ;
  /// forward declaration
  template <class OBJECT> struct TypeId         ;
  
  /** @struct ObjectTypeTraits
   *
   *  A helper structure to define the Object Type Traits
   * 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/02/2002
   */
  template <class OBJECT>
  struct ObjectTypeTraits
  {
    /// true type (never used)
    typedef OBJECT                                  TYPE       ;
    /// 'type'-provider, here it is own type  
    typedef ObjectTypeTraits<OBJECT>                Traits     ;
    /// actual "stored" type 
    typedef typename TypePersistent<TYPE>::Result   Type       ;
    /// serializer 
    typedef TypeSerializer<TYPE>                    Serializer ;
    /// "apply"
    typedef TypeApply<TYPE>                         Apply      ;
    /// comparison (strict ordering criteria)
    typedef std::less<Type>                         Less       ;
    /// the unique class identification
    static  const CLID& id()   { return TypeId<TYPE>::id() ; } ;
  };
  
}; // end of namespace Relations

// ============================================================================
//  The End
// ============================================================================
#endif // RELATIONS_ObjectTypeTraits_H
// ============================================================================
