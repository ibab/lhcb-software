// $Id: LoKi_Phi.cpp,v 1.1 2007-03-22 18:33:38 ibelyaev Exp $
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
 *  Simple algorithm to polot dikaon invarinat mass 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-22
 */
// ============================================================================
LOKI_ALGORITHM(LoKi_Phi)
{
  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;

  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  // select all kaon 
  Range kaons  = select ( "kaons" , "K+" == ABSID ) ;
  Range kplus  = select ( "k+"    , kaons , 0 < Q ) ;
  Range kminus = select ( "k-"    , kaons , 0 > Q ) ;
  
  // loop over dikaons:
  for ( Loop dikaon = loop ( "k+ k-" , "phi(1020)" ) ; dikaon ; ++dikaon ) 
  {
    // fast evaluation of mass (4-momenta addition)
    const double m12 = dikaon->mass() ;
    if ( 1150 * Gaudi::Units::MeV < m12 ) { continue ; }           // CONTINUE
    // plot invarinat mass 
    plot ( M(dikaon) / Gaudi::Units::GeV  , "K+ K- mass" , 1 , 1.1 ) ; 
    const double vchi2 = VCHI2( dikaon ) ;
    if ( 0 > vchi2 || 49 < vchi2         ) { continue ; }          // CONTINUE 
    plot ( M(dikaon) / Gaudi::Units::GeV  , "K+ K- mass, chi2_vx<49" , 1 , 1.1 ) ;  
  }
  
  setFilterPassed ( true ) ;
  
  return SUCCESS ;                                                  // RETURN  
};


// ============================================================================
// The END 
// ============================================================================
