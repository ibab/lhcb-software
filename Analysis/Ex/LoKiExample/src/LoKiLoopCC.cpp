// $Id: LoKiLoopCC.cpp,v 1.1.1.1 2003-07-24 16:43:49 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// local
#include "LoKi/LoKi.h"

/** @file 
 *  demo/test for LoopCC construction 
 * 
 * @author Vanya  BELYAEV Ivan.Belyaev@cern.ch
 */

// ============================================================================
LOKI_ALGORITHM( LoKiLoopCC ) 
{
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  using namespace LoKi::Fits ;

  // mc truth 
  MCMatch mc     = mctruth() ;
  MCRange mcd0  = mc -> findDecays ( "D0  -> K- pi+" ) ;
  MCRange mcd0_ = mc -> findDecays ( "D~0 -> K+ pi-" ) ;
  
  if ( mcd0.empty() && mcd0_.empty() ) { return StatusCode::SUCCESS ; }
  
  Cut     d0    = MCTRUTH ( mctruth() , mcd0  ) ;
  Cut     d0_   = MCTRUTH ( mctruth() , mcd0_ ) ;
  Cut     d0mc  = d0 || d0_ ;
  
  Range kplus   = select ( "K+"  , "K+"  == ID ) ;
  Range kminus  = select ( "K-"  , "K-"  == ID ) ;
  Range piminus = select ( "pi-" , "pi-" == ID ) ;
  Range piplus  = select ( "pi+" , "pi+" == ID ) ;
  
  // normal loop 
  for ( Loop l1 = loop ( "K- pi+" , "D0" , FitVertex )  ; l1 ; ++l1 ) 
    {
      if ( l1->mass(1,2) > 2.0 * GeV ) { continue ; }
      if ( l1->mass(1,2) < 1.6 * GeV ) { continue ; }      
      if ( !d0(l1)                   ) { continue ; }
      plot ( "Loop1: K- pi+" , M(l1)/GeV , 1.6 , 2.0 , 400 ) ;
    }

  // normal loop 
  for ( Loop l2 = loop ( "K+ pi-" , "D~0" , FitVertex )  ; l2 ; ++l2 ) 
    {
      if ( l2->mass(1,2) > 2.0 * GeV ) { continue ; }
      if ( l2->mass(1,2) < 1.6 * GeV ) { continue ; }      
      if ( !d0_(l2)                   ) { continue ; }
      plot ( "Loop2: K+ pi-" , M(l2)/GeV , 1.6 , 2.0 , 400 ) ;
    }
  
  // CC-loop 
  CC ccK  ( "K-"  , "K+"  ) ;
  CC ccPi ( "pi+" , "pi-" ) ;
  CC ccD0 ( "D0"  , "D~0" ) ;
  
  for ( LoopCC l = loop ( ccK + ccPi , ccD0 , FitVertex )  ; l ; ++l ) 
    {
      if ( l->mass(1,2) > 2.0 * GeV ) { continue ; }
      if ( l->mass(1,2) < 1.6 * GeV ) { continue ; }      
      if ( !d0mc(l)                 ) { continue ; }
      plot ( "LoopCC: K pi" , M(l)/GeV , 1.6 , 2.0 , 400 ) ;
    }
  
  
  return StatusCode::SUCCESS ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
