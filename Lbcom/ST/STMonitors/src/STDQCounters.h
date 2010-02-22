// $Id: STDQCounters.h,v 1.3 2010-02-22 13:00:37 nchiapol Exp $
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

#include <algorithm>
#include <iterator>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/mean.hpp>
//#include <boost/accumulators/statistics/density.hpp>

using namespace boost::accumulators;


class STDQCounters {

public:
 
  /// constructor
  STDQCounters();
  STDQCounters(int minADC, int maxADC);
  
  int    chargeMPV();
  void   addCharge(double);

  /// typedef template for counters
  template <typename T> 
  struct Type {
    typedef accumulator_set<T, stats<tag::sum, tag::mean> > Counter;
  };

  /** Counters */
  unsigned int m_event;                 ///< number of events
  Type<int>   ::Counter m_sumClusters;  ///< total clusters
  Type<int>   ::Counter m_sumNoise;     ///< noise clusters
  Type<double>::Counter m_sumEff;       ///< processing efficiency
  Type<int>   ::Counter m_nError;       ///< total number of error banks
  Type<int>   ::Counter m_nCorrupted;   ///< corrupted banks
  Type<int>   ::Counter m_sumMissing;   ///< missing banks
 
  typedef std::vector<std::string> Strings;
  static Strings m_txtColumns;    ///< text file column headers
   
  
  /// Struct containing the data collected for one run
  struct DataRow {
    int          run;             ///< run number
    unsigned int event;           ///< number of events
    double       clus;            ///< mean number of clusters
    double       noise;           ///< mean number of noise clusters
    double       procEff;         ///< mean processing efficiency
    int          error;           ///< total number of error banks
    int          corrupted;       ///< total number of corrupted banks
    int          sumMissing;      ///< total number of missing banks
    int          chargeMPV;       ///< Most probable charge value
  };


private:
  void   init(int minADC, int maxADC);  
  
  std::vector< int > m_chargeHist;
  int   m_minADC, m_maxADC;
  bool  m_entries;
};

STDQCounters::Strings STDQCounters::m_txtColumns(0, "");

void STDQCounters::init(int minADC, int maxADC) {
  m_event = 0;

  //prepare m_chargeHist
  m_minADC = minADC;
  m_maxADC = maxADC;
  m_chargeHist.resize(m_maxADC-m_minADC);
  m_entries = false;

  if (!m_txtColumns.size()) {
    m_txtColumns.push_back("Run");
    m_txtColumns.push_back("Events");
    m_txtColumns.push_back("Clusters/evt");
    m_txtColumns.push_back("#Noise/event");
    m_txtColumns.push_back("Proc Eff");
    m_txtColumns.push_back("#ErrorBanks");
    m_txtColumns.push_back("#Corrupted");
    m_txtColumns.push_back("#Missing");
    m_txtColumns.push_back("Charge MPV");
    m_txtColumns.push_back("Comments");
  }
}

STDQCounters::STDQCounters() {
  init(15,45);
}
STDQCounters::STDQCounters(int minADC, int maxADC) {
  init(minADC,maxADC);
}

void STDQCounters::addCharge(double charge) {
  if ((charge < m_minADC) || (charge >= m_maxADC)) {
    return;
  }
  m_entries = true;
  m_chargeHist[(int)(floor(charge)-m_minADC)]++;
}

int STDQCounters::chargeMPV() {
  if (!m_entries) {
    return 0;
  }
  return std::distance(m_chargeHist.begin(), std::max_element(m_chargeHist.begin(), m_chargeHist.end()))+m_minADC;
}   

#endif
