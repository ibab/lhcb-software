// $Id: LoKi_Histos.cpp,v 1.2 2004-03-03 14:17:29 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2003/07/24 16:43:50  ibelyaev
//  new package with LoKi examples 
//
// Revision 1.2  2003/05/12 13:21:33  ibelyaev
//  add the options files for all examples
//
// ============================================================================
// Include files
// LoKi 
#include "LoKi/LoKi.h"

/** @file 
 * 
 *  Implementation file for class : LoKi_Histos
 *
 *  @date  2003-03-21 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */


// ============================================================================
LOKI_ALGORITHM( LoKi_Histos )
{
  // avoid long names 
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
   
  // select all charged kaons
  Range kaons = select ( "kaons" , 321 == abs( ID ) );
  
  // plot the multiplicity of kaons 
  plot (  kaons.size() ,  " Multiplicity of charged kaons " , 0 , 100 );
  
  // fill few histos through the explicit loop over all charged kaons
  for ( Range::iterator iKaon = kaons.begin() ; kaons.end() != iKaon ; ++iKaon )
  {
    const Particle* kaon = *iKaon ;
    if ( 0 == kaon ) { continue ; }
    
    // plot the momentum of charged kaons  
    plot (  P( kaon ) / GeV , "Kaon momentum" , 0 , 100 ) ;
    
    // plot the transverse momentum of charged kaons  
    plot ( PT( kaon ) / GeV , "Kaon transverse momentum" , 0 , 5 ) ;
    
    // evaluate the beta factor for kaon 
    double beta  = P( kaon ) / E( kaon ) ;
    // plot it 
    plot ( beta , "Beta factor for charged kaons" , 0.9 , 1.01 , 101 );
    
  };
  
  // fill the histo through the implicit loop over all charged kaons
  plot ( CL , kaons.begin() , kaons.end() 
         , "Confidence Level for charged kaons" , 0 , 1 );
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END
// ============================================================================
