#include "TrackKernel/TrackCloneData.h"
#include "Kernel/HitPattern.h"
#include "Event/Track.h"

namespace {
  template <class Container>
  size_t nCommonEntries(const Container& lhs, const Container& rhs) {
    size_t rc(0) ;
    typename Container::const_iterator first1 = lhs.begin() ;
    typename Container::const_iterator last1  = lhs.end() ;
    typename Container::const_iterator first2 = rhs.begin() ;
    typename Container::const_iterator last2  = rhs.end() ;
    while (first1 != last1 && first2 != last2)
      if (*first1 < *first2)
	++first1;
      else if (*first2 < *first1)
	++first2;
      else {
	++first1;
	++first2;
	++rc ;
      }
    return rc ;
  }
  
  template <class Container>
  double overlapFraction(const Container& lhs, const Container& rhs) {
    size_t minsize = std::min( lhs.size(), rhs.size()) ;
    return minsize>0 ? nCommonEntries(lhs,rhs) / double(minsize) : 0 ;
  }
}

namespace LHCb
{
  TrackCloneData::TrackCloneData(const LHCb::Track& atrack)
    : m_track(&atrack), m_weight(0)
  {
    const LHCb::Track::LHCbIDContainer& ids = atrack.lhcbIDs() ;
    // WH: catch (and ignore) extremely rare and quite inconsequential
    // (cross-your-fingers or believe-me-I-know-what-I'm-doing)
    // exceptions from StaticArray::push_back
    //try {
    for( LHCb::Track::LHCbIDContainer::const_iterator id = ids.begin() ;
         id != ids.end(); ++id ) {
      switch( id->detectorType() ) {
      case LHCb::LHCbID::VP:
      case LHCb::LHCbID::Velo:
        if( id->isVeloR() ) m_ids[VeloR].push_back(*id) ;
        else m_ids[VeloPhi].push_back(*id) ;
        break ;
      case LHCb::LHCbID::UT:
      case LHCb::LHCbID::TT: m_ids[TT].push_back(*id) ; break ;
      case LHCb::LHCbID::FT:
      case LHCb::LHCbID::OT: 
      case LHCb::LHCbID::IT: m_ids[T].push_back(*id) ; break ;
      default: break ;
      }
    }
    //} catch (...) {}
    
    // compute a weight for sorting.
    LHCb::HitPattern hp(ids) ;
    // first sort by type
    int weightFromType[10] = {0,11,10,20,15,16,12,2,3} ; 
    m_weight = 100000 * weightFromType[m_track->type()] ;
    // then sort by the number of TT planes
    m_weight += 10000*hp.tt().count() ;
    // then sort by number of T layers / velo _clusters_
    m_weight += 100*hp.numTLayers() ;
    m_weight += 100*( (hp.veloRA()|hp.veloRC()) & (hp.veloPhiA()|hp.veloPhiC()) ).count() ;
    // only finally sort by total number of hits
    m_weight += ids.size() ;
  }
  
  size_t TrackCloneData::nCommon( const TrackCloneData& rhs,
					TrackCloneData::HitType type ) const
  {
    return nCommonEntries( m_ids[type],rhs.m_ids[type] ) ;
  }
  
  double TrackCloneData::overlapFraction( const TrackCloneData& rhs,
					  TrackCloneData::HitType type ) const
  {
    size_t minsize = std::min(m_ids[type].size(),rhs.m_ids[type].size()) ;
    return minsize>0 ? nCommonEntries( m_ids[type],rhs.m_ids[type] )/double(minsize) : 0 ;
  }
}
