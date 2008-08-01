// $Id: TypeWrapper.h,v 1.2 2008-08-01 10:36:27 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_TYPEWRAPPER_H 
#define LHCBMATH_TYPEWRAPPER_H 1
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// Boost 
// ============================================================================
#include "boost/call_traits.hpp"
// ============================================================================
namespace Gaudi
{
  namespace Math
  {
    namespace detail 
    {
      // ======================================================================
      /** @struct Null 
       *  The trivial helper structure to define "null enum"
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-07-31
       */
      struct Null { enum { value = 0 } ; } ;
      // ======================================================================
      /** @struct Null_ 
       *  The trivial helper structure to define "null function"
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-07-31
       */
      template <class TYPE,class TYPE2 = bool>
      struct Null_ : public std::unary_function<TYPE,TYPE2>
      {
        // ====================================================================
        /// the actual type of argument (ignored) 
        typedef typename boost::call_traits<TYPE>::param_type  argument ;
        // ====================================================================
        /// the only one essential method 
        inline TYPE2 operator() ( argument /* a */ ) const { return 0 ; } 
        // ====================================================================
      } ;
      // ======================================================================
    }
    // ========================================================================
    /** @struct TypeWrapper 
     *  Simple helepr routine to wrap the type 
     *  @author Juan PALACIOS juan.palacios@cern.ch
     *  @date 2006-10-26 
     */    
    template <typename T>
    struct TypeWrapper 
    {
      typedef T value_type;
    } ;
    // ========================================================================
  } // end of namespace Gaudi::Math 
  // ==========================================================================
} // end of namespace Gaudi 
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_TYPEWRAPPER_H
// ============================================================================
