// $Id: LoKi_MCDecay.cpp,v 1.3 2008-12-06 16:31:28 ibelyaev Exp $
// =============================================================================
// Include files 
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCDecays.h"
// =============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/PrintMCDecay.h"
// =============================================================================
/** @file
 *  Simple test file to test various C++ decay descriptors 
 *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-0702
 */
// =============================================================================
LOKI_MCALGORITHM( MCDecayDescriptor ) 
{
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  
  // get all beauty mesons 
  MCRange b = mcselect ( "B" , MCABSID == "B_s0" ) ;
  
  MsgStream& log = info() ;
  
  Decays::Node bquark = Decays::Nodes::HasQuark ( LHCb::ParticleID::bottom ) ;
  Decays::Trees::MCExclusive p1 ( bquark ) ;


  Decays::Trees::MCExclusive p2 ( "B_s0" ) ;
  p2 += "J/psi(1S)" ;
  p2 += "phi(1020)" ;
  
  
  Decays::Trees::MCExclusive p3 ( Decays::Nodes::CC("B_s0" ) ) ;
  p3 += "J/psi(1S)" ;
  p3 += "phi(1020)" ;
  
  Decays::Trees::MCExclusive p4 ( Decays::Nodes::Pid("B_s0" ) || "B_s~0" ) ;
  p4 += "J/psi(1S)" ;
  p4 += "phi(1020)" ;
  
  
  Decays::Trees::MCInclusive p5 ( Decays::Nodes::Pid("B_s0" ) || "B_s~0" ) ;
  p5 += "J/psi(1S)" ;
  
  Decays::Trees::MCInclusive p6 ( p5 ) ;
  p6.setOscillation ( Decays::Trees::Oscillated ) ;

  Decays::Trees::MCInclusive p7 ( p5 ) ;
  p7.setOscillation ( Decays::Trees::NotOscillated ) ;
  
  Decays::Trees::MCInclusive p8 ( p5 ) ;
  p8.setAlg( Decays::Trees::Sections ) ;
  
  
  Decays::Trees::MCOptional p9 
    ( p5 , std::vector<std::string>(1,"phi(1020)") ) ;
  p9.setAlg( Decays::Trees::Sections ) ;
  
  
  Decays::Trees::Photos p10 ( p3 ) ;
  
  Decays::Trees::Photos p11 ( p1 ) ;
  p11.setAlg( Decays::Trees::Sections ) ;
  
  Decays::Trees::Photos _psi ( "J/psi(1S)" , Decays::Trees::MCExclusive::SubTrees() ) ;
  _psi.setAlg( Decays::Trees::Sections ) ;
  _psi += Decays::Nodes::EllPlus  () ;
  _psi += Decays::Nodes::EllMinus () ;
  
  Decays::Trees::Photos _phi ( "phi(1020)" , Decays::Trees::MCExclusive::SubTrees() ) ;
  _phi.setAlg( Decays::Trees::Sections ) ;
  _phi += Decays::Nodes::Positive () ;
  _phi += Decays::Nodes::Negative () ;
  
  p11 += _psi ;
  p11 += _phi ;
  
  Decays::Trees::PhotosOptional p12 ( p9 ) ;

  Assert ( p1   . validate ( ppSvc () ) . isSuccess () , "p1  is invalid!" ) ;
  Assert ( p2   . validate ( ppSvc () ) . isSuccess () , "p2  is invalid!" ) ;
  Assert ( p3   . validate ( ppSvc () ) . isSuccess () , "p3  is invalid!" ) ;
  Assert ( p4   . validate ( ppSvc () ) . isSuccess () , "p4  is invalid!" ) ;
  Assert ( p5   . validate ( ppSvc () ) . isSuccess () , "p5  is invalid!" ) ;
  Assert ( p6   . validate ( ppSvc () ) . isSuccess () , "p6  is invalid!" ) ;
  Assert ( p7   . validate ( ppSvc () ) . isSuccess () , "p7  is invalid!" ) ;
  Assert ( p8   . validate ( ppSvc () ) . isSuccess () , "p8  is invalid!" ) ;
  Assert ( p9   . validate ( ppSvc () ) . isSuccess () , "p9  is invalid!" ) ;
  Assert ( p10  . validate ( ppSvc () ) . isSuccess () , "p10 is invalid!" ) ;
  Assert ( p11  . validate ( ppSvc () ) . isSuccess () , "p11 is invalid!" ) ;
  Assert ( p12  . validate ( ppSvc () ) . isSuccess () , "p12 is invalid!" ) ;
  
  
  // loop over beauty mesons 
  for ( MCRange::iterator ib = b.begin() ; b.end() != ib ; ++ib )
  {
    const LHCb::MCParticle* p = *ib ;
    
    log << LoKi::Print::printDecay ( p , true ) ;
    
    log << std::endl 
        << "p1  : " << ( p1  ( p ) ? "True  " : "False " ) << "  " << p1  ;
    log << std::endl 
        << "p2  : " << ( p2  ( p ) ? "True  " : "False " ) << "  " << p2  ;
    log << std::endl 
        << "p3  : " << ( p3  ( p ) ? "True  " : "False " ) << "  " << p3  ;
    log << std::endl 
        << "p4  : " << ( p4  ( p ) ? "True  " : "False " ) << "  " << p4  ;


    log << std::endl 
        << "p5  : " << ( p5  ( p ) ? "True  " : "False " ) << "  " << p5  ;
    log << std::endl 
        << "p6  : " << ( p6  ( p ) ? "True  " : "False " ) << "  " << p6  ;
    log << std::endl 
        << "p7  : " << ( p7  ( p ) ? "True  " : "False " ) << "  " << p7  ;
    log << std::endl 
        << "p8  : " << ( p8  ( p ) ? "True  " : "False " ) << "  " << p8  ;

    log << std::endl 
        << "p9  : " << ( p9  ( p ) ? "True  " : "False " ) << "  " << p9  ;

    log << std::endl 
        << "p10 : " << ( p10 ( p ) ? "True  " : "False " ) << "  " << p10  ;

    log << std::endl 
        << "p11 : " << ( p11 ( p ) ? "True  " : "False " ) << "  " << p11  ;

    log << std::endl 
        << "p12 : " << ( p12 ( p ) ? "True  " : "False " ) << "  " << p12  ;
    
    log << endreq ;
  }
  
 
  return StatusCode::SUCCESS ;
}
// =============================================================================
// The END 
// =============================================================================
