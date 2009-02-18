#include "Kernel/HitPattern.h"

namespace LHCb
{
  HitPattern::HitPattern( const std::vector<LHCbID>& ids ) 
  {
    for( std::vector<LHCbID>::const_iterator id = ids.begin() ;
	 id != ids.end(); ++id) {
      switch( id->detectorType() ) {
      case LHCbID::Velo:
	{
	  LHCb::VeloChannelID veloid = id->veloID() ;
	  unsigned int station = (veloid.sensor()%64)/2 ; 
	  unsigned int side    = (veloid.sensor()%2) ; 
	  unsigned int type    = (veloid.sensor()/64) ;
	  // now deal with the PU sensors. we reserve the first two
	  // bits for those. if you leave these lines away it will
	  // still work, but pile-up detectors will end up in separate
	  // bitsets.
	  station += 2*(1-type/2) ;
	  type     = type%2 ;
	  m_velo[side+2*type].set(station) ;
	}
       	break ;
      case LHCbID::TT:
	{
	  LHCb::STChannelID stid = id->stID() ;
	  unsigned int uniquelayer = (stid.station()-1)*2 + stid.layer()-1 ;
	  m_tt.set(uniquelayer) ;
	}
	break ;
      case LHCbID::IT:
	{
	  LHCb::STChannelID stid = id->stID() ;
	  unsigned int uniquelayer = (stid.station()-1)*4 + stid.layer()-1 ;
	  m_it[stid.detRegion()-1].set(uniquelayer) ;
	}
	break ;
      case LHCbID::OT:
	{
	  LHCb::OTChannelID otid = id->otID() ;
	  unsigned int uniquelayer = (otid.station()-1)*4 + otid.layer() ;
	  m_ot[otid.quarter()].set(uniquelayer) ;
	}
	break ;
      case LHCbID::Muon:
	{
	  LHCb::MuonTileID muonid = id->muonID() ;
	  std::cout << "muon station  : " <<  muonid.station() << std::endl ;
	  m_muon.set( muonid.station()-1 ) ;
	}
	break ;
      default:
	;
      }
    }
  }

  std::ostream& HitPattern::fillStream(std::ostream& s) const
  {
    s << "veloRA:   " << m_velo[VeloRA] << std::endl
      << "veloRC:   " << m_velo[VeloRC] << std::endl
      << "veloPhiA: " << m_velo[VeloPhiA] << std::endl
      << "veloPhiC: " << m_velo[VeloPhiC] << std::endl
      << "TT    :   " << m_tt << std::endl
      << "IT-top:   " << m_it[ITTop] << std::endl
      << "IT-bot:   " << m_it[ITBottom] << std::endl
      << "IT-A:     " << m_it[ITA] << std::endl
      << "IT-C:     " << m_it[ITC] << std::endl
      << "OT-topA:  " << m_ot[OTTopA] << std::endl
      << "OT-botA:  " << m_ot[OTBottomA] << std::endl
      << "OT-topC:  " << m_ot[OTTopC] << std::endl
      << "OT-botC:  " << m_ot[OTBottomC] << std::endl
      << "Muon:     " << m_muon << std::endl ;
    return s ;
  }

  // collapses the T layer pattern to a station pattern
  inline std::bitset<3> hitStations( const std::bitset<HitPattern::NumT>& layers ) 
  {
    unsigned long pat = layers.to_ulong() ;
    std::bitset<3> rc ;
    rc.set(0, pat & 0xF ) ;
    rc.set(1, pat & 0xF0 ) ;
    rc.set(2, pat & 0xF00 ) ;
    return rc ;
  }
  
  // return the first bit that is set. returns N if no bits are set.
  template<size_t N>
  int firstbitset(std::bitset<N> mask) {
    int n(0) ;
    for( ; n<int(N) && !mask.test(n) ; ++n) {}
    return n ;
  }

  // return the last bit that is set. returns -1 if no bits are set.
  template<size_t N>
  int lastbitset(std::bitset<N> mask) {
    int n(N-1) ;
    for( ; n>=0 && !mask.test(n) ; --n) {}
    return n ;
  }
  
  size_t HitPattern::numVeloStationsOverlap() const {
    return (( m_velo[VeloRA] | m_velo[VeloPhiA] ) & 
	    ( m_velo[VeloRC] | m_velo[VeloPhiC] )).count() ;
  } 
  
  size_t HitPattern::numITStationsOverlap() const {
    return (( hitStations( m_it[ITC] ) & hitStations( m_it[ITBottom] ) )|
	    ( hitStations( m_it[ITC] ) & hitStations( m_it[ITTop] ) )|
	    ( hitStations( m_it[ITA] ) & hitStations( m_it[ITBottom] ) )|
	    ( hitStations( m_it[ITA] ) & hitStations( m_it[ITTop] ) )).count() ;
  }
  
  size_t HitPattern::numITOTStationsOverlap() const {
    return ( hitStations( it() ) & hitStations( ot() ) ).count() ;
  }
  
  size_t HitPattern::numVeloHoles() const {
    std::bitset<NumVelo> veloPhi = m_velo[VeloPhiA] | m_velo[VeloPhiC] ;
    std::bitset<NumVelo> veloR   = m_velo[VeloRA] | m_velo[VeloRC] ;
    std::bitset<NumVelo> velo = veloPhi | veloR ;
    size_t rc(0) ;
    if( velo.any() ) {
      // don't count missing hits in first or last R/phi pair
      int firstbit = firstbitset(velo) ;
      int lastbit  = lastbitset(velo) ;
      for( int n=firstbit+1; n<lastbit; ++n) 
	rc += (veloPhi.test(n) ? 0 : 1) + (veloR.test(n) ? 0 : 1) ;
      //std::bitset<NumVelo> mask = velo ;
      //for( int n=firstbit+1; n<lastbit; ++n) mask.set(n) ;
      //rc = (mask^veloPhi).count() + (mask^veloR).count() ;
    }
    return rc ;
  }
  
  size_t HitPattern::numTHoles() const {
    std::bitset<NumT> layers = 
      m_it[ITC] | m_it[ITBottom] |
      m_it[ITA] | m_it[ITTop] |
      m_ot[OTTopA] | m_ot[OTTopC] | 
      m_ot[OTBottomA] | m_ot[OTBottomC]  ;
    size_t rc(0) ;
    if(layers.any()) {
      int firstbit = firstbitset(layers) ;
      int lastbit  = lastbitset(layers) ;
      for( int n=firstbit+1; n<lastbit; ++n) 
	rc += layers.test(n) ? 0 : 1 ;
    }
    return rc ;
  }
}

