// $Id: LoKi_Demo1.cpp,v 1.1.1.1 2003-07-24 16:43:50 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/05/18 14:39:12  ibelyaev
//  new examples
// 
// ============================================================================
// Include files
#include "LoKi/LoKi.h"

/** @file 
 *
 *  The simplest 'demo' algoritm for LoKi.
 *  It demonstrates the primitive selection/filtering of particles 
 *  with simplest kinematical criteria, like transverse momentum, 
 *  confidence level, charge and identifier 
 *
 *  @date   2003-05-13 
 *  @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
LOKI_ALGORITHM( LoKi_Demo1 ) 
{
  // avoid long names 
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  
  MsgStream log( msgSvc() , name() );
  
  // select all charged kaons (both positive and negative)
  Range kaons = select ( "kaons" , 321 == abs( ID ) );
  
  // select the positive kaons from selected kaons 
  Range kplus = select ( "k+"    , kaons , Q >  0.5 ) ;
  
  // select well identified fast negative kaons
  Range kgood = select ( "goodK" , ID == "K-" && CL > 0.10 && PT > 500 * MeV );
  
  // select a kaon with maximal energy  
  const Particle* kaon = select_max( kaons , E ) ;
  const double    emax = 0 == kaon ? -1 * TeV : kaon->momentum().e() ;
  
  log << MSG::ALWAYS
      << " #Kaons: "            << kaons.size () 
      << " \t Positive: "       << kplus.size ()
      << " \t Good negative: "  << kgood.size ()
      << " \t E(max): "         << emax / GeV << " GeV " << endreq ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
