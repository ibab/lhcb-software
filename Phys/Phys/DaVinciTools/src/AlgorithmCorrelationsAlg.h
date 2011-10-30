// $Id$
#ifndef SELRESULTCOMBINATORICS_H 
#define SELRESULTCOMBINATORICS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class AlgorithmCorrelationsAlg AlgorithmCorrelationsAlg.h
 *  
 *  Produces a correlations between algorithms. Uses both
 *  filterPassed() and SelResult (which might be in DST)
 *
 *  Usage :
 *
 *  @code
 *  ApplicationMgr().TopAlg += [ AlgorithmCorrelationsAlg() ]
 *  AlgorithmCorrelationsAlg().Algorithms = [ "Hlt2Decision", 
 *                                            "DiLeptonForPreselBu2LLK",
 *                                            "PreselBu2LLK" ]
 *  @endcode 
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-01
 */
class AlgorithmCorrelationsAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  AlgorithmCorrelationsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AlgorithmCorrelationsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::vector<std::string> m_algorithmsRow ; ///< Algorithms to check
  std::vector<std::string> m_algorithmsColumn ; ///< Algorithms to against (= Row by default)
  std::vector<std::string> m_algorithms ; ///< All Algorithms
  mutable IAlgorithmCorrelations* m_algoCorr ; ///< Correlation tool
  mutable ICheckSelResults* m_selTool ; ///< Selection results tool
  
  bool m_printTable ; ///< print output in Table format
  bool m_printList ; ///< print output in List format
};

#endif // SELRESULTCOMBINATORICS_H
