// $Id: CheckSelResult.h,v 1.1 2007-03-05 09:05:35 pkoppenb Exp $
#ifndef CHECKSELRESULT_H 
#define CHECKSELRESULT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


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

  StatusCode writeSelResult(LHCb::SelResults* ) ;

  std::string m_selResults;  ///< Location of selresult
  std::vector<std::string> m_algorithms ; ///< Algorithms to check
  bool m_ANDmode ;
  bool m_avoidSelResult ;  ///< avoid selresult writing?
  
  long        m_nEvents;             ///< Counter of events accepted
  long        m_totEvents;            ///< Counter of events processed
  std::string m_headerPath;          ///< TES of EventHeader

};
#endif // CHECKSELRESULT_H
