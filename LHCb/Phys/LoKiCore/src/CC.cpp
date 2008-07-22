// $Id: CC.cpp,v 1.1 2008-07-22 13:00:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/CC.h"
#include "LoKi/Services.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file 
 *  Implementation file for functions from namespace LoKi::CC
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2008-07-22 
 */
// ============================================================================
/*  simple function to make charge all conjugates inside of 
 *  the original string
 *  all substrings, which are known for IParticlePropertySvc 
 *  are subsutututed by their charge conjugates 
 *  @param decay the original sring 
 *  @return charge-conjugated string 
 */ 
// ============================================================================
std::string LoKi::CC::cc ( const std::string& decay ) 
{
  // get LoKi service 
  const LoKi::Services& services = LoKi::Services::instance() ;
  LoKi::ILoKiSvc* loki = services.lokiSvc() ;
  // 
  LoKi::Report::Assert 
    ( 0 != loki , 
      "LoKi::CC:cc('" + decay + "'): LoKi Service is unavailable") ;
  // use the service 
  return loki->cc( decay ) ;
}
// ============================================================================
// The END 
// ============================================================================
