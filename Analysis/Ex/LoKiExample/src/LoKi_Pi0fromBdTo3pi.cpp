// $Id: LoKi_Pi0fromBdTo3pi.cpp,v 1.3 2004-08-17 15:43:36 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/03/03 14:17:29  ibelyaev
//  update for LoKi v2r0
//
// Revision 1.1.1.1  2003/07/24 16:43:50  ibelyaev
//  new package with LoKi examples 
//
// Revision 1.1.1.1  2003/03/12 19:56:56  ibelyaev
// the first CVS import of new package
// 
// ============================================================================
// Include files 
// LoKi
#include "LoKi/LoKi.h"

/** @file 
 * 
 *  Implementation file for class : LoKi_Pi0fromBdTo3pi 
 * 
 *  @date   2003-03-12 
 *  @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
LOKI_ALGORITHM( LoKi_Pi0fromBdTo3pi ) 
{
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  using namespace LoKi::Fits ;
  
  // select all MC pi0s
  MCMatch match( mctruth() );
  MCRange b0s  = match->findDecays( "B0 -> pi+ pi- pi0 ");
  MCMatch::MCParticleVector pi0s;
  LoKi::Extract::getMCParticles( b0s.begin  () , 
                                 b0s.end    () , 
                                 std::back_inserter( pi0s ) , 
                                 ParticleID( 111 ) == MCID  );
  if( pi0s.empty() ) 
    { return LOKI_ERROR( "No MC pi0 is found ", StatusCode::SUCCESS ) ; }
  
  Range gammas = select( "gamma" , 22 == ID ) ;
  
  Tuple tuple = nTuple("My pi0 tuple");
  for( Loop pi0 = loop( "gamma gamma" , "pi0" ) ; pi0 ; ++pi0 ) 
  {
    if( pi0->mass(1,2) > 200 * MeV ) { continue ; }
    
    tuple -> column ( "fmass" , pi0    -> mass     (1,2) ) ;
    tuple -> column ( "pi0p"  , pi0    -> momentum ( 0 ) ) ;
    tuple -> column ( "ph1"   , pi0(1) -> momentum ()    ) ;
    tuple -> column ( "ph2"   , pi0(2) -> momentum ()    ) ;
    
    // the first photon from MC pi0 
    bool m1  =
      pi0s.end() != match->match ( pi0(1) , pi0s.begin() , pi0s.end() ) ;

    // the second photon from MC pi0 
    bool m2  =
      pi0s.end() != match->match ( pi0(2) , pi0s.begin() , pi0s.end() ) ;
    
    // pi0 from MC pi0 
    bool m   =
      pi0s.end() != match->match ( pi0    , pi0s.begin() , pi0s.end() ) ;
    
    tuple->column ( "m1"  , m1  ) ;
    tuple->column ( "m2"  , m2  ) ;
    tuple->column ( "m"   , m   ) ;
    
    tuple->write();
    
  };
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
