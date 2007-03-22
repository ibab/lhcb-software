// $Id: LoKi_Particles.cpp,v 1.1 2007-03-22 18:33:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================
/** @file 
 *  Simple algorithm to count various particle species
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-22
 */
// ============================================================================
LOKI_ALGORITHM(LoKi_Particles)
{
  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;

  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  Range pions = select ( "pion" , "pi+"  == ABSID ) ;
  Range kaons = select ( "kaon" , "K+"   == ABSID ) ;
  Range muons = select ( "muon" , "mu+"  == ABSID ) ;
  
  
  counter( "#muons" ) += muons.size() ;
  counter( "#kaons" ) += kaons.size() ;
  counter( "#pions" ) += pions.size() ;
  
  info() << " # of found mu/K/pi = "
         <<  muons.size() << " / "
         <<  kaons.size() << " / "
         <<  pions.size() << endreq ;
  
  setFilterPassed ( true ) ;
  
  return SUCCESS ;                                                  // RETURN  
};


// ============================================================================
// The END 
// ============================================================================
