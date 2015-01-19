// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <sstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/CacheFactory.h"
// ============================================================================
/* helper function to create the code for CacheFactory
 *  @param  type    the functor type 
 *  @param  cppcode the actual C++ code
 *  @param  pycode  the python code (full)
 *  @param  pytype  the python type (from object)
 */
// ============================================================================
namespace 
{
  const std::hash<std::string> s_hash ;
}
// ============================================================================
std::string 
LoKi::Cache::makeCode
( const std::string&  type    , 
  const std::string&  cppcode , 
  const std::string&  pycode  ,
  const std::string&  pytype  ) 
{
  std::ostringstream s ;
  makeCode ( s , type , cppcode , pycode , pytype ) ;
  return s.str() ;  
}
// ============================================================================
/* helper function to create the code for CacheFactory
 *  @param  stream  the output stream 
 *  @param  type    the functor type 
 *  @param  cppcode the actual C++ code
 *  @param  pycode  the python code (full)
 *  @param  pytype  the python type (from object)
 */
// ============================================================================
std::ostream& 
LoKi::Cache::makeCode
( std::ostream&       stream  ,
  const std::string&  type    , 
  const std::string&  cppcode , 
  const std::string&  pycode  , 
  const std::string&  pytype  ) 
{
  //
  stream << std::endl
         << "/* ORIGINAL PYTHON CODE " << std::endl 
         << pycode                     << std::endl 
         << "*/"                       << std::endl
         << "/* PYTHON OBJECT "        << std::endl 
         <<  pytype                    << std::endl 
         << "*/"                       << std::endl ;
  //
  const unsigned long n = s_hash ( pycode ) ;
  //
  return stream 
    << "namespace LoKi { namespace Cache { "              << std::endl 
    << "template<>"                                       << std::endl 
    << type                                               << std::endl 
    << "Factory<" << type    << "," 
    << std::hex   << std::showbase << n << ">::create() " << std::endl 
    << " { return "            << cppcode << " ; } "      << std::endl 
    << " }} // end of namespaces LoKi::Cache"             << std::endl ;  
  //
}


// ============================================================================
// The END 
// ============================================================================
