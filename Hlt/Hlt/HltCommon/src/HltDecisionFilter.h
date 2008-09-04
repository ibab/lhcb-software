// $Id: HltDecisionFilter.h,v 1.1 2008-09-04 12:25:20 graven Exp $
#ifndef HLTCOMMON_HLTDECISION_H 
#define HLTCOMMON_HLTDECISION_H 1

// Include files
// from Gaudi

#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/IHltDecisionPredicateFactory.h"

/** @class HltDecisionFilter HltDecisionFilter.h
 *  
 *  functionality:
 *      Filters on HltDecisions
 *
 *  Options:
 *  @author Gerhard Raven
 *  @date   2008-09-02
 */
class HltDecisionFilter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltDecisionFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDecisionFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode restart   ();    ///< Algorithm restart
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  IHltDecisionPredicateFactory                    *m_factory;
  IHltDecisionPredicateFactory::decisionPredicate *m_filter;
  std::string m_filterDescription;
  std::string m_location;
};
#endif // HLTSELECTIONENTRY_H
