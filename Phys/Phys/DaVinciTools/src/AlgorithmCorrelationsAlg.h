// $Id: AlgorithmCorrelationsAlg.h,v 1.1 2008-10-29 13:56:04 pkoppenb Exp $
#ifndef SELRESULTCOMBINATORICS_H 
#define SELRESULTCOMBINATORICS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class AlgorithmCorrelationsAlg AlgorithmCorrelationsAlg.h
 *  
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
  /// method to test if algorithm passed selresult
  bool selResultPassed(std::string,const LHCb::SelResults*);

private:
  bool m_useSelResults ;    ///< Use SelResults, i.e. look in DST if algorithm is not run in process.
  std::string m_selResults;        ///< Location of selresults container
  std::vector<std::string> m_algorithmsRow ; ///< Algorithms to check
  std::vector<std::string> m_algorithmsColumn ; ///< Algorithms to against (= Row by default)
  std::vector<std::string> m_algorithms ; ///< All Algorithms
  mutable IAlgorithmCorrelations* m_algoCorr ; ///< Correlation tool
  
  bool m_printTable ; ///< print output in Table format
  bool m_printList ; ///< print output in List format
};

#endif // SELRESULTCOMBINATORICS_H
