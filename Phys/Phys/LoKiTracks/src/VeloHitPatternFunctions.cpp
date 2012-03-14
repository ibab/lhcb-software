// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LHCb
// ============================================================================
#include "Kernel/HitPattern.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/VeloHitPatternFunctions.h"
// ============================================================================
/** @file
 *  Implementation file for classed form the file
 *  LoKi/VeloHitPatternFunctions.h
 *
 *  @author Wouter Hulsbergen
 *  @author Pieter David
 *  @date 2012-03-12
 */
// ============================================================================



// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::MaxNumConsecutiveVeloSpacePoints::result_type
LoKi::Tracks::MaxNumConsecutiveVeloSpacePoints::operator()
  ( LoKi::Tracks::MaxNumConsecutiveVeloSpacePoints::argument t ) const
{
  LHCb::HitPattern p(t->lhcbIDs()) ;
  // get the R and Phi hit patterns
  std::bitset<LHCb::HitPattern::NumVelo> veloR = p.veloRA() | p.veloRC();
  std::bitset<LHCb::HitPattern::NumVelo> veloPhi = p.veloPhiA() | p.veloPhiC();
  // make a logical 'and' to get the number of stations with a cluster
  std::bitset<LHCb::HitPattern::NumVelo> veloclusters = veloR & veloPhi;
  // now count the number of consecutive stations with a cluster
  int numconsecutive(0) ;
  int maxnumconsecutive(0) ;
  for(int i=0; i<LHCb::HitPattern::NumVelo; ++i) {
    if( veloclusters.test(i) ) {
      ++numconsecutive ;
      if( maxnumconsecutive<numconsecutive)
        maxnumconsecutive=numconsecutive ;
    } else {
      numconsecutive = 0 ;
    }
  }
  return maxnumconsecutive ;

}



// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::NumVeloSpacePoints::result_type
LoKi::Tracks::NumVeloSpacePoints::operator()
  ( LoKi::Tracks::NumVeloSpacePoints::argument t ) const
{
  LHCb::HitPattern p(t->lhcbIDs()) ;
  return ( (p.veloRA()|p.veloRC()) & (p.veloPhiA()|p.veloPhiC()) ).count() ;
}



// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::NumVeloACOverlapHits::result_type
LoKi::Tracks::NumVeloACOverlapHits::operator()
  ( LoKi::Tracks::NumVeloACOverlapHits::argument t ) const
{
  LHCb::HitPattern p(t->lhcbIDs()) ;
  return p.numVeloStationsOverlap() ;
}



// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::NumVeloACOverlapHitsR::result_type
LoKi::Tracks::NumVeloACOverlapHitsR::operator()
  ( LoKi::Tracks::NumVeloACOverlapHitsR::argument t ) const
{
  LHCb::HitPattern p(t->lhcbIDs()) ;
  return ( p.veloRA() & p.veloRC() ).count() ;
}



// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::NumVeloACOverlapHitsPhi::result_type
LoKi::Tracks::NumVeloACOverlapHitsPhi::operator()
  ( LoKi::Tracks::NumVeloACOverlapHitsPhi::argument t ) const
{
  LHCb::HitPattern p(t->lhcbIDs()) ;
  return ( p.veloPhiA() & p.veloPhiC() ).count() ;
}



// ============================================================================
// The END
// ============================================================================
