
#include "RichInferrer.h"

// includes
#include "GenericRingFinder/GenericInferrer.h"
#include "GenericRingFinder/GenericInput.h"
#include "GenericRingFinder/GenericResults.h"
#include "Hit.h"
#include "CircleParams.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

namespace Lester
{

  template < class DATAMODEL >
  RichInferrer<DATAMODEL>::~RichInferrer() {}

  // Return probablility hit was made by the given circle
  template < class DATAMODEL >
  double RichInferrer<DATAMODEL>::
  probabilityHitWasMadeByGivenCircle(const GenRingF::GenericInput::GenericHits::const_iterator & hIt,
                                     const GenRingF::GenericResults::GenericRings::const_iterator & cIt) const
  {
    //const QueriedHits::const_iterator setIt = queriedHits.find(hIt);
    //const bool haveAlreadyProcessedThisHit = (setIt != queriedHits.end());
    const QueryPair queryPair(hIt, cIt);
    const QueryMap::const_iterator queryPairIt = queryMap.find(queryPair);
    const bool haveAlreadyProcessedThisQueryPair = (queryPairIt != queryMap.end());

    if (haveAlreadyProcessedThisQueryPair) {
      // draw an answer from the cache
      return (*queryPairIt).second;
    } else {
      // must process this hit for the first time ... and put answers in for all circles
      cacheAnswersForAllCirclesWRTHit(hIt);
      const QueryMap::const_iterator newQueryPairIt = queryMap.find(queryPair);
      // because of the above cache line, we should now be able to assert the following:
      assert(newQueryPairIt != queryMap.end());
      return (*newQueryPairIt).second;
    }
  }

  // Probability hit was made by no known circles
  template < class DATAMODEL >
  double RichInferrer<DATAMODEL>::
  probabilityHitWasMadeBySomethingOtherThanACircle(const GenRingF::GenericInput::GenericHits::const_iterator & hIt) const
  {
    return probabilityHitWasMadeByGivenCircle(hIt, m_circs.end());
  }

  template < class DATAMODEL >
  void RichInferrer<DATAMODEL>::
  cacheAnswersForAllCirclesWRTHit(const HitIterator & hIt) const
  {
    const Hit hit(*hIt);
    QueryMap newPartOfQueryMap;
    double total = 0;
    for ( CircleIterator cIt = m_circs.begin();
          cIt != m_circs.end(); ++cIt )
    {
      const CircleParams circle(*cIt);
      const double probPart1 = m_ntrm->priorProbabilityOfHitDueToCircle(hit, circle);
      const double probPart2 = 1;// FIX GEOMETRICAL ACCEPTANCE! LESTER:  const double probPart2 = circle.meanNumberOfHitsGeometricallyCorrected();
      const double proportionalToProb = probPart1 * probPart2;
      total += proportionalToProb;

      const QueryPair newQueryPair(hIt, cIt);
      // add "initial un-normalised estimate" to map:
      newPartOfQueryMap[newQueryPair]=proportionalToProb;
      // (normalise later below!)
    }

    // correct for the presence of background
    const double probPart1 = m_ntrm->priorProbabilityOfHitDueToBackground(hit);
    const double probPart2 = m_meanBackground;
    const double proportionalToProb = probPart1 * probPart2;
    total += proportionalToProb;

    const QueryPair newQueryPair(hIt, m_circs.end());
    // add "initial un-normalised estimate" for background to map:
    newPartOfQueryMap[newQueryPair]=proportionalToProb;
    // (normalise later below!)

    // now normalise the new map
    const double min_value = boost::numeric::bounds<double>::smallest();
    if ( total>0 )
    {
      for ( QueryMap::iterator it = newPartOfQueryMap.begin();
            it!=newPartOfQueryMap.end();
            ++it )
      {
        // This is horrid, but avoids FPE underflows ...
        if ( (*it).second>0 &&
             std::log((*it).second)-std::log(total) < std::log(min_value) )
        {
          (*it).second = min_value;
        }
        else
        {
          (*it).second /= total;
        }
      }
    }

    // finally add this query map to the end of the existing map:
    queryMap.insert(newPartOfQueryMap.begin(), newPartOfQueryMap.end());
  }

}


// Instanciate specific templates

#include "Rich/Rich1DataModel.h"
template class Lester::RichInferrer<Lester::Rich1DataModel>;

#include "Rich/Rich2DataModel.h"
template class Lester::RichInferrer<Lester::Rich2DataModel>;
