// $Id: LbAppInit.h,v 1.1 2005-12-21 12:36:39 cattanem Exp $
#ifndef LBAPPINIT_H 
#define LBAPPINIT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class LbAppInit LbAppInit.h
 *  
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-21
 */
class LbAppInit : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LbAppInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LbAppInit( ); ///< Destructor

  virtual StatusCode initialize();     ///< Algorithm initialization
  virtual StatusCode execute   ();     ///< Algorithm execution
  virtual StatusCode finalize  ();     ///< Algorithm finalization
  void printEventRun( longlong, int ); ///< Prints event and run numbers

protected:

private:
  int  m_eventCounter; ///< Number of events processed
  int  m_eventMax;     ///< Number of events requested (ApplicationMgr.EvtMax)
};

#endif // LBAPPINIT_H
