// $Id$
#ifndef CHECKSELRESULT_H 
#define CHECKSELRESULT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/ICheckSelResults.h"


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

  std::vector<std::string> m_algorithms ;          ///< Algorithms to check
  bool m_ANDmode ;                                 ///< Require all algorithms
  bool m_NOTmode ;                                 ///< Invert logic  

  ICheckSelResults* m_readTool ;                  ///< Selresults Reader

};
#endif // CHECKSELRESULT_H
