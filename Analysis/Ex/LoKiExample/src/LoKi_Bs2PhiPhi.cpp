// $Id: LoKi_Bs2PhiPhi.cpp,v 1.1.1.1 2003-07-24 16:43:50 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/03/27 12:23:00  ibelyaev
//  new algorithm
//
// ============================================================================
// Include files 
#include "LoKi/LoKi.h"


// ============================================================================
/** @file 
 * 
 *  Implementation file for class : LoKi_Bs2PhiPhi
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-03-27 
 */
// ============================================================================


// ============================================================================
LOKI_ALGORITHM( LoKi_Bs2PhiPhi ) 
{
  // avoid long names 
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  using namespace LoKi::Fits ;

  
  const StatusCode ok    ( StatusCode::SUCCESS ) ;
  const StatusCode error ( StatusCode::FAILURE ) ;
  
  
  // get all primary vertices 
  VRange primaries = vselect( "Primaries" , Vertex::Primary == VTYPE ) ;

  if( primaries.empty() ) 
    { return LOKI_ERROR(" No primary vertices found ", ok  ) ; }    // RETURN

  // get all charged kaons 
  Range kaons = select( "kaons" , 321 == abs( ID ) && CL > 0.05 ) ;

  Range kplus  = select( "K+" , kaons , Q >  0.5 ) ;
  Range kminus = select( "K-" , kaons , Q < -0.5 ) ;
  
  Cut dmass = abs( DMASS( "phi(1020)" , ppSvc() ) ) < 20 * MeV ;
  
  Tuple tuple_1 = ntuple ( "Phi" ) ;
  
  for( Loop phi = loop( "K+ K-" , 333 ) ; phi ; ++phi ) 
    {
      // fast
      if( phi->mass(1,2) > ( 1020 + 25 ) * MeV ) {           continue ; }
      
      Record rec( tuple_1 , 
                  "mass,chi2,p,pt,lv01", 
                  M( phi ) , VCHI2 ( phi ) , 
                  P( phi ) , PT    ( phi ) , LV01( phi ) );
      
      // check the vertex 
      if( VCHI2( phi ) > 100                   ) {           continue ; }
      
      // save phi 
      if( dmass( phi )                         ) { phi->save( "phi" ) ; }
      
    };
  
  Range phis = selected( "phi" );
  
  Tuple tuple_2 = ntuple ( "Bs" ) ;
  for( Loop Bs = loop( "phi phi" , 531 ) ; Bs ; ++Bs )
    {
      if( Bs->mass(1,2) < 3 * GeV ) { continue ; }
      
      Fun m1 = MASS ( 1 ) ;
      Fun m2 = MASS ( 2 ) ;
      
      Record rec( tuple_2 , "mass,m1,m2,p,pt,lv01" , 
                  M ( Bs ) , m1 ( Bs ) , m2   ( Bs ) , 
                  P ( Bs ) , PT ( Bs ) , LV01 ( Bs ) ) ;
    }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
