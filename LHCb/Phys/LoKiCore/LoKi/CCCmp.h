// $Id: CCCmp.h,v 1.1 2008-07-22 13:00:57 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_CCCMP_H 
#define LOKI_CCCMP_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <functional>
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @struct CCCmp 
   *  a bit specific comparison of strings, useful for 
   *  ordering according to the length as the primary parameter 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   */
  struct CCCmp : public std::binary_function<std::string,std::string,bool>
  {
    /** the only one essential method 
     *  The most long string is "less", otherwise the 
     *  standard comparison is applicable.
     */
    inline bool operator() 
      ( const std::string& v1 , 
        const std::string& v2 ) const 
    { 
      const std::string::size_type s1 = v1.size () ; 
      const std::string::size_type s2 = v2.size () ;
      return s1 < s2 ? false : s2 < s1 ? true  : ( v1 < v2 ) ;
    }
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CCCMP_H
// ============================================================================
