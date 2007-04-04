// $Id: LoKi_Particles.cpp,v 1.2 2007-04-04 12:16:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
// Include files 
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================
/** @file 
 *  Simple algorithm to count various particle species
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
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
