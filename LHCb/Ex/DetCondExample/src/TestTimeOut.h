// $Id: TestTimeOut.h,v 1.1 2006-08-31 11:48:14 marcocle Exp $
#ifndef TESTTIMEOUT_H 
#define TESTTIMEOUT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class TestTimeOut TestTimeOut.h
 *  
 *  Algorithm used to test the connection time-out functionality od CondDBAccessSvc().
 *
 *  @author Marco CLEMENCIC
 *  @date   2006-08-31
 */
class TestTimeOut : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TestTimeOut( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestTimeOut( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  
  /// How many seconds to sleep (option TestTimeOut.SleepTime, default = 20).
  int m_sleepTime;
  
};
#endif // TESTTIMEOUT_H
