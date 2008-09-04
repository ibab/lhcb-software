// $Id: HltRoutingBitsWriter.h,v 1.2 2008-09-04 12:21:49 graven Exp $
#ifndef HLTCOMMON_HLTROUTINGBITSWRITER_H 
#define HLTCOMMON_HLTROUTINGBITSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/IHltDecisionPredicateFactory.h"
#include "boost/array.hpp"
#include <algorithm>

/** @class HltRoutingBitsWriter HltRoutingBitsWriter.h
 *  
 *
 *  @author Gerhard Raven
 *  @date   2008-07-29
 */
class HltRoutingBitsWriter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltRoutingBitsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRoutingBitsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
private:

  typedef IHltDecisionPredicateFactory::decisionPredicate combiner_t;
  boost::array<combiner_t*,3*sizeof(unsigned int)> m_evaluators;

  bool evaluate(size_t i,const LHCb::HltDecReports& reports) { return m_evaluators[i]!=0 && (*m_evaluators[i])(reports); }
  std::ostream& print(std::ostream& os, size_t i) 
  { os << " " << i << " : " ; 
    return (m_evaluators[i]!=0) ? m_evaluators[i]->print(os) : (os << "[none]");}

  void updateEvaluators(Property&);
  void updateEvaluators(const IHltDecisionPredicateFactory&);
  void zeroEvaluators() { std::fill(m_evaluators.begin(),m_evaluators.end(),(combiner_t*)0); }
 
  std::string m_location;
  std::map<unsigned int,std::string> m_specifications;

  IHltDecisionPredicateFactory  *m_factory;
  bool                           m_updateRequired;


};
#endif // HLTROUTINGBITSWRITER_H
