// $Id: HltRoutingBitsWriter.h,v 1.4 2008-09-04 13:43:05 graven Exp $
#ifndef HLTCOMMON_HLTROUTINGBITSWRITER_H 
#define HLTCOMMON_HLTROUTINGBITSWRITER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/IHltSelectionCombinerFactory.h"
#include "boost/array.hpp"
#include <algorithm>

/** @class HltRoutingBitsWriter HltRoutingBitsWriter.h
 *  
 *
 *  @author Gerhard Raven
 *  @date   2008-07-29
 */
class HltRoutingBitsWriter : public HltAlgorithm {
public: 
  /// Standard constructor
  HltRoutingBitsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRoutingBitsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
private:

  typedef IHltSelectionCombinerFactory::selectionCombiner combiner_t;
  boost::array<combiner_t*,3*sizeof(unsigned int)*8> m_evaluators;

  bool evaluate(size_t i) { return m_evaluators[i]!=0 && m_evaluators[i]->evaluate(); }
  std::ostream& print(std::ostream& os, size_t i) 
  { os << " " << i << " : " ; 
    return (m_evaluators[i]!=0) ? m_evaluators[i]->print(os) : (os << "[none]");}

  void updateEvaluators(Property&);
  void updateEvaluators(const IHltSelectionCombinerFactory&);
  void zeroEvaluators() { std::fill(m_evaluators.begin(),m_evaluators.end(),(combiner_t*)0); }
 
  std::map<unsigned int,std::string> m_specifications;

  Hlt::SelectionContainer0       m_selection;
  IHltSelectionCombinerFactory  *m_factory;
  bool                           m_updateRequired;


};
#endif // HLTROUTINGBITSWRITER_H
