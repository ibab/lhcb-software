// $Id: TestTrend.h,v 1.1.1.1 2010-06-11 09:58:56 ocallot Exp $
#ifndef TESTTREND_H 
#define TESTTREND_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Trending/ITrendingTool.h"

/** @class TestTrend TestTrend.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2010-06-03
 */
class TestTrend : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TestTrend( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestTrend( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  ITrendingTool* m_trend;
  int  m_event;
};
#endif // TESTTREND_H
