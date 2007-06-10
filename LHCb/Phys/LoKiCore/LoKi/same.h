// $Id: same.h,v 1.1 2007-06-10 19:54:06 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_SAME_H 
#define LOKI_SAME_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Reference.h"
// ============================================================================
namespace LoKi 
{
  template <class TYPE>
  bool same ( const TYPE* o1 , const TYPE* o2           ) { return o1 == o2 ; }
  template <class TYPE>
  bool same ( const TYPE* o1 , const SmartRef<TYPE>& o2 ) { return o1 == o2 ; }
  template <class TYPE>
  bool same ( const SmartRef<TYPE>& o1 , const TYPE* o2 ) { return o1 == o2 ; }
  template <class TYPE>
  bool same ( const LoKi::Reference<TYPE>& o1 , 
              const                 TYPE&  o2 ) { return o1 == o2 ; }
  template <class TYPE>
  bool same ( const                 TYPE&  o1 ,
              const LoKi::Reference<TYPE>& o2 ) { return o2 == o1 ; }
  template <class TYPE>
  bool same ( const LoKi::Reference<TYPE>& o1 , 
              const LoKi::Reference<TYPE>& o2 ) { return o1 == o2 ; }
  template <class TYPE>
  bool same ( const TYPE& o1 , 
              const TYPE& o2 ) { return same ( &o1 , &o2 ) ; }
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_SAME_H
// ============================================================================
