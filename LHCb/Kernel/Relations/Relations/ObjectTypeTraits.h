// $Id: ObjectTypeTraits.h,v 1.2 2006-06-02 16:18:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2004/07/21 07:57:25  cattanem
// first import of Kernel/Relations
//
// Revision 1.10  2002/05/10 12:29:42  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
//
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
#include "Relations/TypePersistent.h"
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
