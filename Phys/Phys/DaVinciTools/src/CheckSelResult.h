// $Id: CheckSelResult.h,v 1.3 2007-09-07 13:25:14 pkoppenb Exp $
#ifndef CHECKSELRESULT_H 
#define CHECKSELRESULT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IWriteSelResult.h"


/** @class CheckSelResult CheckSelResult.h
 *  
 *  Checks SelResults written out by algorithms
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-07-14
 */
class CheckSelResult : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CheckSelResult( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheckSelResult( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  LHCb::SelResults* readSelResult() ;              ///< Read in SelResults, create if needed.
  StatusCode algoLoop(LHCb::SelResults*&);         ///< loop on algos

  std::string m_selResults;                        ///< Location of selresult container
  std::vector<std::string> m_algorithms ;          ///< Algorithms to check
  bool m_ANDmode ;
  bool m_avoidSelResult ;                          ///< avoid selresult writing? default: yes.
  
  long        m_nEvents;                           ///< Counter of events accepted
  long        m_totEvents;                         ///< Counter of events processed
  std::string m_headerPath;                        ///< TES of RecHeader

  IWriteSelResult* m_writeTool ;                  ///< SelResultWriter

};
#endif // CHECKSELRESULT_H
