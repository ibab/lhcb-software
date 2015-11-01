// $Id: LoKi_GenDecay.cpp,v 1.7 2009-06-11 16:46:05 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/GenDecays.h"
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/PrintHepMCDecay.h"
// ============================================================================
/** @file
 *  Simple test file to test various C++ decay descriptors 
 *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-0702
 */
// ============================================================================
LOKI_MCALGORITHM( GenDecayDescriptor ) 
{
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  using namespace Decays::Nodes  ;
  using namespace Decays::Trees  ;

  typedef Decays::Trees::Marked_<const HepMC::GenParticle*> Marked ;
  typedef Decays::iTree_<const HepMC::GenParticle*>         iTree  ;
  
  // get all beauty mesons 
  GRange b = gselect ( "B" , GABSID == "B_s0" ) ;
  
  MsgStream& log = info() ;

  Decays::Node bquark = HasQuark ( LHCb::ParticleID::bottom )  ;
  GenExclusive p1 ( bquark ) ;
  
  GenExclusive p2 ( Pid("B_s0") ) ;
  
  Decays::Node any = Any() ;
  GenExclusive p3 = GenExclusive( any ) ;
  
  GenExclusive p4 ( Pid("B_s0") ) ;
  p4 += "J/psi(1S)" ;
  p4 += "phi(1020)" ;
  
  GenExclusive p5 ( Pid("B_s~0") ) ;
  p5 += "J/psi(1S)" ;
  p5 += "phi(1020)" ;
  
  GenExclusive p6 ( CC ( "B_s0" ) ) ;
  p6 += "J/psi(1S)" ;
  p6 += "phi(1020)" ;
  
  GenInclusive p7 ( CC ( "B_s0" ) ) ;
  p7 += "J/psi(1S)" ;
  
  GenInclusive p8 
    ( CC ( "B_s0" ) , Decays::Trees::Sections ) ;
  p8 += EllPlus  () ;
  
  
  GenInclusive p9 
    ( CC ( "B_s0" ) , Decays::Trees::Sections ) ;
  p9 += EllPlus  () ;
  p9 += EllMinus () ;
  
  GenInclusive p10
    ( CC ( "B_s0" ) , Decays::Trees::Sections ) ;
  p10 += EllPlus  () ;
  p10 += EllMinus () ;
  p10 += "K-" ;
  
  GenInclusive p11
    ( CC ( "B_s0" ) , Decays::Trees::Sections ) ;
  p11 += EllPlus  () ;
  p11 += EllMinus () ;
  p11 += "K-" ;
  p11 += "K+" ;
  
  GenInclusive p12 ( p11 ) ;
  p12 += "gamma" ;
  
  GenPhotos p13 ( p11 ) ;
  
  GenOptional p14 
    ( CC ( "B_s0" ) , GenExclusive::TreeList() ) ;
  p14.addOptional  ("phi(1020)" ) ;
  
  p14 += "J/psi(1S)" ;
  
  
  GenPhotosOptional p15 ( p14 ) ;


  GenExclusive p16 ( p4 ) ;
  p16.setOscillation ( Decays::Trees::Oscillated ) ;
  
  GenExclusive p17 ( Pid ( "B_s~0" ) ) ;
  p17 += "J/psi(1S)" ;
  p17 += "phi(1020)" ;
  p17.setOscillation ( Decays::Trees::NotOscillated ) ;
  
  GenExclusive p18 ( Pid ( "B_s0" ) ) ;
  p18 += "J/psi(1S)" ;
  p18 += "phi(1020)" ;
  p18.setOscillation ( Decays::Trees::Oscillated ) ;
  
  Decays::Tree_<const HepMC::GenParticle*> p19 = p17 || p18 ;
  
  
  GenExclusive p20 ( Pid ( "B_s0" )  || Pid("B_s~0") ) ;
  p20 += "J/psi(1S)" ;
  p20 += "phi(1020)" ;
  p20.setOscillation ( Decays::Trees::Oscillated ) ;

  
  GenExclusive p21 ( Pid( "B_s0" ) ) ;
  p21 += "J/psi(1S)" ;
  p21 += Marked ( GenExclusive ( Pid("phi(1020)") ) ) ;
  
  GenExclusive p22 ( Pid("B_s0") ) ;
  p22 += Marked ( GenExclusive ( Pid("J/psi(1S)") ) ) ;
  p22 += Marked ( GenExclusive ( Pid("phi(1020)") ) ) ;
  
  Assert ( p1  . validate ( ppSvc () ) . isSuccess () , "p1  is invalid!" ) ;
  Assert ( p2  . validate ( ppSvc () ) . isSuccess () , "p2  is invalid!" ) ;
  Assert ( p3  . validate ( ppSvc () ) . isSuccess () , "p3  is invalid!" ) ;
  Assert ( p4  . validate ( ppSvc () ) . isSuccess () , "p4  is invalid!" ) ;
  Assert ( p5  . validate ( ppSvc () ) . isSuccess () , "p5  is invalid!" ) ;
  Assert ( p6  . validate ( ppSvc () ) . isSuccess () , "p6  is invalid!" ) ;
  Assert ( p7  . validate ( ppSvc () ) . isSuccess () , "p7  is invalid!" ) ;
  Assert ( p8  . validate ( ppSvc () ) . isSuccess () , "p8  is invalid!" ) ;
  Assert ( p9  . validate ( ppSvc () ) . isSuccess () , "p9  is invalid!" ) ;
  Assert ( p10 . validate ( ppSvc () ) . isSuccess () , "p10 is invalid!" ) ;
  Assert ( p11 . validate ( ppSvc () ) . isSuccess () , "p11 is invalid!" ) ;
  Assert ( p12 . validate ( ppSvc () ) . isSuccess () , "p12 is invalid!" ) ;
  Assert ( p13 . validate ( ppSvc () ) . isSuccess () , "p13 is invalid!" ) ;
  Assert ( p14 . validate ( ppSvc () ) . isSuccess () , "p14 is invalid!" ) ;
  Assert ( p15 . validate ( ppSvc () ) . isSuccess () , "p15 is invalid!" ) ;
  Assert ( p16 . validate ( ppSvc () ) . isSuccess () , "p16 is invalid!" ) ;
  Assert ( p17 . validate ( ppSvc () ) . isSuccess () , "p17 is invalid!" ) ;
  Assert ( p18 . validate ( ppSvc () ) . isSuccess () , "p18 is invalid!" ) ;
  Assert ( p19 . validate ( ppSvc () ) . isSuccess () , "p19 is invalid!" ) ;
  Assert ( p20 . validate ( ppSvc () ) . isSuccess () , "p20 is invalid!" ) ;
  Assert ( p21 . validate ( ppSvc () ) . isSuccess () , "p21 is invalid!" ) ;
  Assert ( p22 . validate ( ppSvc () ) . isSuccess () , "p21 is invalid!" ) ;

  // loop over beauty mesons 
  for ( GRange::iterator ib = b.begin() ; b.end() != ib ; ++ib )
  {
    const HepMC::GenParticle* p = *ib ;
    
    log << LoKi::Print::printDecay ( p ) ;
    
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
        << "p10 : " << ( p10 ( p ) ? "True  " : "False " ) << "  " << p10 ;
    log << std::endl 
        << "p11 : " << ( p11 ( p ) ? "True  " : "False " ) << "  " << p11 ;
    log << std::endl 
        << "p12 : " << ( p12 ( p ) ? "True  " : "False " ) << "  " << p12 ;
    log << std::endl 
        << "p13 : " << ( p13 ( p ) ? "True  " : "False " ) << "  " << p13 ;
    log << std::endl 
        << "p14 : " << ( p14 ( p ) ? "True  " : "False " ) << "  " << p14 ;
    log << std::endl 
        << "p15 : " << ( p15 ( p ) ? "True  " : "False " ) << "  " << p15 ;
    log << std::endl 
        << "p16 : " << ( p16 ( p ) ? "True  " : "False " ) << "  " << p16 ;
    log << std::endl 
        << "p17 : " << ( p17 ( p ) ? "True  " : "False " ) << "  " << p17 ;
    log << std::endl 
        << "p18 : " << ( p18 ( p ) ? "True  " : "False " ) << "  " << p18 ;
    log << std::endl 
        << "p19 : " << ( p19 ( p ) ? "True  " : "False " ) << "  " << p19 ;
    log << std::endl 
        << "p20 : " << ( p20 ( p ) ? "True  " : "False " ) << "  " << p20 ;
    log << std::endl 
        << "p21 : " << ( p21 ( p ) ? "True  " : "False " ) << "  " << p21 ;
  
    p21.reset() ;
    if ( p21 (  p ) ) 
    {
      iTree::Collection marked ;
      log << p21.collect ( marked ) << std::endl ;
      for ( iTree::Collection::const_iterator im = marked.begin() ;
            marked.end() != im ; ++im ) 
      { log << std::endl << " marked: " << LoKi::Print::printDecay ( *im ) ; }
      p21.reset() ;
    }
    
    log << std::endl 
        << "p22 : " << ( p22 ( p ) ? "True  " : "False " ) << "  " << p22 ;
  
    p22.reset() ;
    if ( p22 (  p ) ) 
    {
      iTree::Collection marked ;
      log << std::endl << " # of marked : " << p22.collect ( marked ) ;
      for ( iTree::Collection::const_iterator im = marked.begin() ;
            marked.end() != im ; ++im ) 
      { log << std::endl << " marked: " << LoKi::Print::printDecay ( *im ) ; }
      p22.reset() ;
    }
    
    log << endmsg ;
  }
  
 
  return StatusCode::SUCCESS ;
}
// =============================================================================
// The END 
// =============================================================================
