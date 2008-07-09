// $Id: LoKi_Decay.cpp,v 1.1 2008-07-09 17:00:48 ibelyaev Exp $
// =============================================================================
// Include files 
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Decays.h"
// =============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/PrintDecay.h"
// =============================================================================
// Boost 
// =============================================================================
#include "boost/integer_traits.hpp"
// =============================================================================
/** @file
 *  Simple test file to test various C++ decay descriptors 
 *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-0702
 */
// =============================================================================
LOKI_ALGORITHM( DecayDescriptor ) 
{
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  // get all beauty mesons 
  Range b = select ( "B" , "B_s0" == ABSID ) ;
  
  MsgStream& log = always() ;
  
  log << "Number of input particles: " << b.size() << endreq ;
  
  LoKi::Decays::Trees::Exclusive p1 
    ( LoKi::Decays::Nodes::HasQuark ( LHCb::ParticleID::bottom ) ) ;
  
  LoKi::Decays::Trees::Exclusive p2 
    ( LoKi::Decays::Nodes::CC ( "B_s0" ) ) ;
  p2 += "J/psi(1S)" ;
  p2 += "phi(1020)" ;
  

  LoKi::Decays::Trees::Inclusive p3 
    ( LoKi::Decays::Nodes::CC ( "B_s0" ) ) ;
  p3 += "J/psi(1S)" ;
  
  LoKi::Decays::Nodes::Meson meson ;
  LoKi::Decays::Trees::Optional p4 ( meson ) ;
  p4 += "J/psi(1S)" ;
  p4.addOptional ("phi(1020)") ;
  
  Assert ( p1   . validate ( ppSvc () ) . isSuccess () , "p1  is invalid!" ) ;
  Assert ( p2   . validate ( ppSvc () ) . isSuccess () , "p2  is invalid!" ) ;
  Assert ( p3   . validate ( ppSvc () ) . isSuccess () , "p3  is invalid!" ) ;
  Assert ( p4   . validate ( ppSvc () ) . isSuccess () , "p4  is invalid!" ) ;
  
  
  // loop over beauty mesons 
  for ( Range::iterator ib = b.begin() ; b.end() != ib ; ++ib )
  {
    const LHCb::Particle* p = *ib ;
    
    log << LoKi::Print::printDecay ( p ) ;
    
    log << std::endl 
        << "p1  : " << ( p1  ( p ) ? "True  " : "False " ) << "  " << p1  ;
    log << std::endl 
        << "p2  : " << ( p2  ( p ) ? "True  " : "False " ) << "  " << p2  ;
    log << std::endl 
        << "p3  : " << ( p3  ( p ) ? "True  " : "False " ) << "  " << p3  ;
    log << std::endl 
        << "p4  : " << ( p4  ( p ) ? "True  " : "False " ) << "  " << p4  ;

    log << endreq ;
    
  }

  return StatusCode::SUCCESS ;
}
// =============================================================================
// The END 
// =============================================================================
