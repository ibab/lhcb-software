// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "Kali.h"
// ============================================================================
/** @file 
 *  Implementation of simple functions from namespace Kali
 *
 *  @see Kali
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-07-31
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// "kalify the TES-location
// ============================================================================
std::string Kali::kalify ( const std::string& input , 
                           const std::string& kali  ) 
{
  // TES-location is already properly "kalified"
  if ( std::string::npos != input.find ( kali  ) ) { return input ; }
  //
  if ( 0 == input.find ( "/Event/" ) ) 
  { return "/Event/" + kali + input.substr( 7 ) ; }
  //
  return "/Event/" + kali + input ;
}
// ============================================================================
// The END 
// ============================================================================
