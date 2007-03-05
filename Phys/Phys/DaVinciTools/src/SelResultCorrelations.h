// $Id: SelResultCorrelations.h,v 1.1 2007-03-05 09:05:36 pkoppenb Exp $
#ifndef SELRESULTCOMBINATORICS_H 
#define SELRESULTCOMBINATORICS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class SelResultCorrelations SelResultCorrelations.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-01
 */
class SelResultCorrelations : public GaudiAlgorithm {
public: 
  /// Standard constructor
  SelResultCorrelations( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SelResultCorrelations( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_selResults;        ///< Location of mother Particle in TES
  std::vector<std::string> m_algorithmsRow ; ///< Algorithms to check
  std::vector<std::string> m_algorithmsColumn ; ///< Algorithms to against (= Row by default)
  mutable IAlgorithmCorrelations* m_algoCorr ; ///< Correlation tool
  
  bool m_printTable ; ///< print output in Table format
  bool m_printList ; ///< print output in List format
};

#endif // SELRESULTCOMBINATORICS_H
