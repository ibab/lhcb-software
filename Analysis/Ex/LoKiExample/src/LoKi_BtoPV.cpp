// $Id: LoKi_BtoPV.cpp,v 1.1 2005-06-02 10:02:55 ibelyaev Exp $
// =============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// =============================================================================
// $Log: not supported by cvs2svn $ 
// =============================================================================
// Include files
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/LoKi.h"
// =============================================================================

/** @file
 *  Simple algorihtm to illistrate hot to store the Primary Vertex, 
 *  associated with the B-candidate
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-06-02
 */
 

LOKI_ALGORITHM( LoKi_BtoPV ) 
{
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  using namespace LoKi::Fits ;
  
  const StatusCode OK = StatusCode::SUCCESS ;
  
  // get MC truth information 
  MCMatch mc    = mctruth() ;
  MCRange mcBs  = 
    mc->findDecays ( " [ B_s0 -> (  phi(1020) ->  K+  K- ) (  phi(1020) ->  K+  K-) ]cc ") ;
  MCRange mcPhi = 
    mc->findDecays ( " [ B_s0 -> ( ^phi(1020) ->  K+  K- ) ( ^phi(1020) ->  K+  K-) ]cc ") ;
  MCRange mcK   = 
    mc->findDecays ( " [ B_s0 -> (  phi(1020) -> ^K+ ^K- ) (  phi(1020) -> ^K+ ^K-) ]cc ") ;
  
  // get all primary vertices 
  VRange primaries = vselect( "Primaries" , Vertex::Primary == VTYPE ) ;
  if ( primaries.empty() ) 
  { return LOKI_ERROR ( " No primary vertices found " , OK  ) ; }    // RETURN
  
  // get all true kaons form the decay 
  Range kaons  = select ( "kaon" , "K+" == ABSID && MCTRUTH( mc , mcK ) ) ;
  Range kplus  = select ( "K+"   , kaons , Q > 0 ) ;
  Range kminus = select ( "K-"   , kaons , Q < 0 ) ;
  
  Cut dmass = abs ( DMASS( "phi(1020)" , ppSvc() ) ) < 12 * MeV ;
  
  for ( Loop phi = loop ( "K+ K-" , "phi(1020)" ) ; phi ; ++phi ) 
  {
    // fast 
    if ( phi->mass ( 1 , 2 ) > ( 1020 + 50 ) * MeV ) { continue ; }
    // chi2 cut 
    if ( VCHI2 ( phi ) > 100 ) { continue ;  } 
    // DM cut 
    if ( dmass( phi ) ) { phi -> save ( "phi" ) ; }
  }
  
  for ( Loop Bs = loop ( "phi phi" , "B_s0" ) ; Bs ; ++Bs )
  {
    const double m12 = Bs->mass( 1 , 2 )  ;
    // DM cut 
    if ( m12 < 4.5 * GeV || m12 > 6 * GeV ) { continue ; }
    // chi2 vertex 
    if ( VCHI2( Bs ) > 100                ) { continue ; }
    if ( 0 == Bs->particle()              ) { continue ; }
    // find Primary Vertex:
    // # from upstream PVxs select PVx with minimal B-impact parameter significance 
    VFun vipChi2 =  VIPCHI2 ( Bs , geo  () ) ;
    VCut vCut    = ( vipChi2 >= 0 ) && ( VPSD( Bs , geo  () )  > -0.5 * mm ) ;
    VRange::iterator iprim  = 
      LoKi::select_min ( primaries.begin ()   , // begin of vertices
                         primaries.end   ()   , // end of vertices
                         vipChi2              , // to be minimized 
                         vCut                 ) ; // constraints  
    if ( primaries.end() == iprim ) 
    { Warning ( "'Good' primary vertex is not found " )  ; continue ; }
    const Vertex* prim = *iprim ;
    if ( 0 == prim ) 
    { Warning ( "'Good' primary vertex pints to NULL " ) ; continue ; }
    //
    // save Bs-candidate 
    Bs->save("Bs") ;
    // store the associted PV, use "weight" 
    VFun vip = VIPCHI2( Bs , geo() ) ;
    asctPVs ( Bs , prim , vip( prim ) ) ;
    //
  };

  Range B = selected("Bs") ;
  
  for ( Range::iterator iB = B.begin() ; B.end() != iB ; ++iB ) 
  {
    P2PVRange pvs = PVs( *iB ) ;
    always() << "B-candidate, #mass "       << M(*iB) / GeV 
             << " is assosciated with #pv " <<  pvs.size() 
             << endreq ;
  }

  if ( !B.empty() ) { setFilterPassed ( true ) ; }
  
  return StatusCode::SUCCESS ;
};





// =============================================================================
// The END 
// =============================================================================
