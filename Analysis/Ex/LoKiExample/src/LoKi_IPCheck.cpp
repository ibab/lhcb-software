// $Id: LoKi_IPCheck.cpp,v 1.2 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files 
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
/** @file
 *  The simple LoKi algorithm to check Wouter's idea about 
 *  "in-plane" and "off-plane" impact parameters 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-01
 */
// ============================================================================
namespace 
{
  inline std::pair<double,double> 
  dipdop 
  ( const LHCb::Particle*   p  , 
    const LHCb::VertexBase* pv ) 
  {
    const Gaudi::XYZVector slopes = p->slopes() ;
    
    const double tx = slopes.X () ;
    const double ty = slopes.Y () ;
    
    const Gaudi::XYZPoint& rp = p  -> referencePoint() ;
    const Gaudi::XYZPoint& rv = pv -> position () ;
    
    const double x  = rp . X () ;
    const double y  = rp . Y () ;
    const double z  = rp . Z () ;
    
    const double x0 = rv . X () ;
    const double y0 = rv . Y () ;
    const double z0 = rv . Z () ;
    
    const double dx = x0 - x - ( z0 - z ) * tx ;
    const double dy = y0 - y - ( z0 - z ) * ty ;
    
    const double n  = ::sqrt ( 1 + tx * tx + ty * ty ) ;
    const double n0 = ::sqrt (     tx * tx + ty * ty ) ;
    
    const double dip = dx * tx + dy * ty ;
    const double dop = dx * ty - dy * tx ;
    
    return std::make_pair ( dip / n / n0 , dop / n / n0 ) ;
  } 
}
// ============================================================================
LOKI_MCALGORITHM( IPCheck ) 
{
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  // get all primary vertices
  VRange primaries = vselect( "PV" , ISPRIMARY ) ;
  if ( 1 != primaries.size () ) { return StatusCode::SUCCESS ;  } // RETURN 
  
  MCFinder finder  = mcFinder("My local MC-decay finder");
  
  MCRange good  = finder->findDecays 
    ( "[ B_s0 -> (  J/psi(1S) ->  ^mu+  ^mu- ) (  phi(1020) ->  ^K+  ^K- ) ]cc" ) ;
  
  if ( 4 != good.size () ) { return StatusCode::SUCCESS ; }
  
  MCRange b = mcselect ( "beauty" , BEAUTY ) ;
  MCRange c = mcselect ( "charm"  , CHARM  ) ;
  
  MCRange fromPV = mcselect 
    ( "fromMV" , ( "mu+" == MCABSID || 
                   "K+"  == MCABSID || 
                   "pi+" == MCABSID || 
                   "p+"  == MCABSID ) && 
      !FROMMCTREE ( b ) && ! FROMMCTREE ( c ) && 
      abs( MCVFASPF(MCVZ) ) < 20 * Gaudi::Units::cm ) ;
  
  MCMatch mc = mcTruth() ;
  
  Range signal = select ( "signal" , MCTRUTH ( mc , good   ) ) ;
  Range frompv = select ( "frompv" , MCTRUTH ( mc , fromPV ) ) ;
  
  always() 
    << " #particles MC: "
    << good.size() << "/" << fromPV.size()
    << " RC: " << signal.size() << "/" << frompv.size()
    << endmsg ;
  
  std::vector<Range> ranges ;
  ranges.push_back ( signal ) ;
  ranges.push_back ( frompv ) ;


  Tuple tuple = nTuple ( "IP" ) ;

  const LHCb::VertexBase* pv = primaries.front() ;
  
  Fun ip     = IP     ( geo() , pv ) ;
  Fun ipChi2 = IPCHI2 ( geo() , pv ) ;

  // const IDistanceCalculator* t = distanceCalculator() ;
  
  for ( std::vector<Range>::const_iterator ir = ranges.begin() ; 
        ranges.end() != ir ; ++ir ) 
  {
    const Range& range = *ir ;
    
    const bool isSignal = ranges.begin() == ir ;
    
    for ( Range::iterator _ip = range.begin() ; range.end() != _ip ; ++_ip ) 
    {
      const LHCb::Particle* p = *_ip ;
      if ( 0 == p ) { continue ; }
      
      tuple -> column ( "pt"     , p->pt() / Gaudi::Units::GeV ) ;
      tuple -> column ( "ip"     , ip     ( p ) ) ;
      tuple -> column ( "ipChi2" , ipChi2 ( p ) ) ;
      
      tuple -> column ( "signal" , isSignal ) ;

      std::pair<double,double> d = dipdop ( p , pv ) ;
      
      tuple -> column ( "dip" , d.first  ) ;
      tuple -> column ( "dop" , d.second ) ;
      
      tuple -> write () ;
    } 
  }
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END 
// ============================================================================
