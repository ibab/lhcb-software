// $Id: LoKi_Bs2PhiGamma.cpp,v 1.4 2005-02-12 16:11:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// LoKi itself 
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================
// DaVinciTools 
// ============================================================================
#include "DaVinciTools/IPhotonParams.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/EventHeader.h"
// ============================================================================

/** @file 
 *
 *  Implementation file for class : LoKi_Bs2PhiGamma
 * 
 *  @date 2003-03-11 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
LOKI_ALGORITHM( LoKi_Bs2PhiGamma ) 
{
  // avoid long names 
  using namespace LoKi                ;  
  using namespace LoKi::Cuts          ;  
  using namespace LoKi::Fits          ;
  
  // get all primary vertices with :
  //    number of tracks > 4 and reasonably good Chi2 
  VRange v1 = 
    vselect( "Good PVs" , 
             VTYPE == Vertex::Primary && VTRACKS > 4 && VCHI2 / VDOF < 10 );
  
  plot ( VTRACKS , v1.begin() , v1.end() , 
         "Tracks per vertices " , 0 , 20 , 20 );
  
  // select only 1 good PV with maximal number of tracks
  Vertex* pv = select_max( v1 , VTRACKS );
  if( 0 == pv ) 
    { return LOKI_ERROR( "No primary vertex found!", StatusCode::SUCCESS ) ; }
  
  // helper object to find MC decays 
  MCMatch mc = mctruth();
  // all MC phis 
  MCRange phi_mc = mc -> findDecays ( "phi(1020) -> K+K-"                    ) ;
  // all MC Bs 
  MCRange bs_mc  = mc -> findDecays ( "B_s0 -> ( phi(1020) ->K+ K- ) gamma " ) ;

  Cut mc_phi = MCTRUTH( mctruth() , phi_mc ) ;
  Cut mc_bs  = MCTRUTH( mctruth() , bs_mc  ) ;
  
  Cut vxcut   = 
    IP( point( pv ) ) > 50 * micrometer && IPCHI2( point( pv ) ) > 4 ;
  
  Cut kaons   = vxcut && PT > 200 * MeV && 321 == abs( ID ) && CL > 0.05 ;
  
  Range kplus  = select ( "K+" , Q >  0.5 && kaons ) ;
  Range kminus = select ( "K-" , Q < -0.5 && kaons ) ;

  Cut dm = abs( DMMASS("phi(1020)", ppSvc() ) ) < 15 * MeV ;
  
  for( Loop phi = loop( "K+ K-" , 333 ) ; phi ; ++phi ) 
    {    
      if( phi->mass(1,2) > 1.1 * GeV ) { continue ; }
      if( VCHI2( phi )   > 49        ) { continue ; }
      plot ( M( phi ) / GeV ,  "K+ K- mass " , 0.950 , 1.100 , 60 );
      
      if( dm( phi ) ) phi->save( "phi" );
      
      if ( mc_phi( phi ) ) 
      { plot ( M( phi ) / GeV ,  "(MC) K+ K- mass " , 0.950 , 1.100 , 60 ); }
    }
  
  Range phis = selected("phi");
  plot ( phis.size() ,  " number of phis " , phis.size() , 0 , 20 , 40 );
  
  // find high-pt photon
  Range gamma = select("gamma" , 1 * GeV < PT && ID == 22 );
  plot (  gamma.size() , " number of gammas  " , gamma.size() , 0 , 20 , 40 );
  
  // redefine gamma parameters 
  photonTool()->process( gamma.begin() , gamma.end() , pv );
  
  Fun theta = DIRA  ( point( pv ) );
  Fun time  = VDTIME( point( pv ) );
  Fun dist  = VDSIGN( point( pv ) );
  
  const EventHeader* evthdr = get<EventHeader>( EventHeaderLocation::Default );
  if( 0 == evthdr ) { return StatusCode::SUCCESS ; }
  
  Tuple tuple2 = nTuple(" my n-tuple number 2 ");
  Loop Bs = loop ( "phi gamma" , "B_s0" , FitNone ) ;
  Bs->setPV( pv );
  for( ; Bs ; ++Bs ) 
  {
    
    Bs->fit( FitDirection ) ; 
    
    child ( Bs , 2 , 1  ) ;
    child ( Bs , 2 ) ;
    plot(  M( Bs ) / GeV , "phi gamma mass " , 4.0 , 6.0  );
    if( mc_bs( Bs ) ) 
    { 
      plot(  M( Bs ) / GeV , "(MC) phi gamma mass " , 4.0 , 6.0 ); 
      
      tuple2->column ( "BsP"  , Bs->p()      ) ;
      tuple2->column ( "tb"   , theta( Bs )  ) ;
      tuple2->column ( "time" , time ( Bs )  ) ;
      tuple2->column ( "dist" , dist ( Bs )  ) ;
      tuple2->column ( "zpv"  , VZ( pv )     ) ;
      
      {
        // 
        // commented by V.B. 2005-02-11
        // tuple2->column ( "pv"   , pv->position() ) ;
        //
        const LoKi::Point3D& _pos = pv->position() ;
        tuple2->column ( "pv" , LoKi::ThreeVector( _pos.x() , 
                                                   _pos.y() , 
                                                   _pos.z() ) ) ;
      }
      
      tuple2 << Tuples::Column( "" , evthdr ) ;
      
      Record rec     ( tuple2 , "vxb,vyb,vzb,vxpv,vypv,vzpv" , 
                       VX( Bs ) , VY( Bs ) , VZ( Bs ) , 
                       VX( pv ) , VY( pv ) , VZ( pv ) ) ;

      Bs->save("Bs") ;
      
    }
  }
  
  {
    Range Bs = selected("Bs") ;
    if ( !Bs.empty() ) { setFilterPassed( true ) ; }  
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

