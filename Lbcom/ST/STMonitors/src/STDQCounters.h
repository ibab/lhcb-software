// $Id: STDQCounters.h,v 1.1 2010-02-02 16:40:59 nchiapol Exp $
#ifndef STDQCounters_H
#define STDQCounters_H 1

/** @class STDQCounters STDQCounters.h
 *
 *  Wrapper-Class containing the counters
 *  for STDQSummaryAlg 
 *
 *  @author N.Chiapolini
 *  @date   1/2/2010
 */

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/mean.hpp>

using namespace boost::accumulators;

class STDQCounters {

public:
 
  /// constructer
  STDQCounters();

  /// typedef template for counters
  template <typename T> 
  struct Type {
    typedef accumulator_set<T, stats<tag::sum, tag::mean> > Counter;
  };

  
  /** Counters */
  unsigned int m_event;                 ///< number of events
  Type<int>   ::Counter m_sumNoise;     ///< noise clusters
  Type<int>   ::Counter m_sumClusters;  ///< total clusters
  Type<double>::Counter m_sumEff;       ///< processing efficiency
  Type<int>   ::Counter m_nCorrupted;   ///< corrupted banks
  Type<int>   ::Counter m_sumMissing;   ///< missing banks

};

STDQCounters::STDQCounters() {
  m_event = 0;
}

#endif 
