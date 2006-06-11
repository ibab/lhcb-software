// $Id: ObjectTypeTraits.h,v 1.4 2006-06-11 17:46:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_ObjectTypeTraits_H
#define RELATIONS_ObjectTypeTraits_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// Boost 
// ============================================================================
#include "boost/type_traits/is_convertible.hpp"
#include "boost/call_traits.hpp"
#include "boost/static_assert.hpp"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/Pointer.h"
#include "Relations/Reference.h"
#include "Relations/IsConvertible.h"
// ============================================================================

namespace Relations
{  
  /// forward declaration
  template <class OBJECT>      class  Pointer   ;
  template <class OBJECT>      class  Reference ;
  template <class OBJECT>      struct Types     ;
  template <class T1,class T2> struct InConvertible ;
  
  namespace detail 
  {
    template <class T1,class T2> struct Conversion ;
    
    template <bool,class TYPE>
    struct _Types
    {
      typedef typename boost::call_traits<TYPE>::param_type Input ;
      typedef const TYPE&       Output ;
      typedef       TYPE        Inner  ;
    } ;
    template <bool value,class TYPE>
    struct _Types<value,TYPE*>
    {
      typedef const TYPE* const Input  ;
      typedef       TYPE*       Output ;
      typedef Pointer<TYPE>     Inner  ;
    } ;
    template <bool value ,class TYPE>
    struct _Types<value,const TYPE*>
    {
      typedef const TYPE* const Input  ;
      typedef       TYPE*       Output ;
      typedef Pointer<TYPE>     Inner  ; 
    } ;
    template <bool value,class TYPE>
    struct _Types<value,TYPE&>
    {
      typedef const TYPE&       Input  ;
      typedef       TYPE&       Output ;
      typedef Reference<TYPE>   Inner  ;
    } ;
    template <bool value,class TYPE>
    struct _Types<value,const TYPE&>
    {
      typedef const TYPE&       Input  ;
      typedef const TYPE&       Output ;
      typedef Reference<TYPE>   Inner  ;
    } ;
    template <class TYPE>
    struct _Types<true,TYPE> 
    {
      typedef const TYPE* const Input   ;
      typedef       TYPE*       Output  ;
      typedef SmartRef<TYPE>    Inner   ;  
    } ;
    template <class TYPE>
    struct _Types<true,TYPE*> 
    {
      BOOST_STATIC_ASSERT( sizeof(TYPE) == 0 )  ;
    } ;
    template <class TYPE>
    struct _Types<true,TYPE&> 
    {
      BOOST_STATIC_ASSERT( sizeof(TYPE) == 0 )  ;
    } ;
  } ;
  
  template <class TYPE>
  struct Types 
  {
    enum {
      value = 
      Relations::IsConvertible<const TYPE*,const DataObject*>      :: value ||
      Relations::IsConvertible<const TYPE*,const ContainedObject*> :: value  
    } ;
    typedef typename detail::_Types<value,TYPE>::Input  Input  ;
    typedef typename detail::_Types<value,TYPE>::Output Output ;
    typedef typename detail::_Types<value,TYPE>::Inner  Inner  ;
  } ;

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
    /// true type ( almost never used)
    typedef OBJECT                                  Type   ;
    /// 'type'-provider, here it is own type  
    typedef ObjectTypeTraits<OBJECT>                Traits ;
    /// 'in'-type 
    typedef typename Types<Type>::Input             Input  ;
    /// 'out'-type
    typedef typename Types<Type>::Output            Output ;
    /// 'storable' type 
    typedef typename Types<Type>::Inner             Inner  ;
    /// comparison (strict ordering criteria)
    typedef std::less<Inner>                        Less   ;
  } ;
  
}; // end of namespace Relations

// ============================================================================
//  The End
// ============================================================================
#endif // RELATIONS_ObjectTypeTraits_H
// ============================================================================
