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
#ifdef __INTEL_COMPILER        // Disable ICC warnings and remarks
  #pragma warning(disable:279) // BOOST_ASSERT controlling expression is constant
  #pragma warning(push)
#endif
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
#ifdef __INTEL_COMPILER
  #pragma warning(pop)        // Reenable ICC warning 279
#endif
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_TYPEWRAPPER_H
// ============================================================================
