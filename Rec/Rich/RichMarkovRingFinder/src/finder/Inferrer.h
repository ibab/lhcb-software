
#ifndef RICHMARKOV_INFERRER_H
#define RICHMARKOV_INFERRER_H

//#include <set>
#include <map>
#include "RichParams.h"
#include "RichPriors.h"
#include "Data.h"

namespace RichMarkov {
  
  template <class Mode, const Mode & mode>
  class Inferrer {
  public:
    typedef Data<Mode, mode> DataT;
    typedef RichPriors<Mode, mode> RichPriorsT;
    typedef RichParams<Mode, mode> RichParamsT;
    typedef CircleParams<Mode, mode> CircleParamsT;
  private:
    const DataT & m_data;
    const typename DataT::Hits & m_hits;
    const RichParamsT & m_richParams;
    const typename RichParamsT::Circs & m_circs;
    const double m_sigmaMuCorrected;
  public:
    Inferrer(const DataT & data, const RichParamsT & rps) :
    m_data(data),
    m_hits(data.hits),
    m_richParams(rps), 
    m_circs(rps.circs),
    m_sigmaMuCorrected(rps.sigmaMuGeometricallyCorrected()) {
    };
    typedef typename DataT::Hits::const_iterator HitIterator;
    typedef typename RichParamsT::Circs::const_iterator CircleIterator;
    //typedef std::set<HitIterator> QueriedHits;
    typedef std::pair<HitIterator, CircleIterator> QueryPair;
    typedef std::map<QueryPair, double> QueryMap;
  private:
    //mutable QueriedHits queriedHits;
    mutable QueryMap queryMap;
  public:
    inline double probabilityHitWasMadeBySomethingOtherThanACircle(const HitIterator & hIt) const {
      return probabilityHitWasMadeByGivenCircle(hIt, m_circs.end());
    };
    inline double probabilityHitWasMadeByGivenCircle(const HitIterator & hIt, const CircleIterator & cIt) const {
      //const QueriedHits::const_iterator setIt = queriedHits.find(hIt);
      //const bool haveAlreadyProcessedThisHit = (setIt != queriedHits.end());
      const QueryPair queryPair(hIt, cIt);
      const typename QueryMap::const_iterator queryPairIt = queryMap.find(queryPair);
      const bool haveAlreadyProcessedThisQueryPair = (queryPairIt != queryMap.end());

      if (haveAlreadyProcessedThisQueryPair) {
	// draw an answer from the cache
	return (*queryPairIt).second;
      } else {
	// must process this hit for the first time ... and put answers in for all circles
	cacheAnswersForAllCirclesWRTHit(hIt);
	const typename QueryMap::const_iterator newQueryPairIt = queryMap.find(queryPair);
	// because of the above cache line, we should now be able to assert the following:
	assert(newQueryPairIt != queryMap.end());
	return (*newQueryPairIt).second;
      };
    };
  private:
    inline void cacheAnswersForAllCirclesWRTHit(const HitIterator & hIt) const {
      const Hit & hit = *hIt;
      QueryMap newPartOfQueryMap;
      double total = 0;
      for(CircleIterator cIt = m_circs.begin(); 
	  cIt != m_circs.end();
	  ++cIt) {
	const CircleParamsT & circle = *cIt;
	const double probPart1 = RichPriorsT::priorProbabilityOfHitDueToCircle(hit, circle);
	const double probPart2 = circle.meanNumberOfHitsGeometricallyCorrected();
	const double proportionalToProb = probPart1 * probPart2;
	total += proportionalToProb;

	const QueryPair newQueryPair(hIt, cIt);
	// add "initial un-normalised estimate" to map:
	newPartOfQueryMap[newQueryPair]=proportionalToProb;
	// (normalise later below!)
      };
      
      {
	// correct for the presence of background
	const double probPart1 = RichPriorsT::priorProbabilityOfHitDueToBackground(hit);
	const double probPart2 = m_richParams.meanBackground;
	const double proportionalToProb = probPart1 * probPart2;
	total += proportionalToProb;
	
	const QueryPair newQueryPair(hIt, m_circs.end());
	// add "initial un-normalised estimate" for background to map:
	newPartOfQueryMap[newQueryPair]=proportionalToProb;
	// (normalise later below!)	
      };

      // now normalise the new map 
      for (typename QueryMap::iterator it = newPartOfQueryMap.begin();
	   it!=newPartOfQueryMap.end();
	   ++it) {
	(*it).second /= total;
      };

      // finally add this query map to the end of the existing map:
      queryMap.insert(newPartOfQueryMap.begin(), newPartOfQueryMap.end());
    };
  };

}

#endif
