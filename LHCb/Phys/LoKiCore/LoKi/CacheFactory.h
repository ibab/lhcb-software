// $Id:$ 
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
    /** @class CacheFactory
     *  Helper class to create factory of functors 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-01-17
     */
    template <class FUNCTOR,std::size_t N>
    struct Factory
    {
      /// the only useful method 
      FUNCTOR* create () ;
    };
    // ========================================================================
    /** helper function to create the code for CacheFactory
     *  @param  type    the functor type 
     *  @param  cppcode the actual C++ code
     *  @param  pycode  the python code (full)
     *  @param  pytype  the python type (from object)
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
  } //                                             end of namespace LoKi::Cache
  // ==========================================================================
} //                                                       end of namepace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CACHEFACTORY_H
// ============================================================================
