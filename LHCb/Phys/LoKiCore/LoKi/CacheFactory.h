// $Id$ 
// ============================================================================
#ifndef LOKI_CACHEFACTORY_H 
#define LOKI_CACHEFACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <ostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cache 
  {
    // ========================================================================
    /** helper function to create the code for CacheFactory
     *  @param  type    the functor type 
     *  @param  cppcode the actual C++ code
     *  @param  pycode  the python code (full)
     *  @param  pytype  the python type (from object)
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-01-17
     */
    GAUDI_API
    std::string 
    makeCode
    ( const std::string&  type    , 
      const std::string&  cppcode , 
      const std::string&  pycode  ,
      const std::string&  pytype  ) ;
    // ========================================================================
    /** helper function to create the code for CacheFactory
     *  @param  stream  the output stream 
     *  @param  type    the functor type 
     *  @param  cppcode the actual C++ code
     *  @param  pycode  the python code (full)
     *  @param  pytype  the python type (from object)
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-01-17
     */
    GAUDI_API
    std::ostream& 
    makeCode
    ( std::ostream&       stream  ,
      const std::string&  type    , 
      const std::string&  cppcode , 
      const std::string&  pycode  ,
      const std::string&  pytype  ) ;
    // ========================================================================
    /** calculate the hash for the code flagment 
     *  @param code  the code 
     *  @return hash-value      
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-01-17
     */
    GAUDI_API 
    unsigned int 
    makeHash  ( const std::string& code ) ;
    // ========================================================================
  } //                                             end of namespace LoKi::Cache
  // ==========================================================================
} //                                                       end of namepace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CACHEFACTORY_H
// ============================================================================
