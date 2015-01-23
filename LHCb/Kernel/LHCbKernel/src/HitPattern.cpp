#include "Kernel/HitPattern.h"

namespace LHCb
{
    
    HitPattern::HitPattern( const std::vector<LHCbID>& ids  ) {

      for (const auto& id: ids ) {
  	  switch( id.detectorType() ) {
		case LHCbID::Velo:
		{
		    LHCb::VeloChannelID veloid = id.veloID() ;
		    unsigned int station = (veloid.sensor()%64)/2 ; 
		    unsigned int side    = (veloid.sensor()%2) ; 
		    unsigned int type    = (veloid.sensor()/64) ;
		    // now deal with the PU sensors. we reserve the first two
		    // bits for those. if you leave these lines away it will
		    // still work, but pile-up detectors will end up in separate
		    // bitsets.
		    station += 2*(1-type/2) ;
		    type     = type%2 ;
		 
		    switch( side+2*type ) {
		    case 0:
		      m_veloRA.set(station) ;
		      break ;
		    case 1:
		      m_veloRC.set(station) ; 
		      break;
		    case 2:
		      m_veloPhiA.set(station) ;
		      break;
		    case 3:
		      m_veloPhiC.set(station) ;
		      
		    }
		}
		break ;
                case LHCbID::VP:
		{
		  // FIXME: WH: the problem is that we have some
		  // interesting logic to cpmpute velo holes, which
		  // doesn't work for VP. this is not very neat, but
		  // it is a solution for now. I may also have swapped
		  // A and C side.
		  LHCb::VPChannelID vpid = id.vpID() ;
		  unsigned int station = vpid.station() ;
		  switch( vpid.sidepos() ) {
		  case 0:
		    m_veloRA.set(station) ;
		    m_veloPhiA.set(station) ;
		    break ;
		  case 1:
		    m_veloRC.set(station) ; 
		    m_veloPhiC.set(station) ;
		    break;
		  }		    
		}
		break ;
		case LHCbID::UT:
		case LHCbID::TT:
		{
		    LHCb::STChannelID stid = id.stID() ;
		    unsigned int uniquelayer = (stid.station()-1)*2 + stid.layer()-1 ;
		    m_tt.set(uniquelayer) ;
		}
		break ;
	        case LHCbID::IT:
		{
		    LHCb::STChannelID stid = id.stID() ;
		    unsigned int uniquelayer = (stid.station()-1)*4 + stid.layer()-1 ;
		    if (stid.detRegion() == 1 || stid.detRegion() == 2)
		      m_itAC.set(uniquelayer);
		    else
		      m_itTopBottom.set(uniquelayer);
		}
		break ;
		case LHCbID::OT:
		{
		    LHCb::OTChannelID otid = id.otID() ;
		    unsigned int uniquelayer = (otid.station()-1)*4 + otid.layer() ;
		    
		    if ((otid.quarter()==0 || otid.quarter()==2) && otid.module()==9)
		      if (otid.straw() > 32)
			m_ot1stMonoLayer.set(uniquelayer);
		      else
			m_ot2ndMonoLayer.set(uniquelayer);
		    else
		      if (otid.straw() > 64)
			m_ot1stMonoLayer.set(uniquelayer);
		      else
			m_ot2ndMonoLayer.set(uniquelayer);
		}
		break ;
		case LHCbID::FT:
		{
		   // we could also fill in OT, but it doesn't really matter now
		   m_itAC.set(id.ftID().layer());
		}
		break ;
		case LHCbID::Muon:
		{
		    m_muon.set( id.muonID().station() ) ;
		}
		break ;
	    }
	}
    }
    
    
    std::ostream& HitPattern::fillStream(std::ostream& s) const
    {
	s << "veloRA:             " << m_veloRA << std::endl
	  << "veloRC:             " << m_veloRC << std::endl
	  << "veloPhiA:           " << m_veloPhiA << std::endl
	  << "veloPhiC:           " << m_veloPhiC << std::endl
	  << "TT:                 " << m_tt << std::endl
	  << "IT-top-bottom:      " << m_itTopBottom << std::endl
	  << "IT-AC:              " << m_itAC << std::endl
	  << "OT-1st-mono-layer:  " << m_ot1stMonoLayer << std::endl
	  << "OT-2nd-mono-layer:  " << m_ot2ndMonoLayer << std::endl
	  << "Muon:     " << m_muon << std::endl ;
	return s ;
    }
    
    // collapses the T layer pattern to a station pattern
    inline std::bitset<3> hitStations( const std::bitset<LHCb::HitPattern::NumT>& layers ) 
    {
	unsigned long pat = layers.to_ulong() ;
	std::bitset<3> rc ;
	rc.set(0, (pat & 0xF)   !=0 ) ;
	rc.set(1, (pat & 0xF0)  !=0 ) ;
	rc.set(2, (pat & 0xF00) !=0 ) ;
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
	return (( m_veloRA | m_veloPhiA ) & 
		( m_veloRC | m_veloPhiC )).count() ;
    } 
    
    size_t HitPattern::numITStationsOverlap() const {
	return ( hitStations( m_itAC ) & hitStations( m_itTopBottom ) ).count() ;
    }
    
    size_t HitPattern::numITOTStationsOverlap() const {
	return ( hitStations( it() ) & hitStations( ot() ) ).count() ;
    }
    
    size_t HitPattern::numVeloHoles() const {
	std::bitset<NumVelo> veloPhi = m_veloPhiA | m_veloPhiC ;
	std::bitset<NumVelo> veloR   = m_veloRA | m_veloRC ;
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
	    m_itAC | m_itTopBottom |
	    m_ot1stMonoLayer | m_ot2ndMonoLayer;
	size_t rc(0) ;
	if(layers.any()) {
	    int firstbit = firstbitset(layers) ;
	    int lastbit  = lastbitset(layers) ;
	    for( int n=firstbit+1; n<lastbit; ++n) 
		rc += layers.test(n) ? 0 : 1 ;
	}
	return rc ;
    }


    bool HitPattern::operator==(const HitPattern& hitPat) const{
    
      return (m_veloRA == hitPat.veloRA() &&
	      m_veloRC == hitPat.veloRC() &&
	      m_veloPhiA == hitPat.veloPhiA() &&
	      m_veloPhiC == hitPat.veloPhiC() &&
	      m_ot1stMonoLayer == hitPat.ot1stMonoLayer() &&
	      m_ot2ndMonoLayer == hitPat.ot2ndMonoLayer() &&
	      m_itAC == hitPat.itAC() &&
	      m_itTopBottom == hitPat.itTopBottom() &&
	      m_tt == hitPat.tt() &&
	      m_muon == hitPat.muon() );
  }
}
