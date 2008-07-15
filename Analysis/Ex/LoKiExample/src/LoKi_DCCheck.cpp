// $Id: LoKi_DCCheck.cpp,v 1.1 2008-07-15 10:18:10 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GauduKernel
// ============================================================================
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/IChronoSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/MCVertexCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"
// ============================================================================
namespace 
{
  /// helper function to evaluate the relative difference
  inline double diff ( const double var1 , const double var2 )
  { return ( var1 - var2 ) / ( var1 + var2 ) ; } 
}
// ============================================================================
/** @file
 *  simple algorithm to check the distacn ecalculator tools 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-07014
 */
// ============================================================================
LOKI_MCALGORITHM( DCCheck ) 
{
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  // get all primary vertices
  VRange primaries = vselect( "PV" , ISPRIMARY ) ;
  if ( 1 != primaries.size () ) { return StatusCode::SUCCESS ;  } // RETURN 
  
  const LHCb::VertexBase* primary = primaries.front() ;
  
  MCFinder finder  = mcFinder("My local MC-decay finder");
  
  MCRange mcBs  = finder->findDecays 
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- ) (  phi(1020) ->  K+  K- ) ]cc" ) ;
  
  if ( mcBs.empty() ) { return StatusCode::SUCCESS ; }           // RETURN


  // get true muons 

  MCRange mcMu  = finder->findDecays 
    ( "[ B_s0 -> (  J/psi(1S) -> ^mu+ ^mu- ) (  phi(1020) ->  K+  K- ) ]cc" ) ;  
  Range muons = select ( "mu" , "mu+" == ABSID && MCTRUTH ( mcTruth() , mcMu ) ) ;
  if ( 2 != muons.size() ) { return StatusCode::SUCCESS ; }        // REUTRN

  // get true kaons
  
  MCRange mcK   = finder->findDecays 
    ( "[ B_s0 -> (  J/psi(1S) ->  mu+  mu- ) (  phi(1020) -> ^K+ ^K- ) ]cc" ) ;
  Range kaons = select ( "K" , "K+" == ABSID && MCTRUTH ( mcTruth() , mcK ) ) ;
  if ( 2 != kaons.size() ) { return StatusCode::SUCCESS ; }        // REUTRN
  
  // get particles from the primary vertex 
  
  MCRange b = mcselect ( "beauty" , BEAUTY ) ;
  MCRange c = mcselect ( "charm"  , CHARM  ) ;
  
  MCRange fromPV = mcselect 
    ( "fromMV" , "pi+" == MCABSID && 
      !FROMMCTREE ( b ) && ! FROMMCTREE ( c ) && 
      abs( MCVFASPF(MCVZ) ) < 15 * Gaudi::Units::cm ) ;
  
  MCMatch mc = mcTruth() ;
  
  Range frompv = select ( "frompv" , MCTRUTH ( mc , fromPV ) ) ;

  
  std::vector<Range> ranges ;
  ranges.push_back ( frompv ) ;
  ranges.push_back ( muons  ) ;
  ranges.push_back ( kaons  ) ;
  
  
  const IDistanceCalculator* dc1 = distanceCalculator ( "1" ) ;
  const IDistanceCalculator* dc2 = distanceCalculator ( "2" ) ;


  Tuple tuple = nTuple ( "per Track" ) ;
  
  for ( std::vector<Range>::const_iterator ir = ranges.begin() ; 
        ranges.end() != ir ; ++ir ) 
  {
    const Range& range = *ir ;
    
    const bool isSignal = ranges.begin() == ir ;
    
    for ( Range::iterator _ip = range.begin() ; range.end() != _ip ; ++_ip ) 
    {
      
      const LHCb::Particle* p = *_ip ;
      if ( 0 == p ) { continue ; }
      
      tuple -> column ( "signal" , isSignal) ;
      tuple -> column ( "pt"     , PT ( p ) / Gaudi::Units::GeV ) ;
      tuple -> column ( "p"      , P  ( p ) / Gaudi::Units::GeV ) ;
      
      double ip1,ip2,ip3,ip4,chi2_3,chi2_4 ;
      
      
      {
        // global timer 
        Chrono chrono ( chronoSvc() , name() + ":track") ;
        
        for ( int i = 0 ; i < 10 ; ++i )
        {
          StatusCode sc1 = dc1 -> distance ( p , primary , ip1 ) ;
          StatusCode sc2 = dc2 -> distance ( p , primary , ip2 ) ;
          StatusCode sc3 = dc1 -> distance ( p , primary , ip3 , chi2_3 ) ;
          StatusCode sc4 = dc2 -> distance ( p , primary , ip4 , chi2_4 ) ;            
        }
      }
      

      
      tuple->column ( "ip1" , ip1 ) ;
      tuple->column ( "ip2" , ip2 ) ;
      
      tuple->column ( "ip3"    , ip3 ) ;
      tuple->column ( "chi2_3" , chi2_3 ) ;
      tuple->column ( "ip4"    , ip4 ) ;
      tuple->column ( "chi2_4" , chi2_4 ) ;
      
      tuple -> write() ;
      
      counter ( "diff-ip"     ) += diff ( ip1    , ip2    ) ;
      counter ( "diff-ipChi2" ) += diff ( chi2_3 , chi2_4 ) ;

    }
  }
  
  Tuple tup2 = nTuple ( "B" ) ;
  
  for ( Loop Bs = loop ( "mu mu K K" , "B_s0") ; Bs ; ++Bs )
  {
    
    const double m1234 = Bs->mass(1,2,3,4) / Gaudi::Units::GeV ;
    if ( m1234 < 4.5 || m1234 > 6.0 ) { continue ; }
    const double vchi2 = VCHI2 ( Bs ) ;
    if ( vchi2 < 0   || vchi2 > 49  ) { continue ; }

    const LHCb::VertexBase* vertex = Bs->vertex() ;
    const LHCb::Particle*   B      = Bs           ;
    const LHCb::Particle*   mu1    = Bs(1) ;
    const LHCb::Particle*   mu2    = Bs(2) ;
    
    if ( 0 == vertex || 0 == B || 0 == mu1 || 0 == mu2  ) { continue ; }
    

    tup2 -> column( "mass" , m1234 ) ;
    

    double vd1, vd2 ;    
    double vd3 ,vd4, vdchi2_3 , vdchi2_4 ;

    double doca1, doca2 ;
    double doca3, doca4 , dchi2_3 , dchi2_4 ;
    
    double path1 , err1 , pchi2_1 ;
    double path2 , err2 , pchi2_2 ;

    double pd1 , pderr1 ;
    double pd2 , pderr2 ;
    
    {
      // global timer 
      Chrono chrono ( chronoSvc() , name() + ":B") ;
      
      for ( int i = 0 ; i < 10 ; ++i )
      {
        StatusCode sc1  = dc1 -> distance ( vertex , primary , vd1 ) ;
        StatusCode sc2  = dc2 -> distance ( vertex , primary , vd2 ) ;

        StatusCode sc3  = dc1 -> distance ( vertex , primary , vd3 , vdchi2_3 ) ;
        StatusCode sc4  = dc2 -> distance ( vertex , primary , vd4 , vdchi2_4 ) ;

        StatusCode sc5  = dc1 -> distance ( mu1 , mu2 , doca1 ) ;
        StatusCode sc6  = dc2 -> distance ( mu1 , mu2 , doca2 ) ;
        
        StatusCode sc7  = dc1 -> distance ( mu1 , mu2 , doca3 , dchi2_3 ) ;
        StatusCode sc8  = dc2 -> distance ( mu1 , mu2 , doca4 , dchi2_4 ) ;

        StatusCode sc9  = dc1 -> pathDistance 
          ( B , primary , path1 , err1 , pchi2_1 )  ;
        
        StatusCode sc10 = dc2 -> pathDistance 
          ( B , primary , path2 , err2 , pchi2_2 )  ;

        StatusCode sc11 = dc1 -> projectedDistance 
          ( B , primary , pd1 , pderr1 )  ;
        
        StatusCode sc12 = dc2 -> projectedDistance 
          ( B , primary , pd2 , pderr2 )  ;
        
      }
    }
    
    tup2 -> column ( "vd1" , vd1 ) ;
    tup2 -> column ( "vd2" , vd2 ) ;
    tup2 -> column ( "vd3" , vd3 ) ;
    tup2 -> column ( "vd4" , vd4 ) ;
    tup2 -> column ( "vdchi2_3" , vdchi2_3 ) ;
    tup2 -> column ( "vdchi2_4" , vdchi2_4 ) ;
    
    counter ( "diff-vd"     ) += diff ( vd1      , vd2    ) ;
    counter ( "diff-vdChi2" ) += diff ( vdchi2_3 , vdchi2_4 ) ;
    
    tup2 -> column ( "doca1"   , doca1 ) ;
    tup2 -> column ( "doca2"   , doca2 ) ;
    tup2 -> column ( "doca3"   , doca3 ) ;
    tup2 -> column ( "doca4"   , doca4 ) ;
    tup2 -> column ( "dchi2_3" , dchi2_3 ) ;
    tup2 -> column ( "dchi2_4" , dchi2_4 ) ;
    
    counter ( "diff-doca"     ) += diff ( doca1   , doca2   ) ;
    counter ( "diff-docaChi2" ) += diff ( dchi2_3 , dchi2_4 ) ;

    tup2 -> column ( "path1"   , path1   ) ;
    tup2 -> column ( "path2"   , path2   ) ;
    tup2 -> column ( "err1"    , err1    ) ;
    tup2 -> column ( "err2"    , err2    ) ;
    tup2 -> column ( "pchi2_1" , pchi2_1 ) ;
    tup2 -> column ( "pchi2_2" , pchi2_2 ) ;
    
    counter ( "diff-path"     ) += diff ( path1   , path2   ) ;
    counter ( "diff-pathErr"  ) += diff ( err1    , err2    ) ;
    counter ( "diff-pathchi2" ) += diff ( pchi2_1 , pchi2_2 ) ;
    
    tup2 -> column ( "pd1"      , pd1    ) ;
    tup2 -> column ( "pd2"      , pd2    ) ;
    tup2 -> column ( "pderr1"   , pderr1 ) ;
    tup2 -> column ( "pderr2"   , pderr2 ) ;
    
    counter ( "diff-proj"     ) += diff ( pd1    , pd2    ) ;
    counter ( "diff-projErr"  ) += diff ( pderr1 , pderr2 ) ;
    
    tup2 -> write() ;
      
  }


  return StatusCode::SUCCESS ;
  
}
// ============================================================================
// The END 
// ============================================================================
