// $Id: LoKi_MCDecays.cpp,v 1.1 2004-06-29 06:44:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/EventHeader.h"
#include "Event/GenHeader.h"
#include "Event/Collision.h"
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "LoKi/MCParticles.h"
// ============================================================================


/** @file
 *  Simple ilustration of usage of new features of 
 *  classes LoKi::MCMatch, LoKi::MCMatchObj
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-06-28
 */


LOKI_ALGORITHM( LoKi_MCDecays ) 
{
  using namespace LoKi ;
  using namespace LoKi::Cuts ;
  
  // create the matching  object 
  MCMatch mc = mctruth();
  
  MCRange trees = mc ->findDecays (        "[D0 ->  K-  pi+]cc" ) ;
  MCRange all   = mc ->findDecays (        "[D0 -> ^K- ^pi+]cc" ) ;
  MCRange p1    = mc ->findDecays (        "[D0 -> ^K-  pi+]cc" ) ;
  MCRange p2    = mc ->findDecays (        "[D0 ->  K- ^pi+]cc" ) ;
  MCRange k1    = mc ->findDecays ( " K-  : [D0 ->  K-  pi+]cc" ) ;
  MCRange k2    = mc ->findDecays ( " K+  : [D0 ->  K-  pi+]cc" ) ;
  MCRange pi1   = mc ->findDecays ( " pi- : [D0 ->  K-  pi+]cc" ) ;
  MCRange pi2   = mc ->findDecays ( " pi+ : [D0 ->  K-  pi+]cc" ) ;
  
  always() 
    << " MCDecays found : " 
    << " trees " << trees.size() << ", " 
    << " all "   << all  .size() << ", " 
    << " p1 "    << p1   .size() << ", " 
    << " p2 "    << p2   .size() << ", " 
    << " k1 "    << k1   .size() << ", " 
    << " k2 "    << k2   .size() << ", " 
    << " pi1 "   << pi1  .size() << ", " 
    << " pi2 "   << pi2  .size() << ", " 
    << endreq ;
    
  return StatusCode::SUCCESS ;
  
};

// ============================================================================
// The END 
// ============================================================================
