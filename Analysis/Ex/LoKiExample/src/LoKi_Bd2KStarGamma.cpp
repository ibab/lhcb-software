// $Id: LoKi_Bd2KStarGamma.cpp,v 1.2 2003-11-30 16:37:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2003/07/24 16:43:49  ibelyaev
//  new package with LoKi examples 
//
// Revision 1.2  2003/03/27 15:47:38  ibelyaev
//  update the algorithm
//
// Revision 1.1  2003/03/26 19:26:52  ibelyaev
//  new 'realistic' algorithm
// 
// ============================================================================
// Include files
#include "LoKi/LoKi.h"
// Event 
#include "Event/EventHeader.h"
// GaudiKernel
#include "GaudiKernel/IRegistry.h"
// DaVinciTools 
#include "DaVinciTools/IPhotonParams.h"
// ============================================================================
/** @file 
 *  
 *  (Pre)-Selection algorithm for Bd -> K*(892) gamma selection 
 *
 *    - Analysis N-Tuple is filled 
 *    - The Event Tag Collection is created 
 * 
 *   @date   2003-03-26 
 *
 *   @author Galina PAKHLOVA Galina.Pakhlova@cern.ch
 *   @author Vanya  BELYAEV  Ivan.Belyaev@itep.ru 
 *
 */
// ============================================================================

// ============================================================================
LOKI_ALGORITHM( LoKi_Bd2KStarGamma )
{
  // avoid long names 
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  using namespace LoKi::Fits ;
  
  const StatusCode ok    ( StatusCode::SUCCESS ) ;
  const StatusCode error ( StatusCode::FAILURE ) ;
//-----------------------------------------------------------------------------
  // The general information: event header, event address  

  const EventHeader* hdr = get( eventSvc ()                  , 
                                EventHeaderLocation::Default , hdr ) ;
  if( 0 == hdr      ) { return StatusCode::FAILURE ; }
  
  DataObject*     event    = get( eventSvc() , "/Event" , event );
  if( 0 == event    ) { return StatusCode::FAILURE ; }
  IRegistry*      registry = event->registry() ;
  if( 0 == registry ) { return StatusCode::FAILURE ; }
  IOpaqueAddress* address  = registry->address () ;
  if( 0 == address  ) { return StatusCode::FAILURE ; }
//---------------------------------------------------------------    
  //  All Primary Vertices   
  VRange primaries =  vselect( "Primaries", VTYPE == Vertex::Primary ) ;
  
  if( primaries.empty() ) 
    { return LOKI_ERROR( "No primary vertices are found" , ok ); }
//---------------------------------------------------------------  
  //   PRESELECTION CUTS  
  
  Cut gamma_cut      =  ID == 22 && PT > 1.5 * GeV ;
  Cut kaon_cut       =  abs(ID) == 321 && CL > 0.05;
  Cut pion_cut       =  abs(ID) == 211 && CL > 0.05;
  
  Cut  Kst_kin_cut   = 
    abs(DMMASS("K*(892)0" , ppSvc())) < 0.2*GeV && PT > 0.75*GeV ;
  VCut vchi2_Kst_cut = VCHI2 < 49 ;
  
  Cut mass_b_cut =  4.0 * GeV < M  && M < 6.0 * GeV ;
  double  ptB_gam_cut = 1.6*GeV ;
//---------------------------------------------------------------   
  Range gamma  = select( "gamma", gamma_cut ) ;
  if ( gamma.empty () ) { return StatusCode::SUCCESS  ; }      // RETURN  
  
  Range kaons  = select( "K" , kaon_cut ) ;
  if ( kaons.empty () ) { return StatusCode::SUCCESS  ; }      // RETURN 
   
  Range pions  = select( "pi", pion_cut ) ;
  if ( pions.empty () ) {  return StatusCode::SUCCESS ; }      // RETURN 
 //---------------------------------------------------------------   
  // save K*0, perform the vertex fit  
  for ( Loop Kst = loop( "K pi" , 313 ) ; Kst ; ++Kst )  
    {
      // "+" & "-" combinations
      if ( 0 != Q( Kst(1) )+ Q( Kst ( 2 ) ) )  { continue ; }
      if ( !Kst_kin_cut   ( Kst )           )  { continue ; } // CONTINUE
      if ( !vchi2_Kst_cut ( Kst )           )  { continue ; } // CONTINUE
      //  save K*0 as "a particle" 
      Kst->save( "K*0" );
    }
  // get all saved K*0s 
  Range kstar = selected( "K*0" ) ;
  if( kstar.empty() ) { return StatusCode::SUCCESS ; }        // RETURN
//---------------------------------------------------------------    
  
  // the counter for combinations 
  size_t  combinations = 0 ;
  
  // get the tuple for analysis  
  Tuple tuple = ntuple("Selection");
  // loop over K*0 gamma combinations 
  for ( Loop B0 = loop( "K*0 gamma" , "B0" , FitNone ) ; B0 ; ++B0 ) 
    {
      const Particle* K      = child( B0 , 1 , 1 ) ;
      const Particle* pi     = child( B0 , 1 , 2 ) ;
      const Particle* photon = child( B0 , 2     ) ;
      if( 0 == K || 0 == pi || 0 == photon )                 // CONTINUE  
        { LOKI_ERROR ( " Invalid daughter particle" , error ) ; continue ; }
      
       if( ! mass_b_cut( B0 ) )               { continue ; } // CONTINUE 

      // get all "upstream" primary vertices 
      std::vector<const Vertex*> prims ;
      LoKi::select ( primaries.begin    ()           , 
                     primaries.end      ()           , 
                     std::back_inserter ( prims )    , 
                     VPSD( B0 , geo() )  > -200 * micrometer ) ;
      
      // from all upstream primary vertices select 
      // the primary vertex with minimal B-impact parameter
      const Vertex* primary  = select_min( prims , VIP( B0 , geo() ) ) ;
      
      //       // from all primary vertices select 
      //       // the primary vertex with minimal B-impact parameter
      // const Vertex* primary  = select_min( primaries , VIP( B0 , geo() ) ) ;
      
      if ( 0 ==  primary )                   { continue ; }  // CONTINUE 
//---------------------------------------------------------------         
      //  FUNCTIONS DEFINITION
      
      Fun ip      =   IP     ( point ( primary ) ) ; // impact parameter
      Fun ipchi2  =   IPCHI2 ( point ( primary ) ) ; // IP chi2 evaluator
      Fun dz      =   VDSIGN ( point ( primary ) ) ; // delta Z evaluator 
      Fun tof     =   VDTIME ( point ( primary ) ) ; // c*tau evaluator
      Fun dang    =   DIRA   ( point ( primary ) ) ; // the angle
//---------------------------------------------------------------  
      // kaon & pions do not point to primary vertex   
      if( ipchi2 ( K )  < 4 ) { continue ; } // !!! PRESELECTION CUT !!! 
      if( ipchi2 ( pi ) < 4 ) { continue ; } // !!! PRESELECTION CUT !!! 
//---------------------------------------------------------------  
      // re-evaluate the photon parameters at B-decay ( K*0 ) vertex
      photonTool () -> process( B0->particle ( 2 ) , B0 -> vertex() ) ;
      
      // refit the B vertex 
      B0 -> fit( FitVertex ) ;
//---------------------------------------------------------------        
      Cut cuts = dz > 0 && dang > 0.995 && tof/0.482 < 6  ;   // 
      if( !cuts( B0 ) ) { continue ; }                        // CONTINUE 
//---------------------------------------------------------------        
      const HepLorentzVector& p_B0    = B0->p(0);
      const HepLorentzVector& p_Kst   = B0->p(1);
      const HepLorentzVector& p_gamma = B0->p(2);

      // ### Gamma momentum in B meson rest frame
      double ptB_gam =  p_gamma.vect().perp(p_B0.vect());
      if( ptB_gam < ptB_gam_cut ) { continue ; }                // CONTINUE  

      // ### K*0 helicity(: angle between P_B0 & P_K boosted into K*0 rest frame
      HepLorentzVector p_B0_tmp = p_B0;          // tmp vector
      HepLorentzVector p_K_tmp  = K->momentum(); // tmp vector
      // Boost of tmp vector into K*0 rest frame:
      p_B0_tmp.boost(-p_Kst.boostVector()); 
      p_K_tmp.boost(-p_Kst.boostVector()); 
      double helicity = cos( p_B0_tmp.vect().angle(p_K_tmp.vect()) );
//---------------------------------------------------------------             
  
      tuple -> column ( "" , hdr ) ; // the event header 
      // the index of the combinations 
      tuple -> column ( "comb" ,    (long) combinations++    ) ; 
      // number of selected primary vertices 
      tuple -> column ( "nprims" ,  (long) primaries.size () ) ; 
      // number of selected photons 
      tuple -> column ( "ngamma" ,  (long)    gamma.size () ) ; 
      // the number of selected K*0s  
      tuple -> column ( "nkstar" ,  (long)    kstar.size () ) ; 
      
       // impact parameter, delta z , c*tau , angle B0 
      tuple -> fill ( "ipb,ipchi2d,dz,tof,dang" , 
                      ip (B0),ipchi2(B0),dz(B0),tof(B0),dang(B0) );
      
      tuple->fill ( "mb,ptb",    M(B0)/GeV, PT(B0)/GeV);
      // confidence level, transverse momentum of gamma, 
      tuple->fill ( "clgam,ptgam,ptbgam", 
                    CL(B0(2)),   PT(B0(2))/GeV, ptB_gam/GeV);
      // transverse momentum , mass , vertex chi2, helicity of K*0
      tuple->fill ( "ptkst,mkst", PT(B0(1))/GeV, M(B0(1))/GeV);      
      tuple->fill ( "kstchi2,hel" , VCHI2(B0(1)->endVertex()),helicity  );   
      // confidence level, transverse momentum, impact parameter of K & pi
      tuple -> fill( "clk,ptk,ipk,ipchi2k"     ,    
                     CL(K) ,PT(K), ip(K), ipchi2(K)  ) ;
      tuple -> fill( "clpi,ptpi,ippi,ipchi2pi" , 
                     CL(pi),PT(pi),ip(pi),ipchi2(pi) ) ;
      
      tuple -> column ( "Gamma" , photon -> momentum() ) ;
      tuple -> column ( "Kstar" , B0(1)  -> momentum() ) ;
      tuple -> column ( "B0"    , B0     -> momentum() ) ;
      
      tuple -> column ( "BV"    , B0 -> vertex() -> position() ) ;
      tuple -> fill   ( "bvchi2,bvdof" ,  VCHI2(B0),VDOF(B0));  
      tuple -> column ( "PV"    , primary -> position() ) ;
      tuple -> fill   ( "pvchi2,pvdof,pvntr", 
                        VCHI2   ( primary ) , 
                        VDOF    ( primary ) ,
                        VTRACKS ( primary ) ); 
      
      tuple -> write() ;
    
      // save B0 as a particle 
      B0->save( "B0" ) ;
    }
//---------------------------------------------------------------             
  // get the all saved B0s
  Range B0s = selected( "B0" ) ;
  if( B0s.empty() ) { return StatusCode::SUCCESS ; }                // RETURN 
  
  // Event Tag collection
  Tuple tags = evtCollection( "Tags" ) ;
  // Tuple tags = ntuple ( "Tags" ) ;
  
  // put event /run into tags 
  tags -> column ( "" , hdr ) ;
  
  // put the address 
  tags -> column ( "Address" , address );
  
  // primary vertices
  tags -> column ( "nprims"  , (long) primaries.size () ) ;
  
  // photons, array pf photon P/PT  
  tags -> farray ( "ptgamma"     , PT          ,
                   "egamma"      , E           ,
                   gamma.begin() , gamma.end() , 
                   "ngamma"      , 20          ) ;
  
  // kstars, array of mass  
  tags -> farray ( "mkstar"      , M           , 
                   kstar.begin() , kstar.end() , 
                   "nkstar"      , 500         ) ;
  
  tags -> farray ( "mass"        , M           , 
                   "p"           , P           , 
                   "M1"          , MASS(1)     , 
                   B0s.begin ()  , B0s.end ()  , 
                   "nB0"         , 100         ) ;

  Fun pt1 = CHILD( PT , 1 ) ;
  
  tags -> farray ( "PT"          , PT          , 
                   "pt1"         , pt1         ,   
                   "pt2"         , CHILD( PT , 2 ) ,   
                   B0s.begin ()  , B0s.end ()  , 
                   "nB0"         , 100         ) ;
  
  tags -> write   ()       ;
  
  setFilterPassed ( true ) ;
  
  return StatusCode::SUCCESS ;
  
};
// ============================================================================
// The END 
// ============================================================================



