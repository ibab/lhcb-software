// $Id$ 
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
// Boost 
// ============================================================================
#include "boost/format.hpp"
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
  const std::hash<std::string> s_hash{} ;
}
// ============================================================================
/*  calculate the hash for the code flagment 
 *  @param code  the code 
 *  @return hash-value      
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-01-17
 *  @todo replace with sha1
 */
// ============================================================================
unsigned int 
LoKi::Cache::makeHash  ( const std::string& code ) 
{ 
  // @todo replace with sha1
  return s_hash ( code ) ; 
}
// ============================================================================
/*  helper function to create the code for CacheFactory
 *  @param  type    the functor type 
 *  @param  cppcode the actual C++ code
 *  @param  pycode  the python code (full)
 *  @param  pytype  the python type (from object)
 */
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
         << "\n/* ORIGINAL PYTHON CODE " << std::endl 
         << pycode                       << std::endl 
         << "*/"                         << std::endl
         << "\n/* PYTHON OBJECT "        << std::endl 
         <<  pytype                      << std::endl 
         << "*/"                         << std::endl ;
  //
  const unsigned int n  = LoKi::Cache::makeHash ( pycode ) ;
  const std::string  nX = ( boost::format("%#x") % n ) .str() ;
  //
  return stream 
    << "\nnamespace LoKi { namespace Details { "                  << std::endl 
    << "template<>"                                               << std::endl 
    << type            << "*"                                     << std::endl 
    << "CacheFactory<" << type << "," << nX << ">::create() "     << std::endl 
    << " { return ( "  << cppcode << " ).clone() ; } "            << std::endl 
    << "}} // end of namespaces LoKi::Details"                    << std::endl 
    << "\ntypedef " << type    << "  t_" << nX << ";"             << std::endl 
    << "DECLARE_LOKI_FUNCTOR(" << "  t_" << nX << ","<< nX << ")" << std::endl ;
}


// ============================================================================
// The END 
// ============================================================================
