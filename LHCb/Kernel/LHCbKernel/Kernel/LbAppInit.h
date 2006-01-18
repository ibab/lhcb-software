// $Id: LbAppInit.h,v 1.2 2006-01-18 12:28:26 gcorti Exp $
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
  // Return number of events processed
  int  eventCounter()
  { 
    return m_eventCounter; 
  }
  // Return name of application being run
  const std::string& appName() 
  {
    return m_appName;
  }
  // Return version of application being run
  const std::string& appVersion() 
  {
    return m_appVersion;
  }

private:
  int  m_eventCounter; ///< Number of events processed
  int  m_eventMax;     ///< Number of events requested (ApplicationMgr.EvtMax)
  std::string   m_appName;      ///< Application Name
  std::string   m_appVersion;   ///< Application Version
};

#endif // LBAPPINIT_H
