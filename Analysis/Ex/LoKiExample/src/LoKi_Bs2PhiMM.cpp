// $Id: LoKi_Bs2PhiMM.cpp,v 1.1 2005-02-16 11:16:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Release:$
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================


LOKI_ALGORITHM( LoKi_Bs2PhiMM )
{
  using namespace LoKi          ;
  using namespace LoKi::Fits    ;
  using namespace LoKi::Cuts    ;
  
  const StatusCode ok = StatusCode::SUCCESS ;

  
  // get MC truth 
  MCMatch mc = mctruth() ;
  
  // # true Bs
  MCRange mcBs  = 
    mc->findDecays ( " [B_s0 ->  mu+   mu- (  phi(1020) ->  K+  K- ) ]cc" ) ;
  // # true phis from this decay  
  MCRange mcPhi = 
    mc->findDecays ( " [B_s0 ->  mu+   mu- ( ^phi(1020) ->  K+  K- ) ]cc" ) ;
  // # true muons from this decay
  MCRange mcMu  = 
    mc->findDecays ( " [B_s0 -> ^mu+  ^mu- (  phi(1020) ->  K+  K- ) ]cc" ) ;
  // # true kaons from this decay
  MCRange mcK   = 
    mc->findDecays ( " [B_s0 ->  mu+   mu- (  phi(1020) -> ^K+ ^K- ) ]cc" ) ;
  
  // # create MC cuts/predicates
  Cut mcCutBs  = MCTRUTH ( mc , mcBs  ) ;
  Cut mcCutPhi = MCTRUTH ( mc , mcPhi ) ;
  Cut mcCutMu  = MCTRUTH ( mc , mcMu  ) ;
  Cut mcCutK   = MCTRUTH ( mc , mcK   ) ;
  
  // # get all reconstructed primary vertices:
  VRange primaries = vselect ( "PV" , Vertex::Primary == VTYPE ) ;
  if( primaries.empty() )
  { return Warning ( "No primary vertex is found!" , ok ) ; }      // RETURN 
  
  // # minimal impact parameter of tarck with respect to ALL primary vertices
  Fun mips = MIPCHI2 ( primaries , geo() ) ;
  
  // # get all reconstructed muons with loose criteria 
  Range muons   = select ( "mu"   , ( "mu+" == ABSID ) && ( PIDmu > -5 ) ) ;
  Range muplus  = select ( "mu+"  , muons , Q > 0 ) ;
  Range muminus = select ( "mu-"  , muons , Q < 0 ) ;
  
  // # get all reconstructed kaons with loose criteria 
  Range kaons   = select ( "K"    , ( "K+"  == ABSID ) & ( PIDK  > -5 ) ) ;
  Range kplus   = select ( "K+"   , kaons , Q > 0 ) ;
  Range kminus  = select ( "K-"   , kaons , Q < 0 ) ;
  
  // # retrive  N-Tuple 
  Tuple tup = nTuple( "Bs -> Phi Mu+ Mu-" ) ;
  
  // # make a loop over 4 particles 
  for( Loop Bs = loop( "K+ K- mu+ mu-" , "B_s0" ) ; Bs ; ++Bs )
  {
    
    // # rough estimate for invariant mass 
    const double m1234 = Bs->mass() ;
    if (  m1234 < 3.0 * GeV || m1234 > 8.0 * GeV ) { continue ;} // CONTINUE 
    
    // # rough estimate for invariant KK-mass 
    const double m12   = Bs->mass(1,2) ;
    if ( m12    > 1.1 * GeV                      ) { continue ; } // CONTINUE
    
    const Particle* K1  = Bs(1) ; // the first  daughter 
    const Particle* K2  = Bs(2) ; // the second daughter 
    const Particle* Mu1 = Bs(3) ; // the third  daughter  
    const Particle* Mu2 = Bs(4) ; // the fourth daughter 
    
    const Particle* B = Bs ;     // particle itself 
    
    if ( 0 == B   || 
         0 == K1  || 0 == K2 || 
         0 == Mu1 || 0 == Mu2 ) { continue ; }                    // CONTINUE
    
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

    // # fill N-Tuple
    tup -> column ( "mass" , M        ( Bs ) / GeV ) ; // KKmm invariant mass
    tup -> column ( "chi2" , VCHI2    ( Bs )       ) ; // chi2 of vertex fit 
    tup -> column ( "m12"  , M12      ( Bs ) / GeV ) ; // KK invariant mass 
    tup -> column ( "m34"  , M34      ( Bs ) / GeV ) ; // mm invariant mass
    
    // # transverse momenta 
    tup -> column ( "pt1"   , PT      ( K1  ) / GeV ) ; // # pt of K1 
    tup -> column ( "pt2"   , PT      ( K2  ) / GeV ) ; // # pt of K2 
    tup -> column ( "pt3"   , PT      ( Mu1 ) / GeV ) ; // # pt of Mu1 
    tup -> column ( "pt4"   , PT      ( Mu2 ) / GeV ) ; // # pt of Mu2
    
    // # impact parameter significances for daughter particles 
    tup -> column ( "mip1"  , mips    ( K1  )       ) ;
    tup -> column ( "mip2"  , mips    ( K2  )       ) ;
    tup -> column ( "mip3"  , mips    ( Mu1 )       ) ;
    tup -> column ( "mip4"  , mips    ( Mu2 )       ) ;
    
    // # impact parameter significance for the whole Bs
    tup -> column ( "mips"  , mips    ( Bs  )       ) ;
    
    // # identification variables
    tup -> column ( "pidk1" , PIDK    ( K1  )       ) ; // # DLL(K-pi)  for K1 
    tup -> column ( "pidk2" , PIDK    ( K2  )       ) ; // # DLL(K-pi)  for K2 
    tup -> column ( "muid1" , PIDmu   ( Mu1 )       ) ; // # DLL(mu-pi) for Mu1 
    tup -> column ( "muid2" , PIDmu   ( Mu2 )       ) ; // # DLL(mu-pi) for Mu2
    
    // # mctruth flags
    tup -> column ( "mcbs"  , mcCutBs ( Bs  )       ) ;
    tup -> column ( "mck1"  , mcCutK  ( K1  )       ) ;
    tup -> column ( "mck2"  , mcCutK  ( K2  )       ) ;
    tup -> column ( "mcm1"  , mcCutMu ( Mu1 )       ) ;
    tup -> column ( "mcm2"  , mcCutMu ( Mu2 )       ) ;
    
    Fun dz   = VDSIGN  ( primary   ) ; // delta Z evaluator 
    Fun tof  = VDTIME  ( primary   ) ; // c*tau evaluator
    Fun dtof = VDDTIME ( primary   ) ; // c*tau evaluator ("dot"-version)
    // the cosine of angle between Bs momentum and 
    // the vector from primary vertex to decay vertex 
    Fun dira = DIRA    ( primary   ) ; 

    tup -> column ( "dz"    , dz      ( Bs )        ) ; // z(Bs)-z(PV)
    tup -> column ( "tof"   , tof     ( Bs )        ) ; // c*tau for Bs 
    tup -> column ( "dtof"  , dtof    ( Bs )        ) ; // c*tau for Bs 
    tup -> column ( "dira"  , dira    ( Bs )        ) ; // angle
    
    tup -> write() ;
    
  };
  
  return StatusCode::SUCCESS ;
};

// ============================================================================
// The END 
// ============================================================================
