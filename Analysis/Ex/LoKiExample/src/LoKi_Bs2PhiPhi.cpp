// $Id: LoKi_Bs2PhiPhi.cpp,v 1.4 2005-02-18 11:40:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/02/17 10:22:43  ibelyaev
//  update Bs-> Phi Phi example
//
// ============================================================================
// Include files 
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================


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
  using namespace LoKi          ;
  using namespace LoKi::Fits    ;
  using namespace LoKi::Cuts    ;
  
  const StatusCode ok    ( StatusCode::SUCCESS ) ;

  // get the event header 
  const EventHeader* evtHdr = 
    get<EventHeader>( EventHeaderLocation::Default ) ;
  
  // get MC truth information 
  MCMatch mc    = mctruth() ;
  MCRange mcBs  = 
    mc->findDecays ( " [ B_s0 -> (  phi(1020) ->  K+  K- ) (  phi(1020) ->  K+  K-) ]cc ") ;
  MCRange mcPhi = 
    mc->findDecays ( " [ B_s0 -> ( ^phi(1020) ->  K+  K- ) ( ^phi(1020) ->  K+  K-) ]cc ") ;
  MCRange mcK   = 
    mc->findDecays ( " [ B_s0 -> (  phi(1020) -> ^K+ ^K- ) (  phi(1020) -> ^K+ ^K-) ]cc ") ;
  
  Cut mcCutBs  = MCTRUTH ( mc , mcBs  ) ;
  Cut mcCutPhi = MCTRUTH ( mc , mcPhi ) ;
  Cut mcCutK   = MCTRUTH ( mc , mcK   ) ;
  
  // get all primary vertices 
  VRange primaries = vselect( "Primaries" , Vertex::Primary == VTYPE ) ;

  if( primaries.empty() ) 
    { return LOKI_ERROR(" No primary vertices found ", ok  ) ; }    // RETURN

  // minimal chi2 of impact parameter with respect to all primary vertices  
  Fun  mips = MIPCHI2 ( primaries ,geo() ) ;
  
  // get all charged kaons 
  Range kaons = select( "kaons" , ( "K+" == ABSID ) && ( PIDK >  -10 ) ) ;
  
  Range kplus  = select( "K+" , kaons , Q >  0.5 ) ;
  Range kminus = select( "K-" , kaons , Q < -0.5 ) ;
  
  Cut dmass = abs( DMASS( "phi(1020)" , ppSvc() ) ) < 20 * MeV ;
  
  Tuple tuple_1 = nTuple ( "Phi" ) ;
  
  for( Loop phi = loop( "K+ K-" , 333 ) ; phi ; ++phi ) 
    {
      // fast
      if ( phi->mass(1,2) > ( 1020 + 50 ) * MeV ) { continue ; }
      
      const Particle* ph = ph ;
      if ( 0 == ph            ) { continue ; }
      
      const Particle* K1 = child ( phi , 1 ) ;
      const Particle* K2 = child ( phi,  2 ) ;

      if ( 0 == K1 || 0 == K2 ) { continue ; }

      // event header
      tuple_1 << Tuples::Column( "" , evtHdr ) ;
      // K+ K- invariant mass 
      tuple_1 -> column ( "mass"  , M        ( phi ) / GeV  ) ;
      // chi2 of vertex fit for common K+ K- vertex   
      tuple_1 -> column ( "chi2v" , VCHI2    ( phi )        ) ;
      // momentum of   K+ K- combination 
      tuple_1 -> column ( "p"     , P        ( phi ) / GeV  ) ;
      // transverse momentum of   K+ K- combination 
      tuple_1 -> column ( "pt"    , PT       ( phi ) / GeV  ) ;
      // cosine of decay angle  LV01 
      tuple_1 -> column ( "lv01"  , LV01     ( phi )        ) ;
      //  transverse momentum of K+
      tuple_1 -> column ( "pt1"   , PT       ( K1  ) / GeV  ) ;
      //  transverse momentum of K-
      tuple_1 -> column ( "pt1"   , PT       ( K2  ) / GeV  ) ;
      //  delta Log Likelihood(K/pi) for K+
      tuple_1 -> column ( "pidk1" , PIDK     ( K1  )        ) ;
      //  delta Log Likelihood(K/pi) for K-
      tuple_1 -> column ( "pidk2" , PIDK     ( K2  )        ) ;
      //  delta Log Likelihood(K/e) for K+
      tuple_1 -> column ( "pide1" , (PIDK-PIDe) ( K1  )     ) ;
      //  delta Log Likelihood(K/e) for K-
      tuple_1 -> column ( "pide2" , (PIDK-PIDe) ( K2  )     ) ;
      //  delta Log Likelihood(K/e) for K+
      tuple_1 -> column ( "pidp1" , (PIDK-PIDp) ( K1  )     ) ;
      //  delta Log Likelihood(K/e) for K-
      tuple_1 -> column ( "pidp2" , (PIDK-PIDp) ( K2  )     ) ;
      //  minimal chi2 of IP with respect for all primary vertices for phi 
      tuple_1 -> column ( "mipphi", mips     ( phi )        ) ;
      //  minimal chi2 of IP with respect for all primary vertices for K+ 
      tuple_1 -> column ( "mipk1" , mips     ( K1  )        ) ;
      //  minimal chi2 of IP with respect for all primary vertices for K- 
      tuple_1 -> column ( "mipk2" , mips     ( K2  )        ) ;
      // mc flag ( ==1 if both kaons form the true decay)
      tuple_1 -> column ( "mcphi" , mcCutPhi ( phi )        ) ;
      // mc flag ( ==1 if K+ is a kaon the true decay) 
      tuple_1 -> column ( "mcK1"  , mcCutK   ( K1  )        ) ;
      // mc flag ( ==1 if K0 is a kaon the true decay)
      tuple_1 -> column ( "mcK2"  , mcCutK   ( K2  )        ) ;
      
      tuple_1 -> write  () ;
      
      // check the vertex 
      if( VCHI2( phi ) > 100                   ) {           continue ; }
      
      // save phi 
      if( dmass( phi )                         ) { phi->save( "phi" ) ; }
      
    };
  
  Range phis = selected( "phi" );
  
  Tuple tuple_2 = nTuple ( "Bs" ) ;
  for( Loop Bs = loop( "phi phi" , 531 ) ; Bs ; ++Bs )
  {
    if ( Bs->mass(1,2) < 3 * GeV ) { continue ; }
    if ( Bs->mass(1,2) > 7 * GeV ) { continue ; }
    
    Fun m1 = MASS ( 1 ) ;
    Fun m2 = MASS ( 2 ) ;
    
    const Particle* phi1 = child ( Bs , 1 ) ;
    const Particle* phi2 = child ( Bs , 1 ) ;    
    if ( 0 == phi1 || 0 == phi2 ) { continue ; }
    
    const Particle* K1   = child ( phi1 , 1 ) ;
    const Particle* K2   = child ( phi1 , 2 ) ;
    if ( 0 == K1   || 0 == K2   ) { continue ; }
    
    const Particle* K3 = child ( phi2 , 1 ) ;
    const Particle* K4 = child ( phi2 , 2 ) ;
    if ( 0 == K3   || 0 == K4   ) { continue ; }
    
    const Particle* B  = Bs ;
    if ( 0 == B                 ) { continue ; }
    
    // # from upstream PVxs select PVx with minimal B-impact parameter
    VFun vipChi2 =  VIPCHI2 ( B , geo  () ) ;
    VCut vCut    = ( vipChi2 >= 0 ) && ( VPSD( B , geo  () )  > -0.5 * mm ) ;
    const Vertex* primary  = 
      SelectPrimaryVertexMin ( primaries.begin ()   , // begin of vertices
                               primaries.end   ()   , // end of vertices
                               vipChi2              , // to be minimized 
                               vCut                 ) ; // constraints  
    if ( 0 ==  primary )  
    { Warning ( "Could not find the approproate primary vertex" ) ; continue ; }
    
    // event header
    tuple_2 << Tuples::Column( "" , evtHdr ) ;
    // invariant mass of 2 phi candidates 
    tuple_2 -> column ( "mass"   , M      ( Bs   ) / GeV ) ;
    // invariant mass of the first  phi candidate 
    tuple_2 -> column ( "m1"     , M      ( phi1 ) / GeV ) ;
    // invariant mass of the second phi candidate 
    tuple_2 -> column ( "m2"     , M      ( phi2 ) / GeV ) ;
    // chi2 of vertex fit 
    tuple_2 -> column ( "chi2v"  , VCHI2  ( Bs   )       ) ;
    // transverse momenta 
    tuple_2 -> column ( "pt"     , PT     ( Bs   ) / GeV ) ;
    tuple_2 -> column ( "pt1"    , PT     ( phi1 ) / GeV ) ;
    tuple_2 -> column ( "pt2"    , PT     ( phi2 ) / GeV ) ;
    tuple_2 -> column ( "ptk1"   , PT     ( K1   ) / GeV ) ;
    tuple_2 -> column ( "ptk2"   , PT     ( K2   ) / GeV ) ;
    tuple_2 -> column ( "ptk3"   , PT     ( K3   ) / GeV ) ;
    tuple_2 -> column ( "ptk4"   , PT     ( K4   ) / GeV ) ;
    //  minimal chi2 of IP with respect for all primary vertices  
    tuple_2 -> column ( "mips"   , mips   ( Bs   )       ) ;
    tuple_2 -> column ( "mip1"   , mips   ( phi1 )       ) ;
    tuple_2 -> column ( "mip2"   , mips   ( phi2 )       ) ;
    tuple_2 -> column ( "mipk1"  , mips   ( K1   )       ) ;
    tuple_2 -> column ( "mipk2"  , mips   ( K2   )       ) ;
    tuple_2 -> column ( "mipk3"  , mips   ( K3   )       ) ;
    tuple_2 -> column ( "mipk4"  , mips   ( K4   )       ) ;
    // identification 
    //  delta Log Likelihood(K/pi) for K
    tuple_2 -> column ( "pidk1" , PIDK    ( K1  )        ) ;
    tuple_2 -> column ( "pidk2" , PIDK    ( K2  )        ) ;
    tuple_2 -> column ( "pidk1" , PIDK    ( K3  )        ) ;
    tuple_2 -> column ( "pidk2" , PIDK    ( K4  )        ) ;
    //  delta Log Likelihood(K/e) for K
    tuple_2 -> column ( "pide1" , (PIDK-PIDe)( K1  )     ) ;
    tuple_2 -> column ( "pide2" , (PIDK-PIDe)( K2  )     ) ;
    tuple_2 -> column ( "pide3" , (PIDK-PIDe)( K3  )     ) ;
    tuple_2 -> column ( "pide4" , (PIDK-PIDe)( K4  )     ) ;
    //  delta Log Likelihood(K/p) for K
    tuple_2 -> column ( "pidp1" , (PIDK-PIDp)( K1  )     ) ;
    tuple_2 -> column ( "pidp2" , (PIDK-PIDp)( K2  )     ) ;
    tuple_2 -> column ( "pidp3" , (PIDK-PIDp)( K3  )     ) ;
    tuple_2 -> column ( "pidp4" , (PIDK-PIDp)( K4  )     ) ;
    // cosine of decay angles 
    tuple_2 -> column ( "lv01"   , LV01   ( Bs   )       ) ;
    tuple_2 -> column ( "lvph1"  , LV01   ( phi1 )       ) ;
    tuple_2 -> column ( "lvph2"  , LV01   ( phi2 )       ) ;
    // 
    Fun dz   = VDSIGN  ( primary   ) ; // delta Z evaluator 
    Fun tof  = VDTIME  ( primary   ) ; // c*tau evaluator
    Fun dtof = VDDTIME ( primary   ) ; // c*tau evaluator ("dot"-version)
    // the cosine of angle between Bs momentum and 
    // the vector from primary vertex to decay vertex 
    Fun dira = DIRA    ( primary   ) ; 
    
    tuple_2 -> column ( "dz"     , dz     ( Bs  )        ) ; // z(Bs)-z(PV)
    tuple_2 -> column ( "tof"    , tof    ( Bs  )        ) ; // c*tau for Bs 
    tuple_2 -> column ( "dtof"   , dtof   ( Bs  )        ) ; // c*tau for Bs 
    tuple_2 -> column ( "dira"   , dira   ( Bs  )        ) ; // angle
    
    // MC flags 
    tuple_2 -> column ( "mcBs"   , mcCutBs  ( Bs   )     ) ;
    tuple_2 -> column ( "mcph1"  , mcCutPhi ( phi1 )     ) ;
    tuple_2 -> column ( "mcph2"  , mcCutPhi ( phi2 )     ) ;
    tuple_2 -> column ( "mcK1"   , mcCutK   ( K1   )     ) ;
    tuple_2 -> column ( "mcK2"   , mcCutK   ( K2   )     ) ;
    tuple_2 -> column ( "mcK3"   , mcCutK   ( K3   )     ) ;
    tuple_2 -> column ( "mcK4"   , mcCutK   ( K4   )     ) ;
    tuple_2 -> write  () ;
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
