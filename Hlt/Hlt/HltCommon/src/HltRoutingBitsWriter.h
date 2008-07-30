// $Id: HltRoutingBitsWriter.h,v 1.1 2008-07-30 13:37:32 graven Exp $
#ifndef HLTCOMMON_HLTROUTINGBITSWRITER_H 
#define HLTCOMMON_HLTROUTINGBITSWRITER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltRoutingBitEvaluatorFactory.h"
#include "boost/array.hpp"

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

  enum { NBits = 96 };
  boost::array<HltRoutingBitEvaluatorFactory::evaluator*,NBits> m_evaluators;

  bool evaluate(size_t i) { return m_evaluators[i]!=0 && m_evaluators[i]->evaluate(); }
  std::ostream& print(std::ostream& os, size_t i) { os << " " << i << " : " ; return (m_evaluators[i]!=0) ? m_evaluators[i]->print(os) : (os << "[none]");}

  void updateEvaluators(Property&);
  void updateEvaluators(const HltRoutingBitEvaluatorFactory&);
  void zeroEvaluators() { for(size_t i=0;i<NBits;++i) m_evaluators[i]=0; }

 
  std::map<unsigned int,std::string> m_specifications;

  Hlt::SelectionContainer0       m_selection;
  HltRoutingBitEvaluatorFactory *m_factory;
  bool                           m_updateRequired;


};
#endif // HLTROUTINGBITSWRITER_H
