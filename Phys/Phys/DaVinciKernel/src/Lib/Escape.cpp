// $Id: Escape.cpp,v 1.1 2008-12-09 07:09:52 pkoppenb Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "Kernel/Escape.h"
#include "boost/regex.hpp" 
// ============================================================================
/*  Replace all special characters from particle name
 */
// ============================================================================
namespace Decays
{
  
  std::string escape( const std::string& input ){
    
    const int size = input.size();
    if( size == 0 ) return "";
    
    // all chars must be matched, or they fall in the catchall
    const char *expr = 
      "([[:word:]])" // 1 : allowed chars
      "|^(~)"        // 2 : starting ~
      "|([~])"       // 3 : then ok
      "|(\\*)"       // 4 : star
      "|([+])"       // 5 : +
      "|([-])"       // 6 : -
      "|(.)";        // 7 : default catchall.
    
    const char* replace =
      "(?1$1)" 
      "(?2anti)"
      "(?3~)"
      "(?4st)"
      "(?5plus)"
      "(?6minus)"
      "(?7_)";
    
    std::string ret;
    boost::regex e( expr );
    ret = boost::regex_replace( input, e, replace
                                , boost::match_default | boost::format_all );
    
    // remove double __ and trailing and leading _
    e.assign( "(^_+)|(_+$)|(_{2,})" );
    ret = boost::regex_replace( ret, e, "(?1)(?2)(?3_)"
                                , boost::match_default | boost::format_all );
    
    return ret;
    
    
  }
  // ==========================================================================
} // end of namespace Decays
// ============================================================================
// The END
// ============================================================================

